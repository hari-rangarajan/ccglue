#include "indexed_fstream.hpp"
#include <algorithm>
#include <string>
#include "tagfilerdr.h"

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
    atag->decode_from_stream(&b_sbuf);

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

#if 0
void tag_db::trace (tag_db_trace_query_t *query, 
                                        std::list<tag_db_trace_element> lst)
{
    
}
#endif
