#ifndef INDEXED_FSTREAM_H
#define INDEXED_FSTREAM_H

#include <list>
#include <fstream>
#include <iterator>
#include <vector>
#include <iostream>
#include <memory>
        
typedef struct {
    std::streampos             record_start;
    int                        size;
} index_record_t;

class indexed_ofstream {
    public:
        indexed_ofstream(const char *filename):
            m_ofs(filename, std::ios::out | std::ios::binary) {};
        ~indexed_ofstream() {};
        void end_record() {
            m_index_record.size = m_ofs.tellp() -
                m_index_record.record_start;
            std::cout << "writing record size "
                 << m_index_record.size << std::endl;
            m_record_pos_list.push_back(m_index_record);
        }
        void begin_record() {
            m_index_record.record_start = m_ofs.tellp();
        }
        void write_index_to_file() {
            long            stream_pos = m_ofs.tellp();
            std::ostream    os(m_ofs.rdbuf());
            write_records_binary(os);

            os.write(reinterpret_cast<const char *>(&stream_pos), sizeof(std::streampos));
        };
        void write_records_binary(std::ostream&  sout) {
            typename std::list<index_record_t>::iterator  iter;

            for (iter = m_record_pos_list.begin(); iter != m_record_pos_list.end(); iter++) {
                //sout.write(reinterpret_cast<const char *>(&(iter->record_start)), sizeof(long));
                //sout.write(reinterpret_cast<const char *>(&(iter->size)), sizeof(char));
                sout.write(reinterpret_cast<const char *>(&(*iter)), sizeof(index_record_t));
            }
        }
        void dump(std::ostream&  sout);
        std::streambuf* rdbuf() {return (m_ofs.rdbuf());};
        void close() {m_ofs.close();};
    protected:
        std::ofstream                m_ofs;
        index_record_t               m_index_record;
        std::list<index_record_t>    m_record_pos_list;
    private:
        void create_index_file();
};


class bounded_streambuf:public std::streambuf {
    protected:
        int              m_bound;
        
    public:
        std::streambuf  *m_buf;
        bounded_streambuf(std::streambuf *buf, int bound=0):
            m_buf(buf), m_bound(bound) {
            setg(0,0,0);
        };
        int     setbound (int numc) {
            m_bound = numc;
        };
        traits_type::int_type sbumpc() {
            return m_buf->sbumpc();
        };
        traits_type::int_type sgetc() {
            return m_buf->sgetc();
        };

        traits_type::int_type uflow() {
            m_bound--;
            if (m_bound == 0) {
                return traits_type::eof();
            }
            char c = m_buf->sbumpc();
            std::cout << "gettingnext " << (int)((unsigned char) c) << "[" <<
                c << "]" << std::endl;
            return c;
        };
        traits_type::int_type underflow() {
            if (m_bound == 0) {
                return traits_type::eof();
            }
            char c = m_buf->sgetc();
            std::cout << "gettingb " << (int)((unsigned char) c) <<   "[" << c << "]" 
                << std::endl;
            return c;
        };
        std::streampos seekoff(std::streamoff off, 
                std::ios::seekdir way,  std::ios::openmode which)
        {
            m_bound -= off;
            return m_buf->pubseekoff(off, way, which);
        }
};



