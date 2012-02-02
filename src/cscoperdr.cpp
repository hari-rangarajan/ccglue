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

#define MAX_TEMP_BUFFER 1024

/* ??? */
/*
seq_file_t **cscope_db_rdr_get_seq_file(cscope_db_rdr_t *rdr) {
    return &rdr->cscope_db;
}
*/

cscope_db_rdr::cscope_db_rdr()
{
    m_in_func = NULL;
    m_in_file = NULL;
    m_in_macro = NULL;
    m_in_enum = NULL;
    //m_cscope_db = NULL;

}

RC_t cscope_db_rdr::open (const char *cscope_db_name)
{
    //m_cscope_db = new seq_file;
    //return m_cscope_db->reader_create(cscope_db_name);
}

void cscope_db_rdr::destroy ()
{
    //m_cscope_db->close();
}

cscope_db_rdr::~cscope_db_rdr()
{
    destroy();
    //delete m_cscope_db;
}
        
void cscope_db_rdr::set_scan_action (int action)
{
    m_action_type = action;
}

void cscope_db_rdr::process_line (sym_table *a_sym_table, uchar* line)
{
    switch(m_action_type) {
    case ACTION_LOAD_SYMS:
        build_sym_from_line(a_sym_table, line);
        break;
    case ACTION_XREF_SYMS:
        build_xref_from_line(a_sym_table, line);
        break;
    default:
        break;
    }
}

void cscope_db_rdr::build_xref_from_line (sym_table *a_sym_table, uchar* line)
{
    char        temp[MAX_TEMP_BUFFER];
    sym_entry   *ref_func;
    sym_entry   *ref_file;
    sym_entry   *untagged_sym;

    if (line[0] !='\t') {
        snprintf(temp, MAX_TEMP_BUFFER, "%s", &line[0]);
        untagged_sym = a_sym_table->lookup(temp);
        if (untagged_sym == NULL) {
            return;
        }
        if (m_in_macro) {
            a_sym_table->mark_xref(m_in_macro, untagged_sym);
        } else if (m_in_func) {
            a_sym_table->mark_xref(m_in_func, untagged_sym);
        }
    } else if (line[0] == '\t') {
        snprintf(temp, MAX_TEMP_BUFFER, "%s", &line[2]);
        if (m_in_macro) {
            switch (line[1]) {
            case '`':
                ref_func = a_sym_table->add_ext(temp);
                if (m_in_macro && ref_func) {
                    a_sym_table->mark_xref(m_in_macro, ref_func);
                }
                break;
            case ')':
                m_in_macro = NULL;
            default:
                break;
            }
        } else if (m_in_func) {
            switch (line[1]) {
            case '`':
                ref_func = a_sym_table->add_ext(temp);
                if (m_in_func && ref_func) {
                    a_sym_table->mark_xref(m_in_func, ref_func);
                }
                break;
            case '}':
                m_in_func = NULL;
                break;
            case '#':
                m_in_macro = a_sym_table->lookup(temp);
                break;
            default:
                break;
            }
        } else {
            switch (line[1]) {
            case '$': 
                m_in_func = a_sym_table->lookup(temp);
                break;
            case '#': 
                m_in_macro = a_sym_table->lookup(temp);
                break;
            case '~':
                ref_file = a_sym_table->add_ext(&temp[1]);
                if (ref_file && m_in_file) {
                    a_sym_table->mark_xref(m_in_file, ref_file);
                }
                break;
            case '@':
                if (temp[0] != '\0') {
                    m_in_file = a_sym_table->lookup(temp);
                }
                break;
            default:
                break;
            }
        }
    }
}


void cscope_db_rdr::build_sym_from_line (sym_table *a_sym_table, uchar* line)
{
    char         temp[1024];
    sym_entry   *ref_func;
    sym_entry   *ref_file;

    if (line[0] =='\t') {
        snprintf(temp, MAX_TEMP_BUFFER, "%s", &line[2]);
        if (m_in_enum) {
            switch (line[1]) {
            case 'm':
                (void) a_sym_table->add_ext(temp);
                return;
            default:
                m_in_enum = NULL;
                break;
            } 
        }
        /* fall-through path or otherwise */
        switch (line[1]) {
        case '$': 
            (void) a_sym_table->add_ext(temp);
            break;
        case '#': 
            (void) a_sym_table->add_ext(temp);
            break;
        case '@':
            if (temp[0] != '\0') {
                (void) a_sym_table->add_ext(temp);
            }
            break;
        case 'e':
            m_in_enum = a_sym_table->add_ext(temp);
            break;
        case 'g':
            (void) a_sym_table->add_ext(temp);
            break;
#if 0
            /* typedefs are not that useful to track */
        case 't':
            (void) a_sym_table->add_ext(temp);
            break;
#endif
        default:
            break;
        }
    }
}


void cscope_db_rdr::build_xref (sym_table *a_sym_table, uchar* line)
{
    while (m_cscope_db->read_till_newline(line,
                sizeof(line)) != RC_FAILURE) {
        if (line[0] =='\t') {
            process_line(a_sym_table, line);
        }
    }
}


