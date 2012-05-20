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

#include <iostream>
#include "options.h"
#include "sym_mgr.h"
#include "c_glue.h"
#include "tclap/CmdLine.h"

int main (int argc, char **argv)
{
    ccglue_opts  opts;
    sym_table    a_sym_table;

    /* Parse our arguments; every option seen by parse_opt will
       be reflected in arguments. */
    try {
        ccglue_parse_command_line_options(argc, argv, &opts);
    }
    catch ( TCLAP::ArgException& e ) { 
        std::cout << "ERROR: " << e.error() << " " << e.argId() << std::endl; 
    }
    
    try {
        process_cscope_files_to_build_sym_table(a_sym_table, opts.cscope_dbs);
        std::cout << "built sym table\n";
        process_cscope_files_to_build_xrefs(a_sym_table, opts.cscope_dbs);
        a_sym_table.write_xref_tag_file(opts.output_file, opts.output_index_file);
    }
    catch (std::exception& e) {
        std::cerr << e.what() << "\n";
    }
    return 0;
}

