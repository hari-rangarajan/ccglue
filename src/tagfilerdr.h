#ifndef TAGFILERDR_H
#define TAGFILERDR_H

#include "typedefs.h"
#include "seqfile.h"

typedef int tag_type_t;

#define MAX_SIZE_LINE_NUMBER   256
#define MAX_SIZE_TAG_NAME   256
#define MAX_SIZE_TAG_FILE_NAME 512
#define MAX_SIZE_TAG_LOCATION 1
#define MAX_SIZE_TAG_LINE 4096

typedef struct {
    uchar8 name[MAX_SIZE_TAG_NAME];
    uchar8 filename[MAX_SIZE_TAG_FILE_NAME];
    uchar8 linenumber[MAX_SIZE_LINE_NUMBER];
    uchar8 location[MAX_SIZE_TAG_LOCATION];
    tag_type_t type;
} tag_t;


class tag_file_rdr {
    protected:
        seq_file *m_tag_file;
    public:
        tag_file_rdr();
        ~tag_file_rdr();
        void destroy();
        RC_t open(const char *);
        RC_t close();
        RC_t read_next_tag(tag_t *);
};

#endif
