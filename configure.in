PACKAGE=ccglue
VERSION=0.6.0
AUTHOR=hariranga@sourceforge.users.net

AC_INIT(ccglue, 0.6.0, hariranga@sourceforge.users.net)
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_SRCDIR(src/main.cpp)
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE


# borrowed from http://ubuntuforums.org/showthread.php?t=1556520
# prevent setting xFLAGS to default of -g -O2
if test x"$CXXFLAGS" = x""; then
    AC_SUBST(CXXFLAGS, [-fno-rtti])
fi
AC_PROG_CXX

AC_ARG_ENABLE([debug], AC_HELP_STRING([--enable-debug],
                     [compile with debug symbols @<:@default=no@:>@]),
                     [want_debug="$enableval"], [want_debug=no])

if test "x$want_debug" = "xyes"; then
    CXXFLAGS="$CXXFLAGS -O0 -ggdb"
    AC_DEFINE([DEBUG], 1, [Define for debugging])
else
    CXXFLAGS="$CXXFLAGS -O2"
fi

AC_FUNC_MMAP
AC_CXX_HAVE_SSTREAM
AC_CXX_HAVE_STRSTREAM
AC_CXX_HAVE_LONG_LONG
AC_PROG_INSTALL
AC_CONFIG_FILES([Makefile src/Makefile doc/Makefile])
AC_OUTPUT

