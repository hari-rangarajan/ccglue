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


cscope_db_rdr::cscope_db_rdr ():
    m_in_func(NULL),
    m_in_file(NULL),
    m_in_macro(NULL),
    m_in_enum(NULL)
{
}


cscope_db_rdr::~cscope_db_rdr()
{
}
        
void cscope_db_rdr::set_scan_action (int action)
{
    m_action_type = action;
}

void cscope_db_rdr::process_line (sym_table& a_sym_table, char* line)
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

sym_entry* cscope_db_rdr::create_sym_entry_or_lookup (sym_table& a_sym_table, 
                    const char* sym_text)
{
    sym_entry* a_sym = a_sym_table.lookup(sym_text);

    if (a_sym == NULL) {
        a_sym = new sym_entry(sym_text);
        a_sym_table.add_sym(a_sym);
    }
    return a_sym;
}

void cscope_db_rdr::build_xref_from_line (sym_table& a_sym_table, char* line)
{
    char     *  temp;
    sym_entry* ref_func;
    sym_entry* ref_file;
    sym_entry* untagged_sym;

    if (line[0] !='\t') {
        temp = &line[0];
        untagged_sym = a_sym_table.lookup(temp);
        if (untagged_sym == NULL) {
            return;
        }
        if (m_in_macro) {
            a_sym_table.mark_xref(m_in_macro, untagged_sym);
        } else if (m_in_func) {
            a_sym_table.mark_xref(m_in_func, untagged_sym);
        }
    } else if (line[0] == '\t') {
        temp = &line[2];
        if (m_in_macro) {
            switch (line[1]) {
            case '`':
                ref_func = create_sym_entry_or_lookup(a_sym_table, temp);
                if (m_in_macro && ref_func) {
                    a_sym_table.mark_xref(m_in_macro, ref_func);
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
                ref_func = create_sym_entry_or_lookup(a_sym_table, temp);
                if (m_in_func && ref_func) {
                    a_sym_table.mark_xref(m_in_func, ref_func);
                }
                break;
            case '}':
                m_in_func = NULL;
                break;
            case '#':
                m_in_macro = a_sym_table.lookup(temp);
                break;
            default:
                break;
            }
        } else {
            switch (line[1]) {
            case '$': 
                m_in_func = a_sym_table.lookup(temp);
                break;
            case '#': 
                m_in_macro = a_sym_table.lookup(temp);
                break;
            case '~':
                ref_file = create_sym_entry_or_lookup(a_sym_table,&temp[1]);
                if (ref_file && m_in_file) {
                    a_sym_table.mark_xref(m_in_file, ref_file);
                }
                break;
            case '@':
                if (temp[0] != '\0') {
                    m_in_file = a_sym_table.lookup(temp);
                }
                break;
            default:
                break;
            }
        }
    }
}


void cscope_db_rdr::build_sym_from_line (sym_table& a_sym_table, char* line)
{
    char        *temp;
    sym_entry   *ref_func;
    sym_entry   *ref_file;
    sym_entry   *ref;

    if (line[0] =='\t') {
        temp =  &line[2];
        if (m_in_enum) {
            switch (line[1]) {
            case 'm':
                (void) create_sym_entry_or_lookup(a_sym_table, temp);
                return;
            default:
                m_in_enum = NULL;
                break;
            } 
        }
        /* fall-through path or otherwise */
        switch (line[1]) {
        case '$': 
            ref = create_sym_entry_or_lookup(a_sym_table, temp);
            ref->mark_f(m_in_file);
            break;
        case '#': 
            ref = create_sym_entry_or_lookup(a_sym_table, temp);
            ref->mark_f(m_in_file);
            break;
        case '@':
            if (temp[0] != '\0') {
                m_in_file = create_sym_entry_or_lookup(a_sym_table, temp);
            }
            break;
        case 'e':
            m_in_enum = create_sym_entry_or_lookup(a_sym_table, temp);
            m_in_enum->mark_f(m_in_file);
            break;
        case 'g':
            ref = create_sym_entry_or_lookup(a_sym_table, temp);
            ref->mark_f(m_in_file);
            break;
#if 0
            /* typedefs are not that useful to track */
        case 't':
            (void) create_sym_entry_or_lookup(a_sym_table, temp);
            break;
#endif
        default:
            break;
        }
    }
}


void cscope_db_rdr::build_xref (sym_table& a_sym_table, char* line)
{
#if 0
    while (m_cscope_db.getline(line,
                sizeof(line))) {
        if (line[0] =='\t') {
            process_line(a_sym_table, line);
        }
    }
#endif
}


