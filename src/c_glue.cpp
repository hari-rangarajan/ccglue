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
#include <stdexcept>
#include "typedefs.h"
#include "sym_mgr.h"
#include "cscoperdr.h"

void process_files_with_reader (generic_db_rdr& reader, sym_table& a_sym_table, 
        std::vector<std::string>& db_files, generic_db_scanner& scanner)
{
    std::ifstream                       db_file;
    std::vector<std::string>::iterator  it;

    /* need to copy before we try tokenizing */
    for (it = db_files.begin(); it < db_files.end(); it++) {
        db_file.open((*it).c_str(), std::ios::in);
        if (db_file.fail()) {
            throw std::runtime_error("Failed to open " + *it);
        }
        reader.process_line(a_sym_table, db_file, scanner);
        db_file.close();
    }
    return;
}
    

RC_t process_cscope_files_to_build_sym_table (sym_table& a_sym_table, 
        std::vector<std::string>& cscope_files)
{
    cscope_db_symbol_scanner  sym_scanner;
    cscope_db_rdr rdr;

    sym_scanner.initialize_rules();
    process_files_with_reader(rdr, a_sym_table, cscope_files, sym_scanner);
}

RC_t process_cscope_files_to_build_xrefs (sym_table& a_sym_table,
        std::vector<std::string>& cscope_files)
{
    cscope_db_xref_scanner  xref_scanner;
    cscope_db_rdr rdr;

    xref_scanner.initialize_rules();
    process_files_with_reader(rdr, a_sym_table, cscope_files, xref_scanner);
}

