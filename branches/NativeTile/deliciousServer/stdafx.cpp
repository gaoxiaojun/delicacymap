// stdafx.cpp : source file that includes just the standard includes
// deliciousServer.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

#ifdef _DO_NOT_HAVE_S
int sprintf_s(
   char *buffer,
   size_t sizeOfBuffer,
   const char *format ,...)
{
    va_list ap;
    va_start(ap, format);
    int res = vsprintf(buffer, format, ap);
    va_end(ap);
    return res;
}
#endif

