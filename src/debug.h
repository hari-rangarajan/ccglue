// debug.hpp
#ifndef _DEBUG_H_
#define _DEBUG_H_

#include  <iostream>

// this defines a struct named options and
// an instance of it called "opts"

struct nullstream : std::ostream
{
  struct nullbuf : std::streambuf
  {
    int overflow(int c) { return traits_type::not_eof(c); }
  } _sbuf;

  nullstream()
    : std::ios(&_sbuf), std::ostream(&_sbuf) {}
};

template<class T>
nullstream& operator<<(nullstream& ns, T)
{ return ns; }

//extern options opts;
extern nullstream null;

std::ostream& debug(short level);
std::ostream& warn();
std::ostream& error();

#endif

