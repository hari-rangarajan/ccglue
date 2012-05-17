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

#include <stdio.h>
#include "sym_mgr.h"
#include "digraph.h"
#include "debug.h"
#include <string>
#include <vector>
#include <algorithm>
#include "indexed_fstream.hpp"

sym_table::sym_table ():
    m_monotonic_id(0)
{
}

RC_t sym_table::init ()
{
    return RC_SUCCESS;
}

void sym_table::destroy()
{
    for (std::vector<sym_entry *>::const_iterator it = m_array_sym.begin(); it != m_array_sym.end(); it++) {
        delete *it;
    }
    m_array_sym.clear();
}

sym_table::~sym_table()
{
    destroy();
}

uint32 sym_table::get_new_id () 
{
    return m_monotonic_id++;
}



bool sym_table::add_sym (sym_entry* a_sym_entry)
{
    //debug::log("Adding %s on ???\n", a_sym_entry->get_n().c_str());

    //m_hash_names.insert(std::tr1::unordered_map<const char *,sym_entry *>::
      //      value_type(a_sym_entry->get_n().c_str(), a_sym_entry));
    m_hash_names[a_sym_entry->get_n()] = a_sym_entry;
    m_array_sym.push_back(a_sym_entry);

    return true;
}

sym_entry* sym_table::lookup (const std::string& sym_name)
{
    lookup(sym_name.c_str());
}

sym_entry* sym_table::lookup (const char *sym_name) 
{
    hash_map_sym::const_iterator sym_entries_iterator;

    sym_entries_iterator = m_hash_names.find(sym_name);

    if (sym_entries_iterator == m_hash_names.end()) {
        return NULL;
    } 
    return (sym_entries_iterator->second);
}


void sym_table::write_xref_tag_file (const std::string& fname,
        const std::string& index_fname)
{
    tag_file_writer writer(fname);
    
    writer.write_xref_tag_header();
    
    if (!index_fname.empty()) {
        indexed_ofstream idx_ofs(index_fname);
        write_syms_as_tags_to_file_with_idx(writer, idx_ofs);
    } else {
        write_syms_as_tags_to_file(writer);
    }
}


bool sym_entry_cmp (sym_entry *sym1, sym_entry *sym2)
{
    return (sym1->m_n < sym2->m_n);
}

void sym_table::uncompress_symbol_names ()
{
    std::vector<sym_entry *>::iterator   iter;
    std::stringstream        tmp_sstream;
    digraph_uncompress_buf  uncompress_buf(*(tmp_sstream.rdbuf()),
                                             digraph_maps::get_letter_uncompress_map());
    std::istream            uncompress_stream(&uncompress_buf);
    
    for (iter = m_array_sym.begin(); iter != m_array_sym.end(); iter++) {
        tmp_sstream.str((*iter)->get_n());
        uncompress_stream >> (*iter)->m_n;
    }
}


void sym_table::assign_unique_ids_to_symbols ()
{
    std::vector<sym_entry *>::iterator   iter;

    for (iter = m_array_sym.begin(); iter != m_array_sym.end(); iter++) {
        (*iter)->m_uid = get_new_id();
    }
}
 
void sym_table::prepare_to_serialize ()
{
    uncompress_symbol_names();
    sort(m_array_sym.begin(), m_array_sym.end(), sym_entry_cmp);
    assign_unique_ids_to_symbols();
}

void sym_table::write_syms_as_tags_to_file (tag_file_writer& file)
{
    std::vector<sym_entry *>::iterator   iter;
    
    prepare_to_serialize();
    
    /* TODO: change to a STL idiom */
    for (iter = m_array_sym.begin(); iter != m_array_sym.end(); iter++) {
        file.write_sym_as_tag(*iter); 
    }
}

void sym_table::write_syms_as_tags_to_file_with_idx (tag_file_writer& file, 
        indexed_ofstream& idx_file)
{
    index_record_t      rec;
    std::streampos      fpos;
    std::streampos      idx_fpos_start, idx_fpos_end;
    std::vector<sym_entry *>::iterator   iter;
    std::ostream    fbs(idx_file.rdbuf());
    
    prepare_to_serialize();
    
    /* TODO: change to a STL idiom */
    for (iter = m_array_sym.begin(); iter != m_array_sym.end(); iter++) {
       
        rec.record_start = file.get_file_pos();
        file.write_sym_as_tag(*iter); 
        rec.size = file.get_file_pos() - rec.record_start;
        
        idx_file.begin_record();
        idx_fpos_start = fbs.tellp();
        fbs.seekp(sizeof(index_record_t), std::ios::cur);
        fbs << (*iter)->get_n();
        idx_fpos_end = fbs.tellp();
        fbs.seekp(idx_fpos_start, std::ios::beg);
        fbs.write(reinterpret_cast<char *>(&rec), sizeof(index_record_t));
        fbs.seekp(idx_fpos_end, std::ios::beg);
#if 0
        std::cout << "writing " << (*iter)->get_n() << "size " <<
            idx_fpos_end - idx_fpos_start << 
            " start " << idx_fpos_start <<
            " end " << idx_fpos_end << std::endl;
#endif
        idx_file.end_record();
    }
    idx_file.write_index_to_file();
    idx_file.close();
}

// Sym entry routines

sym_entry::sym_entry (const std::string& name):
    m_n(name), m_uid(0)
{

}

sym_entry::sym_entry (const char *name):
    m_n(name), m_uid(0)
{
    /* default inits */
}

sym_entry::~sym_entry ()
{
}


void sym_entry::mark_p (sym_entry *p) 
{
    m_p.push_back(p);
}

void sym_entry::mark_c (sym_entry *c) 
{
    m_c.push_back(c);
}

							
void sym_table::mark_xref (sym_entry *in_func, sym_entry *ref_func) 
{
    in_func->mark_c(ref_func);
    ref_func->mark_p(in_func);
}
	
sym_entry_loc::sym_entry_loc (sym_entry* sentry, sym_loc_line_number_t line_num):
    m_entry(sentry), m_line_num(line_num)
{
}

sym_entry*  sym_entry_loc::get_sym_entry()
{
    return m_entry;
}

sym_loc_line_number_t sym_entry_loc::get_line_num()
{
    return m_line_num;
}


