#include "tag_file_writer.h"
#include "digraph.h"
#include "string.h"

tag_file_writer::tag_file_writer(const char* filename) 
{
    tagfile = new seq_file;
    tagfile->writer_create(filename);
}

void tag_file_writer::destroy() 
{
    tagfile->close();
}

tag_file_writer::~tag_file_writer() 
{
    destroy();
    delete tagfile;
}

void tag_file_writer::write_xref_tag_header ()
{
    const char *tstr;

    tstr = "!_TAG_FILE_FORMAT\t2\t/extended format; --format=1 will not"
                " append ;\" to lines/\n";
    tagfile->write_buffer(tstr, strlen(tstr));

    tstr = "!_TAG_FILE_SORTED\t1\t/0=unsorted, 1=sorted, 2=foldcase/\n";
    tagfile->write_buffer(tstr, strlen(tstr));
    tstr = "!_TAG_PROGRAM_NAME\t\tccglue for use with CCTree (Vim plugin)//\n";
    tagfile->write_buffer(tstr, strlen(tstr));
    tstr = "!_TAG_PROGRAM_URL\thttp://vim.sourceforge.net/scrip"
                "ts/script.php?script_id=2368\t/site/\n";
    tagfile->write_buffer(tstr, strlen(tstr));
}

void tag_file_writer::write_sym_as_tag (sym_entry *a_sym_entry) 
{
#define MAX_TEMP_BUFFER 1024
    char tmp[MAX_TEMP_BUFFER];
    const char *tstr;

    snprintf(tmp, MAX_TEMP_BUFFER, "%s#", a_sym_entry->get_uid_str()->str);
    tagfile->write_string(tmp);
    digraph_utils::uncompress_string_with_map_write_to_seqfile(digraph_maps::get_letter_map(),
            a_sym_entry->get_n()->str,tagfile);
    
    snprintf(tmp, MAX_TEMP_BUFFER, "\t\t/^\\$/;\" ", a_sym_entry->get_uid_str()->str);
    tagfile->write_string(tmp);

    tstr = "\tc:";
    tagfile->write_string(tstr);

    tstr = &a_sym_entry->get_c()->str[0];
    if (digraph_utils::compress_string_with_map_write_to_seqfile(
                digraph_maps::get_numeric_map(), tstr, tagfile) == RC_FAILURE) {
        perror("Compress seqfile failed");
        return;
    }

    tstr = "\tp:";
    tagfile->write_string(tstr);
    tstr = &a_sym_entry->get_p()->str[0];
    if (digraph_utils::compress_string_with_map_write_to_seqfile(
                digraph_maps::get_numeric_map(),tstr, tagfile) == RC_FAILURE) {
        perror("Compress seqfile failed");
        return;
    }
    tagfile->write_char('\n');
} 



