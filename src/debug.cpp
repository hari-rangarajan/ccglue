#include <stdio.h>
#include <stdarg.h>
#include "debug.h"

debug::debug () 
{
}

void debug::log (const char * fmt, ...)
{
#if 0
  char buffer[256];
  va_list args;
  va_start (args, fmt);
  vsprintf (buffer,fmt, args);
  va_end (args);
  printf("%s", buffer);
#endif
}


