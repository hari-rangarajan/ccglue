// debug.cpp
#include "debug.h"

nullstream null;

std::ostream& debug(short level)
{
    return null;
#if 0
  return std::cout;
#endif
}

std::ostream& warn()
{
  return std::cout << "[warn] ";
}

std::ostream& error()
{
  return std::cerr << "[error] ";
}
