#ifndef CSCOPERDR_H
#define CSCOPERDR_H

#include "sym_mgr.h"
#include "typedefs.h"
#include <fstream>
#include "lexertl/rules.hpp"
#include "lexertl/generator.hpp"
#include "lexertl/lookup.hpp"


enum {
    ACTION_LOAD_SYMS = 0,
    ACTION_XREF_SYMS
};


class generic_db_scanner {
    public:
        generic_db_scanner();
        const lexertl::state_machine& get_state_machine() {
            return sm_;
        };
        virtual void    initialize_rules() {};
    protected:
        lexertl::rules  rules_;
        lexertl::state_machine sm_;
};

class generic_db_rdr {
    public:
        virtual void process_lines(sym_table& , const char* , int , generic_db_scanner& )=0;
};

class cscope_db_rdr_context {
    public: 
        cscope_db_rdr_context ();
        sym_entry              *m_in_func;
        sym_entry              *m_in_file;
        sym_entry              *m_in_macro;
        sym_entry              *m_in_enum;
        sym_loc_line_number_t   m_line_num;

        int                     last_token_id;
};

class cscope_db_symbol_scanner:public generic_db_scanner {
    public:
        void initialize_rules();
};

class cscope_db_xref_scanner:public generic_db_scanner {
    public:
        void initialize_rules();
};

/* move out */
class cscope_db_rdr: public generic_db_rdr {
    protected:
        cscope_db_rdr_context   ctxt;
        int                     m_action_type;

    public:
        cscope_db_rdr();
        ~cscope_db_rdr();
        void process_lines (sym_table& a_sym_table, const char* data,
                int size, generic_db_scanner& scanner);
        void build_sym_table(sym_table&, char* line);
        void build_xref(sym_table&, char* line);
        void build_xref_on_token(sym_table&, std::string&);
        bool build_syms_from_token (sym_table& a_sym_table, 
                    long unsigned int token_type, std::string& token);
        void process_token_on_line(sym_table&, std::string&);
        void set_scan_action(int action);
        sym_entry* create_sym_entry_or_lookup (sym_table&,
                const std::string& sym_text);
};



#endif

