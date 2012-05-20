#ifndef TAGDB_H
#define TAGDB_H

#include "indexed_fstream.hpp"
#include <algorithm>
#include <string>
#include "tagfilerdr.h"
#include "misc_util.h"

#include "lexertl/rules.hpp"
#include "lexertl/state_machine.hpp"
#include "lexertl/generator.hpp"
#include "lexertl/file_shared_iterator.hpp"

struct tag_file_marker_ids {
    enum {
            none = 0,
            id = 1,
            name,
            child_marker,
            parent_marker,
            child_loc_marker,
            parent_loc_marker,
            comma_separator,
            pipe_separator,
            hash_separator,
            skip_tab,
            end_of_tag,
            child_data_marker,
            parent_data_marker,
            field_data,
    };
};

/* make the scanner class generic */
class tag_file_scanner {
    public:
        lexertl::state_machine& get_state_machine() {
            return sm_;
        }
    protected:
        lexertl::rules  rules_;
        lexertl::state_machine sm_;
};

class ccglue_tag_file_scanner : public tag_file_scanner {
    public:
        ccglue_tag_file_scanner();
};

ccglue_tag_file_scanner::ccglue_tag_file_scanner()
{
    rules_.add_state("ID_NAME_SEPARATOR");
    rules_.add_state("HASH_SEPARATOR");
    rules_.add_state("TAB_SKIP");
    rules_.add_state("FIELD_MARKER");
    rules_.add_state("FIELD_DATA");
    rules_.add_state("FIELD_DATA_END");

    rules_.add("INITIAL", "\\d+", tag_file_marker_ids::id, "HASH_SEPARATOR");
    rules_.add("HASH_SEPARATOR", "#", sm_.skip(), "ID_NAME_SEPARATOR");
    rules_.add("ID_NAME_SEPARATOR", "\\S+", tag_file_marker_ids::name, "TAB_SKIP");
    //rules_.add("TAB_SKIP", "\\t\\t[^\\t]*\\t", tag_file_marker_ids::skip_tab, "FIELD_MARKER");
    rules_.add("TAB_SKIP", "\\t\\t[^\\t]*\\t", sm_.skip(), "FIELD_MARKER");
    
    rules_.add("FIELD_MARKER", "c:", tag_file_marker_ids::child_marker, "FIELD_DATA");
    rules_.add("FIELD_MARKER", "p:", tag_file_marker_ids::parent_marker, "FIELD_DATA");
    rules_.add("FIELD_MARKER", "cl:", tag_file_marker_ids::child_loc_marker, "FIELD_DATA");
    rules_.add("FIELD_MARKER", "pl:", tag_file_marker_ids::parent_loc_marker, "FIELD_DATA");
    rules_.add("FIELD_DATA", "[^\\t\\n]+", tag_file_marker_ids::field_data, "FIELD_DATA_END");
    rules_.add("FIELD_DATA", "\\t", tag_file_marker_ids::skip_tab, "FIELD_MARKER");
    rules_.add("FIELD_DATA", "\\n", tag_file_marker_ids::end_of_tag, "INITIAL");
    rules_.add("FIELD_DATA_END", "\\t", tag_file_marker_ids::skip_tab, "FIELD_MARKER");
    rules_.add("FIELD_DATA_END", "\\n", tag_file_marker_ids::end_of_tag, "INITIAL");
    
    lexertl::generator::build (rules_, sm_);
}

struct csv_ids {
    enum {
        numeric_data = 1,
        comma_separator,
    };
};

class scanner_csv : public tag_file_scanner {
    public:
        scanner_csv();
};

scanner_csv::scanner_csv()
{
    rules_.add_state("DATA");

    rules_.add("INITIAL", "\\d+", csv_ids::numeric_data, "DATA");
    rules_.add("DATA", ",", sm_.skip(), "INITIAL");
    
    lexertl::generator::build (rules_, sm_);
}

lexertl::state_machine& get_scanner_csv() {
    static scanner_csv  a_csv_scanner;
    return a_csv_scanner.get_state_machine();
}

typedef unsigned int    tag_id_type_t;
typedef unsigned char   tag_type_t;

class tag {
    public:
        tag () {};
        tag (std::streambuf* is_it);
        void decode_from_stream (std::streambuf* s_buf, tag_file_scanner& scanner);
        void dump (std::ostream& os);

