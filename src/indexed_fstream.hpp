#ifndef INDEXED_FSTREAM_HXX
#define INDEXED_FSTREAM_HXX

#include "indexed_fstream.h"

indexed_ofstream::indexed_ofstream(const std::string& filename):
    m_ofs(filename.c_str(), std::ios::out | std::ios::binary) 
{
    if (m_ofs.fail()) {
        throw std::runtime_error("Failed to open file " + filename);
    }
}

indexed_ofstream::~indexed_ofstream()
{
}
        
void indexed_ofstream::end_record()
{
    m_index_record.size = m_ofs.tellp() -
        m_index_record.record_start;
    m_record_pos_list.push_back(m_index_record);
}
        
void indexed_ofstream::begin_record() 
{
    m_index_record.record_start = m_ofs.tellp();
}
        
void indexed_ofstream::write_index_to_file() 
{
    long            stream_pos = m_ofs.tellp();
    std::ostream    os(m_ofs.rdbuf());
    write_records_binary(os);

    os.write(reinterpret_cast<const char *>(&stream_pos), sizeof(std::streampos));
};
        
void indexed_ofstream::write_records_binary(std::ostream&  sout)
{
    typename std::list<index_record_t>::iterator  iter;

    for (iter = m_record_pos_list.begin(); iter != m_record_pos_list.end(); iter++) {
        sout.write(reinterpret_cast<const char *>(&(*iter)), sizeof(index_record_t));
    }
}
        
std::streambuf* indexed_ofstream::rdbuf() 
{
    return (m_ofs.rdbuf());
}
        
void indexed_ofstream::close() 
{
    m_ofs.close();
}


//end of class
        
bounded_streambuf::bounded_streambuf(std::streambuf *buf, int bound):
            m_buf(buf), m_bound(bound)
{
    setg(0,0,0);
}
        
int bounded_streambuf::setbound (int numc) 
{
    m_bound = numc;
}


std::streambuf::traits_type::int_type  bounded_streambuf::sbumpc() 
{
    return m_buf->sbumpc();
}
        
std::streambuf::traits_type::int_type bounded_streambuf::sgetc() 
{
    return m_buf->sgetc();
}
        
std::streambuf::traits_type::int_type bounded_streambuf::uflow() 
{
    m_bound--;
    if (m_bound == 0) {
        return traits_type::eof();
    }
    char c = m_buf->sbumpc();
    return c;
}
        
std::streambuf::traits_type::int_type bounded_streambuf::underflow() 
{
    if (m_bound == 0) {
        return traits_type::eof();
    }
    char c = m_buf->sgetc();
    return c;
}
        
std::streampos bounded_streambuf::seekoff(std::streamoff off, 
                std::ios::seekdir way,  std::ios::openmode which)
{
    m_bound -= off;
    return m_buf->pubseekoff(off, way, which);
}
// end of class implementation


template <class T>
void indexed_ifstream_vector<T>::init ()
{
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
        
template <class T>
indexed_ifstream_vector<T>::indexed_ifstream_vector(const std::string& index_file):
            m_index_file(index_file.c_str(), std::ios::in | std::ios::binary),
            m_bounded_data_record(m_index_file.rdbuf()) 
{ 
    if (m_index_file.fail()) {
        throw std::runtime_error("Failed to open file " + index_file); 
    }
    init();
};

template <class T>
typename indexed_ifstream_vector<T>::iterator indexed_ifstream_vector<T>::begin()
{
    return indexed_ifstream_vector<T>::iterator(*this, 0);
}
template <class T>
typename indexed_ifstream_vector<T>::iterator indexed_ifstream_vector<T>::end()
{
    return indexed_ifstream_vector<T>::iterator(*this, this->size()-1);
}
template <class T>
bounded_streambuf* indexed_ifstream_vector<T>::operator [] (int index)
{
    return get_record_stream(index);
}
template <class T>
bounded_streambuf* indexed_ifstream_vector<T>::get_record_stream (int rec_num)
{
    seek_record(rec_num);
    m_bounded_data_record.setbound(get_record_len(rec_num));
    return &m_bounded_data_record;
}

template <class T>
int  indexed_ifstream_vector<T>::size() const 
{ 
    return (m_vec.size());
}
template <class T>
int  indexed_ifstream_vector<T>::seek_record(int rec_num)
{
    int count;
    m_index_file.seekg(m_vec.at(rec_num).record_start, std::ios_base::beg);
    return false;
}
template <class T>
int  indexed_ifstream_vector<T>::get_record_len(int rec_num)
{
    return m_vec.at(rec_num).size;
}
template <class T>
void indexed_ifstream_vector<T>::dump(std::ostream&  sout)
{
    std::ostream_iterator<char> out_it (sout,"");
    copy ( this->begin(), this->end(), out_it );
}

#endif