template <class T>
class indexed_ifstream_vector {
    public:
        explicit indexed_ifstream_vector(const char* index_file):
            m_index_file(index_file, std::ios::in | std::ios::binary),
            m_bounded_data_record(m_index_file.rdbuf()) { 
                init();
            };
        class iterator:
            public std::iterator<std::random_access_iterator_tag, std::string > {
                protected:
                    indexed_ifstream_vector<T>&                     m_idx_ifs_vec;
                    int                                             m_current_rec_num;
                public:
                    typedef typename std::iterator<std::input_iterator_tag, indexed_ifstream_vector<T> >::difference_type difference_type;
                    friend class indexed_ifstream_vector;
                    iterator (indexed_ifstream_vector<T>& v,
                            int index = 0):
                        m_idx_ifs_vec(v), m_current_rec_num(index) {};

                    iterator& operator++(int) {
                        //tmp = iterator(*this);
                        m_current_rec_num++;
                        return (*this);
                    }

                    iterator& operator++() {
                        m_current_rec_num++;
                        return (*this);
                    }
#if 0
                    iterator& operator=(iterator rhs) {
                        //m_idx_ifs_vec = rhs.m_idx_ifs_vec;
                        m_current_rec_num = rhs.m_current_rec_num;
                        return (*this);

                        //return iterator(rhs.m_idx_ifs_vec, rhs.m_current_rec_num);
                    }
#endif
                    iterator& operator=(const iterator& rhs) {
                        //m_idx_ifs_vec = rhs.m_idx_ifs_vec;
                        m_current_rec_num = rhs.m_current_rec_num;
                        return (*this);

                        //return iterator(rhs.m_idx_ifs_vec, rhs.m_current_rec_num);
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
#if 0
                    bool operator != (iterator rhs) {
                        return !(this->operator==(rhs));
                    }
#endif
                    std::streambuf* operator* () {
                        return m_idx_ifs_vec[m_current_rec_num];
                    }
                    
                    difference_type operator-(const iterator& iter) const {
                        return m_current_rec_num - iter.m_current_rec_num;
                    }
#if 0
                    iterator operator-(const iterator& iter) const {
                        return iterator(m_idx_ifs_vec,
                                m_current_rec_num - iter.m_current_rec_num);
                    }
#endif
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

        iterator begin() {
            return iterator(*this, 0);
        };
        iterator end() {
            return iterator(*this, this->size()-1);
        };
        bounded_streambuf* operator [] (int index) {
            //char buf[512];
            //read_record(index, buf, sizeof(buf));
            std::cout << "retrieving " << index << std::endl;
            return get_record_stream(index);
        };
        
        bounded_streambuf* get_record_stream (int rec_num) {
            seek_record(rec_num);
            m_bounded_data_record.setbound(get_record_len(rec_num));
            std::cout << " bounded record " << get_record_len(rec_num) << std::endl;
            return &m_bounded_data_record;
        };

        int  size() const { return (m_vec.size());};
        int  seek_record(int rec_num) {
            int count;
            
            m_index_file.seekg(m_vec.at(rec_num).record_start, std::ios_base::beg);

#if 0
            m_index_file.seekg(sizeof(index_record_t), std::ios_base::cur);
            std::istreambuf_iterator<char>   sbuf_it(m_index_file.rdbuf());
            count = get_record_len(rec_num) - 24;
            std::cout << "seeking \n";
            while (count > 0) {
                std::cout << *sbuf_it;
                sbuf_it++;
                count--;
            }

            std::cout << "\n";
#endif
            return false;
        };
        int  get_record_len(int rec_num) {
            return m_vec.at(rec_num).size;
        };
        void dump(std::ostream&  sout) {
            std::ostream_iterator<char> out_it (sout,"");
            copy ( this->begin(), this->end(), out_it );
        };
    protected:
        std::vector<index_record_t>  m_vec;
        std::ifstream   m_index_file;
        bounded_streambuf    m_bounded_data_record;

        void            init() {
            std::streampos beg_of_idx;
            std::streampos end_of_idx;
            int num_records;

            m_index_file.seekg(-sizeof(std::streampos), std::ios_base::end);
            m_index_file.read(reinterpret_cast<char *>(&beg_of_idx), sizeof(std::streampos));
            end_of_idx =  m_index_file.tellg();
            m_index_file.seekg(beg_of_idx, std::ios_base::beg);

            num_records = (end_of_idx - beg_of_idx)/sizeof(index_record_t);

            m_vec.resize(num_records);
            m_index_file.read(reinterpret_cast<char *>(&m_vec[0]), m_vec.size() * sizeof(index_record_t));
        }
};


#endif