        const std::string&                get_symbol_name() const;
        const std::list<tag_id_type_t>&   get_list_by_direction(int dir) const ;
        const std::list<tag_id_type_t>&   get_child_list() const ;
        const std::list<tag_id_type_t>&   get_parent_list() const ;
    protected:
        tag_type_t                      m_type;
        tag_id_type_t                   m_id;
        std::string                     m_name;
        std::list<tag_id_type_t>        m_c;
        std::list<tag_id_type_t>        m_p;

    private:
        int         last_token_id;

        void decode_compressed_list (std::string& buf,
                                std::list<tag_id_type_t>& lst,
                                lexertl::state_machine& sm);
        bool process_decoded_token (int token_id, std::string& token);
};

void tag::dump(std::ostream& os)
{
    os << m_name << std::endl;
    os << "parent links: ";
    copy(m_p.begin(), m_p.end(), std::ostream_iterator<tag_id_type_t> (os, ","));
    os << "\nchild links: ";
    copy(m_c.begin(), m_c.end(), std::ostream_iterator<tag_id_type_t> (os, ","));
    os << std::endl;
}

void tag::decode_compressed_list (std::string& buf,
                                std::list<tag_id_type_t>& lst, 
                                lexertl::state_machine& sm)
{
    std::stringstream               oss;
    std::stringstream               s_buf(buf);
    digraph_uncompress_buf          uncompress_buf((*s_buf.rdbuf()), 
                                        digraph_maps::get_numeric_uncompress_map());
    
    std::istream                    ifs(&uncompress_buf);
    lexertl::file_shared_iterator   iter (ifs);
    lexertl::file_shared_iterator   end;
    lexertl::basic_push_match_results<lexertl::file_shared_iterator, std::size_t> 
                                                                results(iter, end);

    do
    {
        lexertl::lookup (sm, results);
        std::string s(results.start, results.end);
        std::cout << "CSV Id: " << results.id << ", Token: '" << s << "'\n";
        switch (results.id) {
        case csv_ids::numeric_data:
#if 0
            int id = misc_utils::atoi<int>(s);
            if (id == 32767) {
                std::cout << "trouble brewing";
            }
#endif
            lst.push_back(misc_utils::atoi<int>(s));
            break;
        default:
            break;
        }
    } while (results.id != 0 && results.id != results.npos ());

    return;
#if 0
    //std::cout << "STart "  << std::endl;
    while (is_it != eos) {
        c = *is_it;
        //std::cout << " : " << c << std::endl;
        if (c == '\t' || c == '\n') {
            /* end of list */
            return;
        } else if (c == ',') {
            /* separator */
            tag_id_type_t   id;
            oss >> id;
            lst.push_back(id);
            oss.clear();
            oss.str("");
        } else {
            oss.put(c);
        }
        is_it++;
    }
    //std::cout << "Done " << c << std::endl;
#endif
}

tag::tag (std::streambuf* s_buf)
{
    ccglue_tag_file_scanner scanner;

    decode_from_stream(s_buf, scanner);
}

void tag::decode_from_stream (std::streambuf* s_buf, tag_file_scanner& scanner)
{
#if 0
    // bug here
    std::string input_((std::istreambuf_iterator<char>(s_buf)), std::istreambuf_iterator<char>());
    
    std::string::const_iterator iter_ = input_.begin ();
    std::string::const_iterator end_ = input_.end ();
    
    lexertl::push_match_results results(iter_, end_);
#endif
#if 1
    std::istream            ifs(s_buf);
    lexertl::file_shared_iterator iter (ifs);
    lexertl::file_shared_iterator end;
    lexertl::basic_push_match_results<lexertl::file_shared_iterator,
std::size_t>
      results(iter, end);
#endif

    do
    {
        lexertl::lookup (scanner.get_state_machine(), results);
        std::string s(results.start, results.end);
        std::cout << "Id: " << results.id << ", Token: '" << s << "'\n";
        process_decoded_token(results.id, s);
    } while (results.id != 0 && results.id != results.npos ());
}

