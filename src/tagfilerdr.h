#ifndef TAGFILERDR_H
#define TAGFILERDR_H

#include <list>
#include <istream>
#include <sstream>
#include <iterator>
#include "../digraph.h"
#include <string>

#define MAX_SIZE_LINE_NUMBER   256
#define MAX_SIZE_TAG_NAME   256
#define MAX_SIZE_TAG_FILE_NAME 512
#define MAX_SIZE_TAG_LOCATION 1
#define MAX_SIZE_TAG_LINE 4096

typedef unsigned int    tag_id_type_t;
typedef unsigned char   tag_type_t;

class tag {
    public:
        tag (std::streambuf* is_it);
        void dump (std::ostream& os);
    protected:
        tag_type_t                      m_type;
        tag_id_type_t                   m_id;
        std::string                     m_name;
        std::list<tag_id_type_t>        m_c;
        std::list<tag_id_type_t>        m_p;

    private:
        void decode_compressed_list (std::streambuf* s_buf,
                                std::list<tag_id_type_t>& lst);
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

void tag::decode_compressed_list (std::streambuf* s_buf,
                                std::list<tag_id_type_t>& lst)
{
    std::stringstream              oss;
    digraph_uncompress_buf          uncompress_buf((*s_buf), 
                                        digraph_maps::get_numeric_uncompress_map());
    char                           c;
    std::istreambuf_iterator<char>    is_it(&uncompress_buf);
    std::istreambuf_iterator<char>    eos;

    //std::cout << "STart "  << std::endl;
    while (is_it != eos) {
        c = *is_it;
        std::cout << " : " << c << std::endl;
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
    std::cout << "Done " << c << std::endl;
}

tag::tag (std::streambuf* s_buf)
{
    std::stringstream   oss;
    std::istreambuf_iterator<char> is_it(s_buf);
    std::istreambuf_iterator<char> eos;

    while (is_it != eos) {
        if ((*is_it) == '#') {
            /* we have an ID */
            oss >> m_id;
            is_it++;
            break;
        } 
        oss << *is_it;

        is_it++;
    }       
    oss.str(std::string());
    oss.clear();

    while (is_it != eos) {
        if (*is_it == '\t') {
            /* we have a name */
            m_name = oss.str();
            break;
        } 
        oss << *is_it;
        is_it++;
    };
    
    oss.str(std::string());
    oss.clear();

    /* 3 tab skips */
    int count = 3;
    while (count > 0) {
        do { 
            if (*is_it == '\t') {
                count--;
                *is_it++;
                break;
            }
        } while ((is_it++) != eos);
    };
    /* now comes c: */
    std::cout << " skipping " << (int)*is_it;
    is_it++;
    std::cout << " skipping " << (int)*is_it << std::endl;
    is_it++;
    decode_compressed_list(s_buf, m_c);
    /* now comes p: */
    is_it++;
    is_it++;
    decode_compressed_list(s_buf, m_p);
}

#endif
