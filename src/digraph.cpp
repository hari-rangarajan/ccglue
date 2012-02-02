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
#include "seqfile.h"

void digraph_utils::build_char_compress_map (digraph_compress_map_t *map, 
        const char* seq1, const char* seq2)
{
    int i,j, idx = 128;

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
        idx = (uchar) (*cmpstr) - 128;
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

int digraph_utils::compress_string_with_map_write_to_seqfile (digraph_compress_map_t * map, 
       const char* uncmpstr, seq_file *a_seq_file)
{
    int idx;
    int rc = 0;             /* finished the entire string */
    uchar cmpstr;

    while (*uncmpstr != '\0') {
        if (map->rev_index1[*uncmpstr] != 0xFF) {
            if ((uncmpstr)[1] != '\0' && map->rev_index2[(uncmpstr)[1]] != 0xFF) {
                cmpstr = map->table[map->rev_index1[*uncmpstr]]
                    [map->rev_index2[(uncmpstr)[1]]];
                uncmpstr += 2;
            } else {
                cmpstr = *uncmpstr;
                (uncmpstr)++;
            }
        } else {
            cmpstr = *uncmpstr;
            (uncmpstr)++;
        }
        a_seq_file->write_char(cmpstr);
    }
    return rc;
}


void digraph_utils::uncompress_string_with_map_write_to_seqfile (digraph_uncompress_map_t * map,
        const char* cmpstr, seq_file  *a_seq_file)
{
    int idx;

    while (*cmpstr != '\0') {
        idx = (uchar) (*cmpstr) - 128;
        if (idx > 0) {
            a_seq_file->write_char(map->table[idx][0]);
            a_seq_file->write_char(map->table[idx][1]);
        } else {
            a_seq_file->write_char(*cmpstr);
        }
        cmpstr++;
    }
}

        
const char* digraph_maps::numseq1 = ",0123456789";
const char* digraph_maps::numseq2 = ",0123456789";
const char* digraph_maps::charseq1 = " teisaprnl(of)=c";
const char* digraph_maps::charseq2 = " tnerpla";
        
digraph_compress_map_t   digraph_maps::numeric_map;
digraph_uncompress_map_t digraph_maps::letter_map;

bool digraph_maps::numeric_map_init = false;
bool digraph_maps::letter_map_init = false;

/* build singleton access methods here */

digraph_compress_map_t* digraph_maps::get_numeric_map ()
{
    if (digraph_maps::numeric_map_init == false) {
        digraph_utils::build_char_compress_map(&numeric_map, numseq1, numseq2);
        digraph_maps::numeric_map_init = true;
    }
    return &digraph_maps::numeric_map;
}


digraph_uncompress_map_t* digraph_maps::get_letter_map ()
{
    if (digraph_maps::letter_map_init == false) {
        digraph_utils::build_char_uncompress_map(&letter_map, charseq1, charseq2);
        digraph_maps::letter_map_init = true;
    }
    return &digraph_maps::letter_map;
}

