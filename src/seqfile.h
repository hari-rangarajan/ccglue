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
#ifndef SEQFILE_H
#define SEQFILE_H

#include "stdio.h"

class seq_file {
    protected:
        FILE *m_fp;
        uchar mode[2];
        RC_t read_until(uchar* eol_char_list,
                uint8 eol_char_list_size, uchar* eol_char, uchar* line, int maxlen);
        RC_t destroy();

    public:
        seq_file();
        virtual ~seq_file();
        RC_t close();
        RC_t create(const char* fname, const char* mode);
        RC_t read_until (const char* eol_char_list,
                uint8 eol_char_list_size, uchar* eol_char, 
                uchar* line, int maxlen);
        RC_t read_till_newline(uchar* line, int maxlen);
        RC_t read_till_tab_or_newline(uchar* line,
                uchar* eol, int maxlen);
        RC_t write_buffer(const char* buffer, size_t len);
        RC_t write_string(const char* str);
        RC_t write_char(uchar c);

        RC_t reader_create(const char* fname);
        RC_t writer_create(const char* fname);
};

#if 0
class indexed_seq_file: public seq_file {
    protected:
        GArray 
    public:
        indexed_seq_file():seqfile(){};
        ~indexed_seq_file();
        
};

#endif

#endif
