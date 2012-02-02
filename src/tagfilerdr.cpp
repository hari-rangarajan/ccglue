#include <string.h>
#include "tagfilerdr.h"
#include "seqfile.h"
#include "debug.h"

tag_file_rdr::tag_file_rdr ()
{
    m_tag_file = NULL;
}

RC_t tag_file_rdr::open (const char *tag_fname)
{
    m_tag_file = new seq_file;
    // TODO: throw exception here
    return m_tag_file->reader_create(tag_fname);
}

RC_t tag_file_rdr::close ()
{
    return m_tag_file->close();
}

void tag_file_rdr::destroy ()
{
    delete m_tag_file;
}

RC_t tag_file_rdr::read_next_tag (tag_t *tag)
{
    uchar eol;

    while (m_tag_file->read_till_tab_or_newline(&tag->name[0], &eol, 
                MAX_SIZE_TAG_NAME) != RC_FAILURE) {
        if (tag->name[0] == '!') {
            /* comment line in tag file, skip away */
            if (m_tag_file->read_till_newline(NULL, MAX_SIZE_TAG_LINE)
                    == RC_SUCCESS) {
                debug::log("Skipping tag ! line\n");
                continue;
            } else {
                return RC_FAILURE;
            }
        } else {
            debug::log("Read tag name %s\n", tag->name);
            /* a real tag entry */
            if (m_tag_file->read_till_tab_or_newline( 
                        &tag->filename[0], &eol, 
                 MAX_SIZE_TAG_FILE_NAME) != RC_SUCCESS || eol == '\n') {
                return RC_FAILURE;
            }
            debug::log("Read tag filename %s\n", tag->filename);
            if (m_tag_file->read_till_tab_or_newline(&tag->linenumber[0],
                        &eol, MAX_SIZE_LINE_NUMBER) != RC_SUCCESS || eol == '\n') {
                return RC_FAILURE;
            }
            /* is this size portable? */
            if (m_tag_file->read_till_tab_or_newline((uchar *) &tag->type, &eol, 
                        sizeof(uchar)) != RC_SUCCESS) {
                return RC_FAILURE;
            }
            debug::log("Read tag type %d\n", tag->type);
            if (eol == '\n') {
                return RC_SUCCESS;
            } /* end of tag after type */
            /* Skip the rest of the tag */
            if (m_tag_file->read_till_newline(NULL, MAX_SIZE_TAG_LINE)
                    == RC_SUCCESS) {
                /* An entire tag line was read correctly */
                return RC_SUCCESS;
            }
        }
    }
    return RC_FAILURE;
}

