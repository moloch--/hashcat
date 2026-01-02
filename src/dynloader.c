/**
 * Author......: See docs/credits.txt
 * License.....: MIT
 */

#include "common.h"
#include "types.h"
#include "dynloader.h"

#ifdef _WIN

hc_dynlib_t hc_dlopen (LPCSTR lpLibFileName)
{
  return LoadLibraryA (lpLibFileName);
}

BOOL hc_dlclose (hc_dynlib_t hLibModule)
{
  return FreeLibrary (hLibModule);
}

hc_dynfunc_t hc_dlsym (hc_dynlib_t hModule, LPCSTR lpProcName)
{
  return GetProcAddress (hModule, lpProcName);
}

char *hc_dlerror ()
{
  char *msg = NULL;

  DWORD rc = GetLastError ();

  FormatMessageA
  (
    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL,
    rc,
    MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),
    (LPSTR) &msg,
    0,
    NULL
  );

  return msg;
}

#else

hc_dynlib_t hc_dlopen (const char *filename)
{
  return dlopen (filename, RTLD_NOW);
}

int hc_dlclose (hc_dynlib_t handle)
{
  return dlclose (handle);
}

hc_dynfunc_t hc_dlsym (hc_dynlib_t handle, const char *symbol)
{
  return dlsym (handle, symbol);
}

char *hc_dlerror ()
{
  return dlerror ();
}

#endif
