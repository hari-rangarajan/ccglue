bin_PROGRAMS = ccglue
ccglue_SOURCES = options.c cscoperdr.cpp seqfile.cpp sym_mgr.cpp digraph.cpp main.cpp c_glue.cpp  tag_file_writer.cpp debug.cpp
AM_CFLAGS = $(GLIB_CFLAGS)
ccglue_LDADD = $(GLIB_LIBS)
INCLUDES = `pkg-config --cflags glib-2.0`
