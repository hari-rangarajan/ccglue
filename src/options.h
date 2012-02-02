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

#ifndef OPTIONS_H
#define OPTIONS_H

#include "typedefs.h"

typedef struct ccglue_opts {
    char* cscope_dbs;
    char* tag_files;
    char* output_file;
    int    silent;
    int    verbose;
} ccglue_opts_t;

#ifdef __cplusplus
extern "C" {
#endif
    int ccglue_parse_command_line_options (int argc,
                          char **argv, ccglue_opts_t* opts);
#ifdef __cplusplus
}
#endif
#endif
