// debug.cpp
#include "debug.h"
#include "../config.h"

nullstream null;

std::ostream& debug(short level)
{
#if 0
    return std::cout;
#else 
    return null;
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
