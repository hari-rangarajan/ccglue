/* 
 *   ccglue - cscope,ctags glue: Process ctags and cscope output to
 *        produce cross-reference table (for use with Vim CCTree plugin)
 *        and more ...
 *   Copyright (C) April, 2011,  Hari Rangarajan 
 *   Contact: hariranga@users.sourceforge.net

 *   "ccglue" is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.

 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.

 *   You should have received a copy of the GNU General Public License
 *   along with "ccglue".  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "cscoperdr.h"
#include <stdio.h>
#include <string.h>
#include "misc_util.h"
#include "debug.h"
#include "memory_file.hpp"

#include "lexertl/rules.hpp"
#include "lexertl/state_machine.hpp"
#include "lexertl/generator.hpp"
#include "lexertl/stream_shared_iterator.hpp"

struct cscope_tag_marker_ids {
    enum {
            none = 0,
            enum_type = 1,
            function_def,
            function_call,
            function_end,
            begin_macro,
            end_macro,
            include_file,
            start_of_file,
            member_def,
            global,
    };
};

struct cscope_token_ids {
    enum {
            symbol = 100,
            keyword,
            line_number,
    };
};

class cscope_db_tag {
    public:
        cscope_db_tag (const char* _marker, int _tag_id):
            tag_marker(_marker), tag_id(_tag_id) { };
        const std::string& get_tag_marker() const {return tag_marker;};
        const int          get_tag_id() const {return tag_id;};
    private:
        std::string     tag_marker;
        int             tag_id;
};

class cscope_db_tags {
    public:
        static const cscope_db_tag global;
        static const cscope_db_tag enum_type;
        static const cscope_db_tag member_def;
        static const cscope_db_tag end_macro;
        static const cscope_db_tag begin_macro;
        static const cscope_db_tag function_end;
        static const cscope_db_tag include_file_sys;
        static const cscope_db_tag start_of_file;
        static const cscope_db_tag function_call;
        static const cscope_db_tag function_def;
        static const cscope_db_tag include_file;

        cscope_db_tags () {
        };
    private:    
};

const cscope_db_tag cscope_db_tags::global("g", cscope_tag_marker_ids::global);
const cscope_db_tag cscope_db_tags::enum_type("e", cscope_tag_marker_ids::enum_type);
const cscope_db_tag cscope_db_tags::member_def("m", cscope_tag_marker_ids::member_def);
const cscope_db_tag cscope_db_tags::end_macro("\\)", cscope_tag_marker_ids::end_macro);
const cscope_db_tag cscope_db_tags::begin_macro("#", cscope_tag_marker_ids::begin_macro);
const cscope_db_tag cscope_db_tags::function_end("}", cscope_tag_marker_ids::function_end);
const cscope_db_tag cscope_db_tags::include_file_sys("~<", cscope_tag_marker_ids::include_file);
const cscope_db_tag cscope_db_tags::start_of_file("@", cscope_tag_marker_ids::start_of_file);
const cscope_db_tag cscope_db_tags::function_call("`", cscope_tag_marker_ids::function_call);
const cscope_db_tag cscope_db_tags::function_def("\\$", cscope_tag_marker_ids::function_def);
const cscope_db_tag cscope_db_tags::include_file("~\\\"", cscope_tag_marker_ids::include_file);


cscope_db_rdr_context::cscope_db_rdr_context ():
    m_in_func(NULL),
    m_in_file(NULL),
    m_in_macro(NULL),
    m_in_enum(NULL),
    m_line_num(0)
{
}

generic_db_scanner::generic_db_scanner ()
{
    rules_.add_state("TAB_START");
    rules_.add_state("LINE_NO_START");
    rules_.add_state("SYMBOL_TOKEN");
    initialize_rules();
}


void cscope_db_symbol_scanner::initialize_rules ()
{
    rules_.add("INITIAL", "^\\t", sm_.skip(), "TAB_START");

    // g
    rules_.add("TAB_START", cscope_db_tags::global.get_tag_marker(), 
            cscope_db_tags::global.get_tag_id(), "SYMBOL_TOKEN");
    // e
    rules_.add("TAB_START", cscope_db_tags::enum_type.get_tag_marker(), 
            cscope_db_tags::enum_type.get_tag_id(), "SYMBOL_TOKEN");
    // m
    rules_.add("TAB_START", cscope_db_tags::member_def.get_tag_marker(), 
            cscope_db_tags::member_def.get_tag_id(), "SYMBOL_TOKEN");
    // @
    rules_.add("TAB_START", cscope_db_tags::start_of_file.get_tag_marker(), 
            cscope_db_tags::start_of_file.get_tag_id(), "SYMBOL_TOKEN");
    //#
    rules_.add("TAB_START", cscope_db_tags::begin_macro.get_tag_marker(), 
            cscope_db_tags::begin_macro.get_tag_id(), "SYMBOL_TOKEN");
    //) (end of macro)
    rules_.add("TAB_START", cscope_db_tags::end_macro.get_tag_marker(), 
            cscope_db_tags::end_macro.get_tag_id(), "INITIAL");
    // $
    rules_.add("TAB_START", cscope_db_tags::function_def.get_tag_marker(), 
            cscope_db_tags::function_def.get_tag_id(), "SYMBOL_TOKEN");
    // } (end of function)
    rules_.add("TAB_START", cscope_db_tags::function_end.get_tag_marker(), 
            cscope_db_tags::function_end.get_tag_id(), "INITIAL");

    rules_.add("TAB_START", ".", sm_.skip(), "SYMBOL_TOKEN");
    rules_.add("SYMBOL_TOKEN", ".+", cscope_token_ids::symbol, "INITIAL");
    rules_.add("SYMBOL_TOKEN", "\\n", sm_.skip(), "INITIAL");
    rules_.add("TAB_START", "\\n", sm_.skip(), "INITIAL");
    
    rules_.add("INITIAL", "^\\t", sm_.skip(), "SYMBOL_TOKEN");
    rules_.add("INITIAL", "\\n", sm_.skip(), "." );
    rules_.add("INITIAL", "^[^\\t\\n].*", sm_.skip(), "." );

    lexertl::generator::build (rules_, sm_);
}


void cscope_db_xref_scanner::initialize_rules()
{
    rules_.add("INITIAL", "^\\n", 1017, ".");
    rules_.add("INITIAL", "\\n", 1018, ".");
    rules_.add("INITIAL", "^\\d+", cscope_token_ids::line_number, 
            "LINE_NO_START");
    rules_.add("INITIAL", "^[^\\t\\d]", "LINE_NO_START");
    rules_.add("INITIAL", "^\\t", sm_.skip(), "TAB_START");

    // g
    rules_.add("TAB_START", cscope_db_tags::global.get_tag_marker(), 
            cscope_db_tags::global.get_tag_id(), "SYMBOL_TOKEN");
    // e
    rules_.add("TAB_START", cscope_db_tags::enum_type.get_tag_marker(), 
            cscope_db_tags::enum_type.get_tag_id(), "SYMBOL_TOKEN");
    // m
    rules_.add("TAB_START", cscope_db_tags::member_def.get_tag_marker(), 
            cscope_db_tags::member_def.get_tag_id(), "SYMBOL_TOKEN");
    // @
    rules_.add("TAB_START", cscope_db_tags::start_of_file.get_tag_marker(), 
            cscope_db_tags::start_of_file.get_tag_id(), "SYMBOL_TOKEN");
    //#
    rules_.add("TAB_START", cscope_db_tags::begin_macro.get_tag_marker(), 
            cscope_db_tags::begin_macro.get_tag_id(), "SYMBOL_TOKEN");
    // $
    rules_.add("TAB_START", cscope_db_tags::function_def.get_tag_marker(), 
            cscope_db_tags::function_def.get_tag_id(), "SYMBOL_TOKEN");
    // ~ (include file ")
    rules_.add("TAB_START", cscope_db_tags::include_file.get_tag_marker(), 
            cscope_db_tags::include_file.get_tag_id(), "SYMBOL_TOKEN");
    // ~< (include file sys)
    rules_.add("TAB_START", cscope_db_tags::include_file_sys.get_tag_marker(), 
            cscope_db_tags::include_file_sys.get_tag_id(), "SYMBOL_TOKEN");
    // ' (xref call)
    rules_.add("TAB_START", cscope_db_tags::function_call.get_tag_marker(), 
            cscope_db_tags::function_call.get_tag_id(), "SYMBOL_TOKEN");
    
    // } (end of function)
    rules_.add("TAB_START", cscope_db_tags::function_end.get_tag_marker(), 
            cscope_db_tags::function_end.get_tag_id(), "INITIAL");
    //) (end of macro)
    rules_.add("TAB_START", cscope_db_tags::end_macro.get_tag_marker(), 
            cscope_db_tags::end_macro.get_tag_id(), "INITIAL");
    rules_.add("TAB_START", ".", sm_.skip(), "SYMBOL_TOKEN");

    rules_.add("TAB_START", "\\n", 1017, "INITIAL");


    rules_.add("SYMBOL_TOKEN", "[^\\n]+", cscope_token_ids::symbol, "INITIAL");


#if 0
    std::vector<std::string>    keyword_list;

    if (misc_utils::read_string_vector_from_file("keywords_c.txt", keyword_list)
            == false)
    {
        throw;
    }

    for (std::vector<std::string>::iterator it= keyword_list.begin(); 
            it != keyword_list.end(); it++) {
        if (!(*it).empty()) {
            rules_.add("LINE_NO_START", *it, cscope_token_ids::keyword, ".");
        }
    }
#endif

    rules_.add("LINE_NO_START", "\\n", 1010, "INITIAL");
    //rules_.add("LINE_NO_START", "\\s+", 1011,  ".");
    //rules_.add("LINE_NO_START", "\\S+", 1012, ".");
    rules_.add("LINE_NO_START", "[^\\w\\n\\.]", 1012, ".");
    rules_.add("LINE_NO_START", ".+", cscope_token_ids::symbol, ".");
    
    lexertl::generator::build (rules_, sm_);
}

cscope_db_rdr::cscope_db_rdr ()
{
}


cscope_db_rdr::~cscope_db_rdr()
{
}
        
void cscope_db_rdr::set_scan_action (int action)
{
    m_action_type = action;
}


void cscope_db_rdr::process_lines (sym_table& a_sym_table, const char* data, int size,
        generic_db_scanner& scanner)
{
    lexertl::basic_match_results<const char *> results(data, data+size);

    ctxt.last_token_id = cscope_tag_marker_ids::none;

    ////////debug(0) << "processing [" << input_ << "]" << std::endl;
    do
    {
        lexertl::lookup (scanner.get_state_machine(), results);
        ////////debug(0) << "Id: " << results.id << ", Token: '" << std::string (results.start, results.end) << "'\n";
        std::string& s = *new std::string(results.start, results.end);
        if (build_syms_from_token(a_sym_table, results.id, s) == false) {
            delete &s;
        }
    } while (results.id != 0 && results.id != results.npos ());

    ////////debug(0) << "finished job!" << std::endl;
    return;
}

sym_entry* cscope_db_rdr::create_sym_entry_or_lookup (sym_table& a_sym_table, 
                    const std::string& sym_text)
{
    sym_entry* a_sym = a_sym_table.lookup(sym_text);

    if (a_sym == NULL) {
        a_sym = new sym_entry(sym_text);
        a_sym_table.add_sym(a_sym);
        ////////debug(0) << "adding symbol " << sym_text << "\n";
    }
    return a_sym;
}

bool cscope_db_rdr::build_syms_from_token (sym_table& a_sym_table, long unsigned int token_type, std::string& token)
{
    sym_entry* untagged_sym;
    sym_entry* ref_func;
    sym_entry* ref_file;
    bool consumed = false;

    ////////debug(0) << "last token id is " << ctxt.last_token_id << "\n";
    if (ctxt.last_token_id != cscope_tag_marker_ids::none) {
        /* we have a symbol */
        if (ctxt.m_in_macro) {
            switch (ctxt.last_token_id) {
            case cscope_tag_marker_ids::function_call:
                ref_func = create_sym_entry_or_lookup(a_sym_table, token);
                if (ctxt.m_in_macro && ref_func && ctxt.m_line_num) {
                    a_sym_table.mark_xref(ctxt.m_in_macro, ref_func, ctxt.m_in_file, ctxt.m_line_num);
                }
                consumed = true;
                break;
            default:
                break;
            }
        } else if (ctxt.m_in_func) {
            switch (ctxt.last_token_id) {
            case cscope_tag_marker_ids::function_call:
                ref_func = create_sym_entry_or_lookup(a_sym_table, token);
                if (ctxt.m_in_func && ref_func && ctxt.m_line_num) {
                    a_sym_table.mark_xref(ctxt.m_in_func, ref_func, ctxt.m_in_file, ctxt.m_line_num);
                }
                consumed = true;
                break;
            case cscope_tag_marker_ids::function_end:
                ctxt.m_in_func = NULL;
                break;
            case cscope_tag_marker_ids::begin_macro:
                ctxt.m_in_macro = create_sym_entry_or_lookup(a_sym_table, token);
                consumed = true;
                break;
            default:
                break;
            }
        } else {
            switch (ctxt.last_token_id) {
            case cscope_tag_marker_ids::function_def: 
                ctxt.m_in_func = create_sym_entry_or_lookup(a_sym_table,token);
                consumed = true;
                break;
            case cscope_tag_marker_ids::begin_macro: 
                ctxt.m_in_macro = create_sym_entry_or_lookup(a_sym_table,token);
                consumed = true;
                break;
            case cscope_tag_marker_ids::include_file:
                ref_file = create_sym_entry_or_lookup(a_sym_table,token);
                if (ref_file && ctxt.m_in_file && ctxt.m_line_num) {
                    a_sym_table.mark_xref(ctxt.m_in_file, ref_file, ctxt.m_in_file, ctxt.m_line_num);
                }
                consumed = true;
                break;
            case cscope_tag_marker_ids::start_of_file:
                if (token[0] != '\0') {
                    ctxt.m_in_file = create_sym_entry_or_lookup(a_sym_table,token); 
                }
                break;
            case cscope_tag_marker_ids::global:
                create_sym_entry_or_lookup(a_sym_table,token);
                consumed = true;
                break;
            default:
                break;
            }
        }
        ctxt.last_token_id = cscope_tag_marker_ids::none;
    } else {
        switch (token_type) {
        case cscope_token_ids::symbol:
            {
                untagged_sym = a_sym_table.lookup(token);
                if (untagged_sym == NULL) {
                    return false;
                }
                if (ctxt.m_in_macro && ctxt.m_line_num) {
                    a_sym_table.mark_xref(ctxt.m_in_macro, untagged_sym, ctxt.m_in_file, ctxt.m_line_num);
                } else if (ctxt.m_in_func && ctxt.m_line_num) {
                    a_sym_table.mark_xref(ctxt.m_in_func, untagged_sym, ctxt.m_in_file, ctxt.m_line_num);
                }
            }
            break;
        case cscope_token_ids::line_number:
            {
                ctxt.m_line_num = misc_utils::atoi<int>(token);
                //////debug(0) << "processing line " << ctxt.m_line_num << "\n";
            }
            break;

        case cscope_tag_marker_ids::end_macro:
            ctxt.m_in_macro = NULL;
            break;
        case cscope_tag_marker_ids::function_end:
            ctxt.m_in_func = NULL;
            break;

        case cscope_tag_marker_ids::enum_type:
        case cscope_tag_marker_ids::function_def:
        case cscope_tag_marker_ids::function_call:
        case cscope_tag_marker_ids::begin_macro:
        case cscope_tag_marker_ids::include_file:
        case cscope_tag_marker_ids::start_of_file:
        case cscope_tag_marker_ids::member_def:
        case cscope_tag_marker_ids::global:
            {
                ctxt.last_token_id = token_type;
            }
            break;

        case cscope_token_ids::keyword:
            break;
        default:
            //std::cerr << "hitting a weird case " << token_type << "\n";
            break;
        }
    }
    return consumed;
}
