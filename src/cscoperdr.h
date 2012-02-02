#ifndef CSCOPERDR_H
#define CSCOPERDR_H

#include "sym_mgr.h"
#include "typedefs.h"
#include "seqfile.h"


enum {
    ACTION_LOAD_SYMS = 0,
    ACTION_XREF_SYMS
};

class generic_db_rdr {
    public:
        virtual void set_scan_action(int action) = 0;
        virtual void process_line(sym_table *, uchar* ) = 0;
};


class cscope_db_rdr: public generic_db_rdr {
    protected:
        sym_entry   *m_in_func;
        sym_entry   *m_in_file;
        sym_entry   *m_in_macro;
        sym_entry   *m_in_enum;

        seq_file    *m_cscope_db;
        int          m_action_type;
    public:
        cscope_db_rdr();
        ~cscope_db_rdr();
        RC_t open(const char *cscope_db_name);
        void destroy();
        void process_line(sym_table *, uchar* );
        void build_sym_table(sym_table *a_sym_table, uchar* line);
        void build_xref(sym_table *a_sym_table, uchar* line);
        void build_xref_from_line(sym_table *a_sym_table, uchar* line);
        void build_sym_from_line(sym_table *a_sym_table, uchar* line);
        void set_scan_action(int action);
};



#endif

