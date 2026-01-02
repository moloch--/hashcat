/**
 * Author......: See docs/credits.txt
 * License.....: MIT
 */

#include "common.h"
#include "types.h"
#include "generic.h"

const int GENERIC_PLUGIN_VERSION = GENERIC_PLUGIN_VERSION_REQ;
const int GENERIC_PLUGIN_OPTIONS = 0;

bool global_init (MAYBE_UNUSED generic_global_ctx_t *global_ctx, MAYBE_UNUSED generic_thread_ctx_t **thread_ctx, MAYBE_UNUSED hashcat_ctx_t *hashcat_ctx)
{
  return true;
}

void global_term (MAYBE_UNUSED generic_global_ctx_t *global_ctx, MAYBE_UNUSED generic_thread_ctx_t **thread_ctx, MAYBE_UNUSED hashcat_ctx_t *hashcat_ctx)
{
}

u64 global_keyspace (MAYBE_UNUSED generic_global_ctx_t *global_ctx, MAYBE_UNUSED generic_thread_ctx_t **thread_ctx, MAYBE_UNUSED hashcat_ctx_t *hashcat_ctx)
{
  return -1ULL;
}

bool thread_init (MAYBE_UNUSED generic_global_ctx_t *global_ctx, MAYBE_UNUSED generic_thread_ctx_t *thread_ctx)
{
  return true;
}

void thread_term (MAYBE_UNUSED generic_global_ctx_t *global_ctx, MAYBE_UNUSED generic_thread_ctx_t *thread_ctx)
{
}

int thread_next (MAYBE_UNUSED generic_global_ctx_t *global_ctx, MAYBE_UNUSED generic_thread_ctx_t *thread_ctx, MAYBE_UNUSED u8 *out_buf)
{
  return 9;
}

bool thread_seek (MAYBE_UNUSED generic_global_ctx_t *global_ctx, MAYBE_UNUSED generic_thread_ctx_t *thread_ctx, MAYBE_UNUSED const u64 offset)
{
  return true;
}
