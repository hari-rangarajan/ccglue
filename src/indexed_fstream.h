#ifndef INDEXED_FSTREAM_H
#define INDEXED_FSTREAM_H

#include <list>
#include <fstream>
#include <iterator>
#include <vector>
#include <iostream>
#include <memory>
#include <stdexcept>
        
typedef struct {
    std::streampos             record_start;
    int                        size;
} index_record_t;

class indexed_ofstream {
    public:
        indexed_ofstream(const std::string& filename);
        ~indexed_ofstream();
        void end_record(); 
        void begin_record();
        void write_index_to_file();
        void write_records_binary(std::ostream&  sout);
        void dump(std::ostream&  sout);
        std::streambuf* rdbuf();
        void close();
    protected:
        std::ofstream                m_ofs;
        index_record_t               m_index_record;
        std::list<index_record_t>    m_record_pos_list;
};


class bounded_streambuf:public std::streambuf {
    protected:
        int              m_bound;
        
    public:
        std::streambuf  *m_buf;
        bounded_streambuf(std::streambuf *buf, int bound=0);
        int     setbound (int numc);
        traits_type::int_type sbumpc();
        traits_type::int_type sgetc();

        traits_type::int_type uflow();
        traits_type::int_type underflow();
        std::streampos seekoff(std::streamoff off, 
                std::ios::seekdir way,  std::ios::openmode which);
};


template <class T>
class indexed_ifstream_vector {
    public:
        explicit indexed_ifstream_vector(const std::string& index_file);
        class iterator:
            public std::iterator<std::random_access_iterator_tag, std::string > {
                protected:
                    indexed_ifstream_vector<T>                     *m_idx_ifs_vec;
                    int                                             m_current_rec_num;
                public:
                    typedef typename std::iterator<std::input_iterator_tag, indexed_ifstream_vector<T> >::difference_type difference_type;
                    friend class indexed_ifstream_vector;
                    iterator():m_idx_ifs_vec(NULL), m_current_rec_num(0) {};
                    iterator (indexed_ifstream_vector<T>* v,
                            int index = 0):
                        m_idx_ifs_vec(v), m_current_rec_num(index) {};

                    iterator& operator++(int) {
                        m_current_rec_num++;
                        return (*this);
                    }

                    iterator& operator++() {
                        m_current_rec_num++;
                        return (*this);
                    }

                    iterator& operator=(const iterator& rhs) {
                        m_idx_ifs_vec = rhs.m_idx_ifs_vec;
                        m_current_rec_num = rhs.m_current_rec_num;
                        return (*this);
                    }

                    bool operator<(iterator& rhs) {
                        return
                            (m_current_rec_num < rhs.m_current_rec_num);
                    }

                    bool operator == (const iterator& rhs) {
                        return (this->m_current_rec_num ==
                                 rhs.m_current_rec_num);
                    }

                    bool operator != (const iterator& rhs) {
                        return !(this->operator==(rhs));
                    }
                    
                    std::streambuf* operator* () {
                        return m_idx_ifs_vec->at(m_current_rec_num);
                    }
                    
                    difference_type operator-(const iterator& iter) const {
                        return m_current_rec_num - iter.m_current_rec_num;
                    }
                    
                    iterator operator-(const difference_type& n) const {
                        return iterator(m_idx_ifs_vec, m_current_rec_num-n);
                    }
                    iterator operator+(const difference_type& n) const {
                        return iterator(m_idx_ifs_vec, m_current_rec_num+n);
                    }
                    iterator& operator+=(const difference_type& n) {
                        this->m_current_rec_num += n;
                        return (*this);
                    }
            };

        iterator begin();
        iterator end();
        bounded_streambuf* at (int index);
        bounded_streambuf* operator [] (int index);
        bounded_streambuf* get_record_stream (int rec_num);
        int  size() const; 
        int  seek_record(int rec_num);
        int  get_record_len(int rec_num);
        void dump(std::ostream&  sout);
    protected:
        std::vector<index_record_t>  m_vec;
        std::ifstream   m_index_file;
        bounded_streambuf    m_bounded_data_record;

        void            init();
};

#endif
