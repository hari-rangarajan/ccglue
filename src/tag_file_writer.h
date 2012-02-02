#ifndef TAG_FILE_WRITER_H
#define TAG_FILE_WRITER_H

#include "sym_mgr.h"
#include "seqfile.h"
#include "typedefs.h"

class sym_entry;

class tag_file_writer {
    protected:
        seq_file *tagfile;
    public:
        tag_file_writer(const char* filename);
        ~tag_file_writer();
        void destroy();
        void write_xref_tag_header();
        void write_sym_as_tag(sym_entry *a_sym_entry);
};

#endif
