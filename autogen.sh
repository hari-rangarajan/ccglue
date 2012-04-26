#!/bin/sh
rm -f config.cache
rm -f acconfig.h
aclocal -I config
autoconf
acconfig
autoheader
automake -a
exit

