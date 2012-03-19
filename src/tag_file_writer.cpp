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

#include "tag_file_writer.h"
#include "digraph.h"
#include "string.h"
#include <sstream>

tag_file_writer::tag_file_writer(const char* filename):
    tagfile(filename)
{
}

tag_file_writer::~tag_file_writer() 
{
    tagfile.close();
}

void tag_file_writer::write_xref_tag_header ()
{
    tagfile << "!_TAG_FILE_FORMAT\t2\t/extended format; --format=1 will not append ;\" to lines/\n";
    tagfile << "!_TAG_FILE_SORTED\t1\t/0=unsorted, 1=sorted, 2=foldcase/\n";
    tagfile << "!_TAG_PROGRAM_NAME\t\tccglue for use with CCTree (Vim plugin)//\n";
    tagfile << "!_TAG_PROGRAM_URL\thttp://vim.sourceforge.net/scripts/script.php?script_id=2368\t/site/\n";
}

void tag_file_writer::write_sym_as_tag (sym_entry *a_sym_entry) 
{
    digraph_compress_buf    compress_buf(*(tagfile.rdbuf()));
    std::ostream            compress_stream(&compress_buf);
    std::stringstream        sstream;
    digraph_uncompress_buf  uncompress_buf(*(sstream.rdbuf()));
    std::ostream            uncompress_stream(&uncompress_buf);

    uncompress_stream << a_sym_entry->get_n();

    tagfile << a_sym_entry->get_uid() << "#" << sstream.str();
    
    std::list<sym_entry *>::const_iterator   iter;

    tagfile <<  "\t\t/^\\$/;\" ";
    
    tagfile << "\tc:";

    for (iter = a_sym_entry->get_c().begin(); iter != a_sym_entry->get_c().end(); iter++) {
        //tagfile << (*iter)->get_uid() << ",";
        compress_stream << (*iter)->get_uid() << ",";
    }

    tagfile << "\tp:";

    for (iter = a_sym_entry->get_p().begin(); iter != a_sym_entry->get_p().end(); iter++) {
        //tagfile << (*iter)->get_uid() << ",";
        compress_stream << (*iter)->get_uid() << ",";
    }

    tagfile << '\n';
} 



