/**
 * Author......: See docs/credits.txt
 * License.....: MIT
 */

#ifndef HC_SELFTEST_H
#define HC_SELFTEST_H

#if defined (_WIN32) || defined (__WIN32__)
HC_API_CALL DWORD thread_selftest (void *p);
#else
HC_API_CALL void *thread_selftest (void *p);
#endif

#endif // HC_SELFTEST_H
