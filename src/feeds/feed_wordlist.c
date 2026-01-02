/**
 * Author......: See docs/credits.txt
 * License.....: MIT
 */

#include "common.h"
#include "types.h"
#include "memory.h"
#include "convert.h"
#include "filehandling.h"
#include "folder.h"
#include "shared.h"
#include "timer.h"
#include "event.h"
#include "xxhash.h"
#include "generic.h"
#include "feed_wordlist.h"

#if defined (_WIN)
#include "mmap_windows.c"
#else
#include <sys/mman.h>
#endif

#include "seekdb.c"

const int GENERIC_PLUGIN_VERSION = GENERIC_PLUGIN_VERSION_REQ;

const int GENERIC_PLUGIN_OPTIONS = GENERIC_PLUGIN_OPTIONS_AUTOHEX
                                 | GENERIC_PLUGIN_OPTIONS_ICONV
                                 | GENERIC_PLUGIN_OPTIONS_RULES;

static void error_set (generic_global_ctx_t *global_ctx, const char *fmt, ...)
{
  global_ctx->error = true;

  va_list ap;
  va_start (ap, fmt);

  vsnprintf (global_ctx->error_msg, sizeof (global_ctx->error_msg), fmt, ap);

  va_end (ap);
}

static size_t process_word (const u8 *buf, const size_t len, u8 *out_buf)
{
  size_t word_len = len;

  while ((word_len > 0) && (buf[word_len - 1] == '\r')) word_len--;

  // important: the output buffer, provided by hashcat, has only space for PW_MAX (256)

  const size_t report_len = MIN (word_len, PW_MAX);

  memcpy ((char *) out_buf, buf, report_len);

  return report_len;
}

bool global_init (MAYBE_UNUSED generic_global_ctx_t *global_ctx, MAYBE_UNUSED generic_thread_ctx_t **thread_ctx, MAYBE_UNUSED hashcat_ctx_t *hashcat_ctx)
{
  // create our own context

  feed_global_t *feed_global = hcmalloc (sizeof (feed_global_t));

  global_ctx->gbldata = feed_global;

  // check user command line arguments

  if (global_ctx->workc < 2)
  {
    error_set (global_ctx, "Invalid parameter count: %d. Count must be at least 2.", global_ctx->workc);

    return false;
  }

  feed_global->wordlist   = global_ctx->workv[1];
  feed_global->seek_db    = NULL;
  feed_global->seek_count = 0;
  feed_global->line_count = 0;
  feed_global->size       = 0;

  return true;
}

void global_term (MAYBE_UNUSED generic_global_ctx_t *global_ctx, MAYBE_UNUSED generic_thread_ctx_t **thread_ctx, MAYBE_UNUSED hashcat_ctx_t *hashcat_ctx)
{
  feed_global_t *feed_global = global_ctx->gbldata;

  if (feed_global->seek_db) hcfree (feed_global->seek_db);

  hcfree (feed_global);

  global_ctx->gbldata = NULL;
}

u64 global_keyspace (MAYBE_UNUSED generic_global_ctx_t *global_ctx, MAYBE_UNUSED generic_thread_ctx_t **thread_ctx, MAYBE_UNUSED hashcat_ctx_t *hashcat_ctx)
{
  feed_global_t *feed_global = global_ctx->gbldata;

  char *seekdb_file = seekdb_path (global_ctx, feed_global->wordlist);

  feed_global->seek_db = seekdb_load (seekdb_file, &feed_global->seek_count, &feed_global->line_count, &feed_global->size);

  if (feed_global->seek_db)
  {
    cache_hit_t cache_hit;

    cache_hit.dictfile      = feed_global->wordlist;
    cache_hit.stat.st_size  = feed_global->size;
    cache_hit.cached_cnt    = feed_global->line_count;
    cache_hit.keyspace      = feed_global->line_count;

    EVENT_DATA (EVENT_WORDLIST_CACHE_HIT, &cache_hit, sizeof (cache_hit));

    hcfree (seekdb_file);

    return feed_global->line_count;
  }

  thread_init (global_ctx, thread_ctx[0]);

  feed_thread_t *feed_thread = thread_ctx[0]->thrdata;

  hc_timer_t start;

  hc_timer_set (&start);

  feed_global->seek_db = seekdb_build (feed_thread, seekdb_file, feed_global->wordlist, &feed_global->seek_count, &feed_global->line_count, &feed_global->size, hashcat_ctx);

  cache_generate_t cache_generate;

  cache_generate.dictfile    = feed_global->wordlist;
  cache_generate.comp        = feed_global->size;
  cache_generate.percent     = 100;
  cache_generate.cnt         = feed_global->line_count;
  cache_generate.cnt2        = feed_global->line_count;
  cache_generate.runtime     = hc_timer_get (start);

  EVENT_DATA (EVENT_WORDLIST_CACHE_GENERATE, &cache_generate, sizeof (cache_generate));

  hcfree (seekdb_file);

  thread_term (global_ctx, thread_ctx[0]);

  return feed_global->line_count;
}

