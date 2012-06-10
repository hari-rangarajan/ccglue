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


/* Need to revisit, made a mess */

#include "typedefs.h"
#include "digraph.h"
#include <string.h>
#include <iostream>
#include "debug.h"

void digraph_utils::build_char_compress_map (digraph_compress_map_t *map, 
        const char* seq1, const char* seq2)
{
    int i,j, idx = 128;
            
    debug(0)  << "Building compressed table for " <<
              "seq1: " << seq1 <<
              "seq2: " << seq2 << "\n";

    memset(map, -1, sizeof(digraph_compress_map_t));
    /* build reverse indexes */
    for(i=0; i<strlen(seq1); i++) {
        map->rev_index1[seq1[i]] = i;
    }
    for(i=0; i<strlen(seq2); i++) {
        map->rev_index2[seq2[i]] = i;
    }
    for(i=0; i<strlen(seq1); i++) {
        for (j=0; j<strlen(seq2); j++) {
            debug(0) << "Assigning id " << idx <<
                " sym1: " << seq1[i] <<
                " sym2: " << seq2[j] << "\n";

            map->table[i][j] =  idx++;
        }
    }
}


int digraph_utils::compress_string_with_map (digraph_compress_map_t* map, uchar** uncmpstr, uchar* cmpstr, 
        int cmpstr_maxlen)
{
    int idx;
    int cmpstr_len = 0;
    int rc = 0;             /* finished the entire string */

    while (**uncmpstr != '\0') {
        if (map->rev_index1[**uncmpstr] != 0xFF) {
            if ((*uncmpstr)[1] != '\0' && map->rev_index2[(*uncmpstr)[1]] != 0xFF) {
                *cmpstr = map->table[map->rev_index1[**uncmpstr]][map->rev_index2[(*uncmpstr)[1]]];
                cmpstr++;
                *uncmpstr += 2;
            } else {
                *cmpstr = **uncmpstr;
                cmpstr++;
                (*uncmpstr)++;
            }
        } else {
            *cmpstr = **uncmpstr;
            cmpstr++;
            (*uncmpstr)++;
        }
        cmpstr_len++;
        if (cmpstr_len == cmpstr_maxlen) {
            rc = 1; /* more to come */
            break;
        }
    }
    *cmpstr = '\0';
    return rc;
}


void digraph_utils::build_char_uncompress_map (digraph_uncompress_map_t *map, 
        const char* seq1, const char* seq2)
{
    int i,j, idx = 0;
    char idxchar;

    for(i=0; i<strlen(seq1); i++) {
        for (j=0; j<strlen(seq2); j++) {
            map->table[idx][0] = seq1[i];
            map->table[idx][1] = seq2[j];
            idx++;
        }
    }
}



void digraph_utils::uncompress_string_with_map (digraph_uncompress_map_t *map,
        const char* cmpstr, uchar* uncmpstr)
{
    int idx;

    while (*cmpstr != '\0') {
        idx = (int)((uchar) (*cmpstr)) - 128;
        if (idx > 0) {
            uncmpstr[0] = map->table[idx][0];
            uncmpstr[1] = map->table[idx][1];
            uncmpstr += 2*sizeof(uchar);
        } else {
            *uncmpstr = *cmpstr;
            uncmpstr++;
        }
        cmpstr++;
    }
    *uncmpstr = '\0';
}
        
const char* digraph_maps::numseq1 = "|0123456789";
const char* digraph_maps::numseq2 = ",0123456789";
const char* digraph_maps::charseq1 = " teisaprnl(of)=c";
const char* digraph_maps::charseq2 = " tnerpla";
        
digraph_uncompress_map_t   digraph_maps::numeric_uncompress_map;
digraph_compress_map_t   digraph_maps::numeric_compress_map;
digraph_uncompress_map_t digraph_maps::letter_uncompress_map;

bool digraph_maps::numeric_compress_map_init = false;
bool digraph_maps::numeric_uncompress_map_init = false;
bool digraph_maps::letter_uncompress_map_init = false;

/* build singleton access methods here */

digraph_compress_map_t* digraph_maps::get_numeric_compress_map ()
{
    if (digraph_maps::numeric_compress_map_init == false) {
        digraph_utils::build_char_compress_map(&numeric_compress_map, numseq1, numseq2);
        digraph_maps::numeric_compress_map_init = true;
    }
    return &digraph_maps::numeric_compress_map;
}

