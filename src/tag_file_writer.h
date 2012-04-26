#ifndef TAG_FILE_WRITER_H
#define TAG_FILE_WRITER_H

#include "sym_mgr.h"
#include "typedefs.h"
#include <fstream>

class sym_entry;

class tag_file_writer {
    protected:
        std::ofstream tagfile;
    public:
        tag_file_writer(const std::string& filename);
        ~tag_file_writer();
        void destroy();
        void write_xref_tag_header();
        void write_sym_as_tag(sym_entry *a_sym_entry);
};

#endif
