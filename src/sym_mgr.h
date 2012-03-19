#ifndef SYMMGR_H
#define SYMMGR_H

#include <vector>
#include <list>
#include <string>
#include <sstream>
#include <tr1/unordered_map>
#include "typedefs.h"
#include "tag_file_writer.h"
#include <functional>

class sym_table;
class tag_file_writer;
class sym_entry;

struct elf_hash_32bit : std::unary_function< const char *, std::size_t >
{
    // should not throw any exceptions
    std::size_t operator() ( const char* key ) const /* can add noexcept or throw() here */
    {
        int i =0;

        std::size_t hash = 0U ;
        const std::size_t mask = 0xF0000000 ;

        while (key[i] != '\0') {
            hash = ( hash << 4U ) + key[i] ;
            std::size_t x = hash & mask ;
            if( x != 0 ) hash ^= ( x >> 24 ) ;
            hash &= ~x ;
            i++;
        }

        return hash;
    }
};

//typedef std::tr1::unordered_map<const char *, sym_entry*, elf_hash_32bit> hash_map_sym;
typedef std::tr1::unordered_map<std::string , sym_entry*> hash_map_sym;


class sym_table {
    protected:
	uint32                                               m_monotonic_id;
        hash_map_sym                                         m_hash_names;
        std::vector<sym_entry *>                             m_array_sym;
    public:
        RC_t init();
        sym_table();
        ~sym_table();
        void destroy();
        uint32 get_new_id();
        bool add_sym(sym_entry* a_sym_entry);
        sym_entry* lookup(const char *sym_name);
        void write_xref_tag_file(const char* fname);
        void write_syms_as_tags_to_file(tag_file_writer& file);
        void mark_xref(sym_entry* in_func, sym_entry *ref_func);
        void assign_unique_ids_to_symbols();
        void prepare_to_serialize();
};

class sym_entry {
    protected:
	uint32                  m_uid;
	std::string             m_n;
        std::list<sym_entry *>  m_p;
        std::list<sym_entry *>  m_c;
	
    public:
        sym_entry (const char *name);
        ~sym_entry ();
        uint32 get_uid() const {return m_uid;};
        const std::string& get_n() const {return m_n;};
        const std::list<sym_entry*>& get_p() const {return m_p;};
        const std::list<sym_entry*>& get_c() const {return m_c;};
        void mark_p(sym_entry *p);
        void mark_c(sym_entry *p);
        RC_t set(const char *sym_name, uint32 uid);
        friend bool sym_entry_cmp(sym_entry *sym1, 
                sym_entry *sym2);
        friend class sym_table;
};


#endif
