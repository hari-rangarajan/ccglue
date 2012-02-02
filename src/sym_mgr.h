#ifndef SYMMGR_H
#define SYMMGR_H

#include <glib.h>
#include "typedefs.h"
#include "tag_file_writer.h"


class sym_entry {
    protected:
	uint32	 m_uid;	
	GString *m_uid_str;
	GString *m_n;
	GString *m_p;
	GString *m_c;
    public:
        sym_entry ();
        ~sym_entry ();
        RC_t init ();
        RC_t destroy();
        uint32 get_uid() const {return m_uid;};
        GString* get_uid_str() const {return m_uid_str;};
        GString* get_n() const {return m_n;};
        GString* get_p() const {return m_p;};
        GString* get_c() const {return m_c;};
        void mark_p(sym_entry *p);
        void mark_c(sym_entry *p);
        RC_t set(const char *sym_name, uint32 uid);
};

class tag_file_writer;

class sym_table {
    protected:
	uint32      m_monotonic_id;
	GHashTable* m_hash_names;
	GArray*     m_array_sym;
    public:
        RC_t init();
        sym_table();
        ~sym_table();
        void destroy();
        uint32 get_new_id();
        sym_entry* add_sym(const char *sym_name);
        sym_entry* lookup(const char *sym_name);
        sym_entry* add_ext(const char *sym_name);
        void write_xref_tag_file(const char* fname);
        static void sym_name_data_free_iterator (gpointer key,
        gpointer value, gpointer user_data);
        void write_syms_as_tags_to_file(tag_file_writer *file);
        void mark_xref(sym_entry* in_func, 
                sym_entry *ref_func);
};


#endif
