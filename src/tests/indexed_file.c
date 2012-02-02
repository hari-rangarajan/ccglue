#include "indexed_file.h"
#include <iterator>
#include <iostream>

indexed_ofstream::~indexed_ofstream()
{
    dump(*this);
}

void indexed_ofstream::create_index_file()
{
    //m_ofs_index_file = new ofstream(m_index_filename.c_str());
}

void indexed_ofstream::begin_record ()
{
    m_record_pos_list.push_front(tellp());
}

void indexed_ofstream::end_record ()
{
    // Do nothing in particular for now
}


void indexed_ofstream::dump (ostream&  sout)
{
    ostream_iterator<char> out_it (sout,"");
    copy ( m_record_pos_list.begin(), m_record_pos_list.end(), out_it );
}

