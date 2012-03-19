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

/*  ccglue 
 * 
 *
 *  Released under the terms of the GNU GPL v2.0 */

#ifndef DIGRAPH_H
#define DIGRAPH_H

#include "typedefs.h"
#include <streambuf>

struct digraph_uncompress_map {
    uchar	table[128][2];
};

struct digraph_compress_map {
    uchar rev_index1[128];
    uchar rev_index2[128];
    uchar table[12][12];
};

typedef struct digraph_uncompress_map digraph_uncompress_map_t;
typedef struct digraph_compress_map digraph_compress_map_t;


class digraph_utils {
    public:
        static void build_char_compress_map(digraph_compress_map_t *, 
                const char* seq1, const char* seq2);
        static int  compress_string_with_map(digraph_compress_map_t* map, 
                uchar** uncmpstr, uchar* cmpstr, int cmpstr_maxlen);
        static void build_char_uncompress_map(digraph_uncompress_map_t *,
                const char* seq1, const char* seq2);
        static void uncompress_string_with_map(digraph_uncompress_map_t *, 
                const char* cmpstr, uchar* uncmpstr);
};

class digraph_maps {
    protected:
        static const char* numseq1;
        static const char* numseq2;
        static const char* charseq1;
        static const char* charseq2;
        static digraph_compress_map_t   numeric_map;
        static digraph_uncompress_map_t letter_map;

        static bool numeric_map_init;
        static bool letter_map_init;

    public:
        static digraph_compress_map_t* get_numeric_map();
        static digraph_uncompress_map_t* get_letter_map();
};


class digraph_compress_buf: public std::streambuf {
protected:
    digraph_compress_map_t *map;
    std::streambuf&         s_buf;
    traits_type::int_type   tmp_char;
    virtual traits_type::int_type overflow( int_type c = traits_type::eof() );
public:
    digraph_compress_buf(std::streambuf& buf):
        s_buf(buf), tmp_char(0){
            map = digraph_maps::get_numeric_map();
        };
};


class digraph_uncompress_buf: public std::streambuf {
protected:
    digraph_uncompress_map_t *map;
    std::streambuf&         s_buf;
    virtual traits_type::int_type overflow( int_type c = traits_type::eof() );
public:
    digraph_compress_buf(std::streambuf& buf):
        s_buf(buf){
            map = digraph_maps::get_letter_map();
        };
};

#endif