digraph_uncompress_map_t* digraph_maps::get_numeric_uncompress_map ()
{
    if (digraph_maps::numeric_uncompress_map_init == false) {
        digraph_utils::build_char_uncompress_map(&numeric_uncompress_map, numseq1, numseq2);
        digraph_maps::numeric_uncompress_map_init = true;
    }
    return &digraph_maps::numeric_uncompress_map;
}



digraph_uncompress_map_t* digraph_maps::get_letter_uncompress_map ()
{
    if (digraph_maps::letter_uncompress_map_init == false) {
        digraph_utils::build_char_uncompress_map(&letter_uncompress_map, charseq1, charseq2);
        digraph_maps::letter_uncompress_map_init = true;
    }
    return &digraph_maps::letter_uncompress_map;
}

using namespace std;


int digraph_compress_buf::sync() 
{
    if (tmp_char != '\0') {
        s_buf.sputc(traits_type::to_char_type(tmp_char));
        tmp_char = '\0';
    }
}

digraph_compress_buf::int_type 
    digraph_compress_buf::overflow( int_type c ) {
        traits_type::int_type   wc;

        debug(0) << "coming here " << traits_type::to_char_type(c) << "\n";
        if( c != traits_type::eof() ){
            if (tmp_char == '\0') {
                if (m_map->rev_index1[c] != 0xFF) {
                    tmp_char = c;
                } else {
                    debug(0) << "\t\t\twriting_ " << c << "\n";
                    s_buf.sputc(traits_type::to_char_type(c));
                    tmp_char = '\0';
                }
            } else {
                if (m_map->rev_index2[c] == 0xFF) {
                    s_buf.sputc(traits_type::to_char_type(tmp_char));
                    s_buf.sputc(traits_type::to_char_type(c));
                    debug(0) << "\t\t\twriting__" << tmp_char << " and " << c << "\n";
                    tmp_char = '\0';
                } else {
                    wc = m_map->table[m_map->rev_index1[tmp_char]]
                        [m_map->rev_index2[c]];
                    debug(0) << "\t\t\tlookup " << wc << "\n";
                    if (wc == 0xFF) {
                        debug(0) << "\t\t\twriting " << tmp_char << "\n";
                        s_buf.sputc(traits_type::to_char_type(tmp_char));
                        tmp_char = c;
                    } else {
                        s_buf.sputc(traits_type::to_char_type(wc));
                        debug(0) << "\t\t\tcompressing " << wc << "\n";
                        tmp_char = '\0';
                    }
                }
            }
        }
    return c;
} 

digraph_uncompress_buf::int_type digraph_uncompress_buf::uflow() 
{
    traits_type::int_type   c;
    if (num_tmp == 0) {
        c = underflow();
        if (c == traits_type::eof()) {
            return c;
        }
    } 
    c = tmp_char[0];
    tmp_char[0] = tmp_char[1];
    num_tmp--;

    debug(0) << "\t\t\tsending " << (char) c << endl;
    return c;
}


digraph_uncompress_buf::int_type 
    digraph_uncompress_buf::underflow() {
        traits_type::int_type c;

        debug(0) << " getting " << (int) s_buf.sgetc() << " :: ";
        if (num_tmp == 0) {
            c = s_buf.sbumpc();
        } else {
            c = tmp_char[0];
            return c;
        }
        if (c == traits_type::eof()) {
            return c;
        }
        int idx = (int)((unsigned char) c) - 128;
        if (idx > 0) {
            tmp_char[0] = m_map->table[idx][0];
            tmp_char[1] = m_map->table[idx][1];
            num_tmp = 2;
            return tmp_char[0];
        } else { 
            tmp_char[0] = c;
            num_tmp = 1;
            return c;
        }
}

#if 0
digraph_uncompress_buf::int_type 
    digraph_uncompress_buf::overflow( int_type c ) {
        int idx = c - 128;
   //////debug(0) << "index " << idx << "\n";
        if (idx > 0) {
 //           ////debug(0) << "writing " << m_map->table[idx][0] << "," << map->table[idx][1] <<  "\n";
            s_buf.sputc(m_map->table[idx][0]);
            s_buf.sputc(m_map->table[idx][1]);
        } else {
            s_buf.sputc(traits_type::to_char_type(c));
        }
        return c;
}
#endif
