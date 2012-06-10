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
#include <stdexcept>

tag_file_writer::tag_file_writer(const std::string& filename):
    tagfile(filename.c_str())
{
    if (tagfile.fail()) {
        throw std::runtime_error("Failed to write file " + filename);
    }
}

tag_file_writer::~tag_file_writer() 
{
    tagfile.close();
}

std::streampos tag_file_writer::get_file_pos ()
{
    return tagfile.tellp();
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
    std::stringstream       sstream;
    digraph_compress_buf    compress_buf(*(tagfile.rdbuf()),
                                           digraph_maps::get_numeric_compress_map());
    std::ostream            compress_stream(&compress_buf);

    tagfile << a_sym_entry->get_uid() << "#" << a_sym_entry->get_n();
    
    std::list<sym_entry_xref *>::const_iterator   iter;

    tagfile <<  "\t\t/^\\$/;\" ";
    
    tagfile << "\tc:";

    for (iter = a_sym_entry->get_c().begin(); iter != a_sym_entry->get_c().end(); iter++) {
        compress_stream 
        //tagfile
                << (*iter)->get_sym_entry()->get_uid() << "|" 
                << (*iter)->get_sym_entry_file()->get_uid() << "|"
                << (*iter)->get_line_num()  << ",";
    }
    compress_stream.flush();
    tagfile << "\tp:";

    for (iter = a_sym_entry->get_p().begin(); iter != a_sym_entry->get_p().end(); iter++) {
        compress_stream 
        //tagfile 
                << (*iter)->get_sym_entry()->get_uid() << "|" 
                << (*iter)->get_sym_entry_file()->get_uid() << "|"
                << (*iter)->get_line_num()  << ",";
    }
    compress_stream.flush();
    
    tagfile << '\n';
} 