bool thread_init (MAYBE_UNUSED generic_global_ctx_t *global_ctx, MAYBE_UNUSED generic_thread_ctx_t *thread_ctx)
{
  feed_global_t *feed_global = global_ctx->gbldata;

  feed_thread_t *feed_thread = hcmalloc (sizeof (feed_thread_t));

  if (feed_thread == NULL)
  {
    error_set (global_ctx, "hcmalloc failed");

    return false;
  }

  thread_ctx->thrdata = feed_thread;

  if (hc_fopen_raw (&feed_thread->hcfile, feed_global->wordlist, "rb") == false)
  {
    error_set (global_ctx, "%s: %s", feed_global->wordlist, strerror (errno));

    return false;
  }

  struct stat s;

  if (hc_fstat (&feed_thread->hcfile, &s) == -1)
  {
    error_set (global_ctx, "%s: %s", feed_global->wordlist, strerror (errno));

    return false;
  }

  if (s.st_size == 0)
  {
    error_set (global_ctx, "%s: zero size", feed_global->wordlist);

    return false;
  }

  feed_thread->fd_off = 0;
  feed_thread->fd_len = s.st_size;

  void *fd_mem = mmap (NULL, feed_thread->fd_len, PROT_READ, MAP_PRIVATE, feed_thread->hcfile.fd, 0);

  if (fd_mem == MAP_FAILED)
  {
    error_set (global_ctx, "%s: mmap failed", feed_global->wordlist);

    return false;
  }

  feed_thread->fd_mem = fd_mem;
  feed_thread->fd_line = 0;

  // kernel advice

  #if !defined (_WIN)
  #ifdef POSIX_MADV_SEQUENTIAL
  posix_madvise (feed_thread->fd_mem, feed_thread->fd_len, POSIX_MADV_SEQUENTIAL);
  #endif
  #endif

  return true;
}

void thread_term (MAYBE_UNUSED generic_global_ctx_t *global_ctx, MAYBE_UNUSED generic_thread_ctx_t *thread_ctx)
{
  feed_thread_t *feed_thread = thread_ctx->thrdata;

  munmap (feed_thread->fd_mem, feed_thread->fd_len);

  hc_fclose (&feed_thread->hcfile);

  hcfree (feed_thread);

  thread_ctx->thrdata = NULL;
}

int thread_next (MAYBE_UNUSED generic_global_ctx_t *global_ctx, MAYBE_UNUSED generic_thread_ctx_t *thread_ctx, u8 *out_buf)
{
  feed_thread_t *feed_thread = thread_ctx->thrdata;

  const u8      *fd_mem = feed_thread->fd_mem;
  const size_t   fd_len = feed_thread->fd_len;
  size_t         fd_off = feed_thread->fd_off;

  if (fd_off >= fd_len)
  {
    error_set (global_ctx, "next fd_off >= fd_len: %zu:%zu", fd_off, fd_len);

    return -1;
  }

  hc_memchr_t hc_memchr = hc_memchr_get ();

  size_t remaining = fd_len - fd_off;
  size_t step      = hc_memchr (fd_mem + fd_off, '\n', remaining);

  // if no newline, process till EOF
  if (step == remaining)
  {
    size_t word_len = process_word (fd_mem + fd_off, step, out_buf);

    feed_thread->fd_off += step;
    feed_thread->fd_line++;

    return (int) word_len;
  }

  // found newline
  size_t word_len = process_word (fd_mem + fd_off, step, out_buf);

  feed_thread->fd_off += step + 1; // +1 = skip '\n'
  feed_thread->fd_line++;

  return (int) word_len;
}

bool thread_seek (MAYBE_UNUSED generic_global_ctx_t *global_ctx, MAYBE_UNUSED generic_thread_ctx_t *thread_ctx, const u64 offset)
{
  feed_thread_t *feed_thread = thread_ctx->thrdata;
  feed_global_t *feed_global = global_ctx->gbldata;

  const u8      *fd_mem = feed_thread->fd_mem;
  const size_t   fd_len = feed_thread->fd_len;

  if (offset >= feed_global->line_count)
  {
    error_set (global_ctx, "seek target past EOF: %zu", (size_t) offset);

    return false;
  }

  u64 idx = offset / SEEKDB_STEP;

  if ((feed_global->seek_db) && (idx < feed_global->seek_count))
  {
    feed_thread->fd_off  = feed_global->seek_db[idx];
    feed_thread->fd_line = idx * SEEKDB_STEP;
  }

  hc_memchr_t hc_memchr = hc_memchr_get ();

  while (feed_thread->fd_line < offset)
  {
    size_t remaining = fd_len - feed_thread->fd_off;

    if (remaining == 0)
    {
      error_set (global_ctx, "Seek past EOF");

      return false;
    }

    size_t step = hc_memchr (fd_mem + feed_thread->fd_off, '\n', remaining);

    feed_thread->fd_off += step + 1; // +1 for '\n'
    feed_thread->fd_line++;
  }

  return true;
}
