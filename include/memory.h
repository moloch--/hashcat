/**
 * Author......: See docs/credits.txt
 * License.....: MIT
 */

#ifndef HC_MEMORY_H
#define HC_MEMORY_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MSG_ENOMEM "Insufficient memory available"

void *hccalloc                (const size_t nmemb, const size_t sz);
void *hcmalloc                (const size_t sz);
void *hcrealloc               (void *ptr, const size_t oldsz, const size_t addsz);
char *hcstrdup                (const char *s);
void  hcfree                  (void *ptr);

void *hc_alloc_aligned        (size_t alignment, size_t size);
void  hc_free_aligned         (void **ptr);

void *hcmalloc_bridge_aligned (const size_t sz, const int align);
void  hcfree_bridge_aligned   (void *ptr);

#endif // HC_MEMORY_H
