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
#include <string.h>
#include "seqfile.h"
#include <glib/gstring.h>

sym_table::sym_table ()
{
    m_monotonic_id = 0;
    m_hash_names = NULL;
    m_array_sym = NULL;
}

RC_t sym_table::init ()
{
    if ((m_hash_names = g_hash_table_new(g_str_hash, g_str_equal)) 
            == NULL) {
        return RC_FAILURE;
    }
    if ((m_array_sym = g_array_new(FALSE, FALSE,
                    sizeof(sym_entry *))) == NULL) {
        /* need to free */
        return RC_FAILURE;
    }
    return RC_SUCCESS;
}

void sym_table::destroy()
{
    g_hash_table_foreach(m_hash_names, 
            sym_name_data_free_iterator, NULL);
}

sym_table::~sym_table()
{
    destroy();
    g_array_free(m_array_sym, TRUE);
    g_hash_table_destroy(m_hash_names);
}

void sym_table::sym_name_data_free_iterator (gpointer key,
        gpointer value, gpointer user_data)
{
    free((uchar *) key);
    //((sym_entry *) value)->destroy();
    delete (sym_entry *) value;
}


uint32 sym_table::get_new_id ()
{
    return m_monotonic_id++;
}



gint sym_table_array_entry_cmp (gconstpointer a, gconstpointer b)
{
    sym_entry	*sym1 = (sym_entry *) (*(uint32 *)a);
    sym_entry	*sym2 = (sym_entry *) (*(uint32 *)b);

    return strcmp(sym1->get_uid_str()->str, sym2->get_uid_str()->str);
}

sym_entry* sym_table::add_sym (const char *sym_name)
{
    sym_entry *a_sym_entry = new sym_entry;
    a_sym_entry->init();
    a_sym_entry->set(sym_name, get_new_id());

    debug::log("Adding %s on %ld\n", a_sym_entry->get_n()->str, 
            a_sym_entry->get_uid());

    g_hash_table_insert(m_hash_names, (gpointer)g_strdup(sym_name),
            (gpointer)a_sym_entry);

    /* g_list_insert_sorted(table->list_sym, (gpointer)sym_entry, 
       sym_table_list_insert_cmp);
       */
    g_array_append_val(m_array_sym, a_sym_entry);
    return a_sym_entry;
}

sym_entry* sym_table::add_ext (const char *sym_name)
{
    sym_entry *a_sym_entry = lookup(sym_name);
    if (a_sym_entry == NULL) {
        //DEBUG_PRINTF(DBG_VERBOSE, "Could not find %s \n", sym_name);
        a_sym_entry = add_sym(sym_name);
    } 
    return a_sym_entry;
}

sym_entry* sym_table::lookup (const char *sym_name)
{
	sym_entry *a_sym_entry;

	a_sym_entry = (sym_entry *) g_hash_table_lookup(m_hash_names, 
			(gpointer) sym_name);
	return a_sym_entry;
}


void sym_table::write_xref_tag_file (const char* fname)
{
    tag_file_writer *writer = new tag_file_writer(fname);

    writer->write_xref_tag_header();
    write_syms_as_tags_to_file(writer);
    delete writer;
}

void sym_table::write_syms_as_tags_to_file (tag_file_writer *file)
{
    int i;

    g_array_sort(m_array_sym, sym_table_array_entry_cmp);
    for (i = 0; i < m_array_sym->len; i++) {
        file->write_sym_as_tag( 
                g_array_index(m_array_sym, sym_entry*, i));
    }
}

// Sym entry routines

sym_entry::sym_entry ()
{
    /* init strings */
   m_n = (NULL);
   m_p = (NULL);
   m_c = (NULL);
   m_uid_str = (NULL);
}

sym_entry::~sym_entry ()
{
    destroy();
}

RC_t sym_entry::init ()
{
    /* allocate strings */
    m_n = g_string_new(NULL);
    m_p = g_string_new(NULL);
    m_c = g_string_new(NULL);
    m_uid_str = g_string_new(NULL);

    return RC_SUCCESS;
}

RC_t sym_entry::destroy ()
{
    g_string_free(m_n, TRUE);
    g_string_free(m_p, TRUE);
    g_string_free(m_c, TRUE);
    g_string_free(m_uid_str, TRUE);
}

RC_t sym_entry::set (const char *sym_name, uint32 uid)
{
    g_string_printf(m_n, "%s", sym_name);
    m_uid =  uid;
    g_string_printf(m_uid_str, "%ld", m_uid);
}

void sym_entry::mark_p (sym_entry *p) 
{
	g_string_append_printf(m_p, "%ld,", p->get_uid());
}

void sym_entry::mark_c (sym_entry *c) 
{
	g_string_append_printf(m_c, "%ld,", c->get_uid());
}
							
void sym_table::mark_xref (sym_entry *in_func, sym_entry *ref_func) 
{
	in_func->mark_c(ref_func);
	ref_func->mark_p(in_func);
}
	

