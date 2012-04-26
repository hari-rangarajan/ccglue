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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include "typedefs.h"
#include "sym_mgr.h"
#include "cscoperdr.h"

RC_t process_files_with_reader (generic_db_rdr& reader, sym_table& a_sym_table, 
        std::vector<std::string>& db_files)
{
    char		 line[1024];
    std::ifstream        db_file;

    std::vector<std::string>::iterator  it;

    db_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    /* need to copy before we try tokenizing */
    for (it = db_files.begin(); it < db_files.end(); it++) {
        db_file.open((*it).c_str());
        if (db_file.fail()) {
        //    throw (*it);
        }
        while (!db_file.eof()) {
            db_file.getline(line, sizeof(line));
            reader.process_line(a_sym_table, line);
        }
        db_file.close();
    }

    return RC_SUCCESS;
}
    

RC_t process_cscope_files_to_build_sym_table (sym_table& a_sym_table, 
        std::vector<std::string>& cscope_files)
{
    cscope_db_rdr rdr;
    rdr.set_scan_action(ACTION_LOAD_SYMS);
    process_files_with_reader(rdr, a_sym_table, cscope_files);
}

RC_t process_cscope_files_to_build_xrefs (sym_table& a_sym_table,
        std::vector<std::string>& cscope_files)
{
    cscope_db_rdr rdr;
    rdr.set_scan_action(ACTION_XREF_SYMS);
    process_files_with_reader(rdr, a_sym_table, cscope_files);
}

