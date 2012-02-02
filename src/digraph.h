#ifndef DIGRAPH_H
#define DIGRAPH_H

#include "seqfile.h"
#include "typedefs.h"

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
        static int compress_string_with_map_write_to_seqfile(
                digraph_compress_map_t *,
                const char* uncmpstr, seq_file *);
        static void uncompress_string_with_map_write_to_seqfile(digraph_uncompress_map_t *map,
                const char* cmpstr, seq_file  *);
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

#endif
