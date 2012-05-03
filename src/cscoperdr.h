#ifndef CSCOPERDR_H
#define CSCOPERDR_H

#include "sym_mgr.h"
#include "typedefs.h"
#include <fstream>


enum {
    ACTION_LOAD_SYMS = 0,
    ACTION_XREF_SYMS
};

class generic_db_rdr {
    public:
        virtual void set_scan_action(int action) = 0;
        virtual void process_line(sym_table&, char* ) = 0;
};


class cscope_db_rdr: public generic_db_rdr {
    protected:
        sym_entry   *m_in_func;
        sym_entry   *m_in_file;
        sym_entry   *m_in_macro;
        sym_entry   *m_in_enum;

//        std::ifstream     m_cscope_db;
        int          m_action_type;
    public:
        cscope_db_rdr();
        ~cscope_db_rdr();
        void process_line(sym_table&, char* );
        void build_sym_table(sym_table&, char* line);
        void build_xref(sym_table&, char* line);
        void build_xref_from_line(sym_table&, char* line);
        void build_sym_from_line(sym_table&, char* line);
        void set_scan_action(int action);
        sym_entry* create_sym_entry_or_lookup (sym_table&,
                const char* sym_text);
};



#endif

