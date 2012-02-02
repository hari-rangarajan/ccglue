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

#include "typedefs.h"
#include <stdio.h>
#include <string.h>
#include "seqfile.h"

RC_t seq_file::create (const char* fname, const char* mode) 
{
    m_fp = fopen(fname, mode);
    if (m_fp == NULL) {
        return RC_FAILURE;
    }
    return RC_SUCCESS;
}

RC_t seq_file::close()
{
    fclose(m_fp);
    m_fp = NULL;
}

RC_t seq_file::destroy()
{
    if (m_fp) {
        close();
    }
}

seq_file::seq_file ()
{
    m_fp = NULL;
    mode[0] = mode[1] = 0;
}

seq_file::~seq_file ()
{
    destroy();
}

RC_t seq_file::reader_create (const char* fname) 
{
    return create(fname, "rb");
}

RC_t seq_file::writer_create (const char* fname) 
{
    return create(fname, "wb");
}

RC_t seq_file::write_char (uchar c)
{
    return write_buffer((const char *)&c, sizeof(c));
}

RC_t seq_file::write_string (const char* str)
{
    return write_buffer(str, strlen(str));
}

RC_t seq_file::write_buffer (const char* buffer, size_t len)
{
    if (fwrite(buffer, sizeof(char), len, m_fp) == len) {
        return RC_SUCCESS;
    } else {
        return RC_FAILURE;
    }
}

RC_t seq_file::read_until (const char* eol_char_list,
        uint8 eol_char_list_size, uchar* eol_char, uchar* line, int maxlen)
{
    int a;
    int idx = 0;
    uchar *l_eol_char;

    while ((a = fgetc(m_fp)) != EOF) {
        l_eol_char = (uchar *) memchr(eol_char_list, a, eol_char_list_size);
        if (l_eol_char != NULL) {
            break;
        } else if (idx >= maxlen) {
            /* TODO: we should handle this better */
            return RC_RUN_OUT_OF_MEMORY;
        }
        if (line) {
            line[idx++] = a;
        }
    }
    if (eol_char) {
        *eol_char = *l_eol_char;
    }
    if (a == EOF) {
        return RC_FAILURE;
    }
    if (line) {
        line[idx] = '\0';
    }
    return RC_SUCCESS;
}

RC_t seq_file::read_till_tab_or_newline (uchar* line,
        uchar* eol, int maxlen)
{
    return read_until("\t\n", 2, eol, line, maxlen);
}

RC_t seq_file::read_till_newline (uchar* line, int maxlen)
{
    return read_until("\n", 1, NULL, line, maxlen);
}