bool tag::process_decoded_token (int token_id, std::string& token)
{
    switch (last_token_id) {
    case tag_file_marker_ids::end_of_tag:
    case tag_file_marker_ids::none:
        switch (token_id) {
        case    tag_file_marker_ids::id:
            m_id = misc_utils::atoi<int>(token);
            break;
        default:
            return false;
        }
        break;
    case tag_file_marker_ids::id:
        switch (token_id) {
        case tag_file_marker_ids::name:
            m_name = token;
            break;
        default:
            return false;
        }
        break;
    case tag_file_marker_ids::child_marker:
        switch (token_id) {
        case tag_file_marker_ids::field_data:
            decode_compressed_list(token, m_c, get_scanner_csv());
            break;
        default:
            return false;
        }
        break;
    case tag_file_marker_ids::parent_marker:
        switch (token_id) {
        case tag_file_marker_ids::field_data:
            decode_compressed_list(token, m_p, get_scanner_csv());
            break;
        default:
            return false;
        }
        break;
    case tag_file_marker_ids::parent_loc_marker:
        switch (token_id) {
        case tag_file_marker_ids::field_data:
            break;
        default:
            return false;
        }
        break;
    case tag_file_marker_ids::child_loc_marker:
        switch (token_id) {
        case tag_file_marker_ids::field_data:
            break;
        default:
            return false;
        }
        break;
    }
    last_token_id = token_id;
}

const std::list<tag_id_type_t>&   tag::get_parent_list() const
{
    return m_p;
}

const std::list<tag_id_type_t>&   tag::get_child_list() const
{
    return m_c;
}
        
const std::list<tag_id_type_t>&   tag::get_list_by_direction(int dir) const
{
    if (dir == ccglue::trace_direction::forward) {
        return m_c;
    } else {
        return m_p;
    }
}
        
const std::string&  tag::get_symbol_name() const
{
    return m_name;
}


class tag_db {
    public:
        tag_db(const std::string& filename, const std::string& idx_fname):
            ifs_db(idx_fname),
            m_data_file(filename.c_str(), std::ios::in | std::ios::binary) 
            {};
        tag* get_tag_line (std::string tag_name);
        tag* get_tag_from_stream (std::streambuf * buf);
        tag* get_tag_by_id (int id);
        void dmp_all();
    protected:
        std::ifstream                   m_data_file;
        indexed_ifstream_vector<int>    ifs_db;
};

void tag_db::dmp_all ()
{
    indexed_ifstream_vector<int>::iterator iter(&ifs_db);

    for (iter= ifs_db.begin(); iter != ifs_db.end(); iter++) {
        std::cout << *iter << " <<\n";
    }
}


tag* tag_db::get_tag_from_stream (std::streambuf * buf)
{
    index_record_t  rec;
    std::istream    is(buf);
    is.read(reinterpret_cast<char *> (&rec), sizeof(index_record_t));
    m_data_file.seekg(rec.record_start);

    bounded_streambuf   b_sbuf(m_data_file.rdbuf(), rec.size);

    tag *atag = new tag;
    /* create a constructor here */
    ccglue_tag_file_scanner scanner;
    atag->decode_from_stream(&b_sbuf, scanner);

    return atag;
}

tag* tag_db::get_tag_by_id (int id) 
{
    index_record_t  rec;
    std::streambuf*  buf = ifs_db[id];
    return get_tag_from_stream(buf);
}

bool my_dummy_func (std::streambuf* s_buf, const std::string& s)
{
    std::string::const_iterator           s_it = s.begin();
    std::istreambuf_iterator<char>   sbuf_it(s_buf);
    std::istreambuf_iterator<char>   sbuf_end;
    
    s_buf->pubseekoff(sizeof(index_record_t), std::ios::cur);
    
#if 0
    std::cout << "starting\n";
    while (sbuf_it != sbuf_end) {
        std::cout << *sbuf_it;
        sbuf_it++;
    }

    std::cout << "\n";
    
    return false;
#endif

    do {
        char c1 = (char) *sbuf_it;
        char c2 = (char) *s_it;

        if (s_it == s.end()) {
            return false;
        };
        if (sbuf_it == sbuf_end) {
            return true;
        };
        if (c1 < c2) {
            return true;
        };
        if (c1 > c2) {
            return false;
        };
        s_it++;
        sbuf_it++;
    } while (1==1);
    return false; 
}

tag* tag_db::get_tag_line (std::string tag_name)
{
    indexed_ifstream_vector<int>::iterator it = lower_bound(ifs_db.begin(), ifs_db.end(), tag_name, 
            my_dummy_func);
    //std::cout << "Found "<< it - ifs_db.begin();
    std::streambuf*  buf = *it;
    return get_tag_from_stream(buf);
}

typedef struct tag_db_trace_query {
    std::string     sym_pattern;

} tag_db_trace_query_t;

typedef struct tag_db_trace_element {


} tag_db_trace_element_t;

#endif
