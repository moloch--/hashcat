/**
 * Author......: See docs/credits.txt
 * License.....: MIT
 */

#ifndef FEED_WORDLIST_H
#define FEED_WORDLIST_H

#ifndef O_BINARY
#define O_BINARY 0
#endif

typedef struct feed_global
{
  char *wordlist;

  u64   *seek_db;
  u64    seek_count;
  u64    line_count;
  u64    size;

} feed_global_t;

typedef struct feed_thread
{
  HCFILE hcfile;

  size_t fd_off;
  size_t fd_len;
  void  *fd_mem;
  u64    fd_line;

} feed_thread_t;

bool global_init      (MAYBE_UNUSED generic_global_ctx_t *global_ctx, MAYBE_UNUSED generic_thread_ctx_t **thread_ctx, MAYBE_UNUSED hashcat_ctx_t *hashcat_ctx);
void global_term      (MAYBE_UNUSED generic_global_ctx_t *global_ctx, MAYBE_UNUSED generic_thread_ctx_t **thread_ctx, MAYBE_UNUSED hashcat_ctx_t *hashcat_ctx);
u64  global_keyspace  (MAYBE_UNUSED generic_global_ctx_t *global_ctx, MAYBE_UNUSED generic_thread_ctx_t **thread_ctx, MAYBE_UNUSED hashcat_ctx_t *hashcat_ctx);

bool thread_init      (MAYBE_UNUSED generic_global_ctx_t *global_ctx, MAYBE_UNUSED generic_thread_ctx_t *thread_ctx);
void thread_term      (MAYBE_UNUSED generic_global_ctx_t *global_ctx, MAYBE_UNUSED generic_thread_ctx_t *thread_ctx);
int  thread_next      (MAYBE_UNUSED generic_global_ctx_t *global_ctx, MAYBE_UNUSED generic_thread_ctx_t *thread_ctx, u8 *out_buf);
bool thread_seek      (MAYBE_UNUSED generic_global_ctx_t *global_ctx, MAYBE_UNUSED generic_thread_ctx_t *thread_ctx, const u64 offset);

#endif // FEED_WORDLIST_H
