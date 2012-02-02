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

#include <argp.h>
#include "options.h"

const char *argp_program_version = "ccglue 0.3.0";
const char *argp_program_bug_address = "Hari Rangarajan "
                                        "<hariranga@users.sourceforge.net>";

/* Program documentation. */
static char ccglue_doc[] =
"ccglue - cscope ctags glue. Produces cross-reference tags file"
" for use with Vim CCTree plugin "
"(http://www.vim.org/scripts/script.php?script_id=2368)."
" CTags support not implemented yet.";

/* A description of the arguments we accept. */
static char ccglue_args_doc[] = "<None>";

/* The options we understand. */
static struct argp_option ccglue_options[] = {
	{"verbose",  'v', 0,      0,  "Produce verbose output" },
	{"quiet",    'q', 0,      0,  "Don't produce any output" },
	{"silent",   's', 0,      OPTION_ALIAS },
	{"cscopefiles",'S', "cscope1.out,[cscope2,...,cscopeN]", 0, 
        "List of cscope databases to parse "
        "(i.e, cscope1.out,cscope2.out,...,cscopeN.out)."
        " Default: cscope.out"},
	{"ctagsfiles",'T', "tags1[,tags2,...,tagsN]", 0, 
            "List of ctags databases to parse (i.e, tags1,tags2,...,tags3)."
            "Default: tags"},
	{"output",   'o', "FILE", 0,
	 "Output to FILE ( - for standard output). Default: ccglue.out" },
	{ 0 }
};

/* Parse a single option. */
static error_t
ccglue_parse_opt (int key, char *arg, struct argp_state *state)
{
	/* Get the input argument from argp_parse, which we
	   know is a pointer to our arguments structure. */
	ccglue_opts_t *opts = (ccglue_opts_t *) state->input;

	switch (key)
	{
		case 'q': case 's':
			opts->silent = 1;
			break;
		case 'v':
			opts->verbose = 1;
			break;
		case 'T':
			opts->tag_files = arg;
			break;
		case 'S':
			opts->cscope_dbs = arg;
			break;
		case 'o':
			opts->output_file = arg;
			break;

		case ARGP_KEY_ARG:
			if (state->arg_num != 0)
				/* Too many opts. */
				argp_usage (state);

			// opts->args[state->arg_num] = arg;

			break;

		case ARGP_KEY_END:
			if (state->arg_num != 0)
				/* Not enough opts. */
				argp_usage (state);
			break;

		default:
			return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

/* Our argp parser. */
static struct argp argp = { ccglue_options, ccglue_parse_opt, 
    ccglue_args_doc, ccglue_doc };

int ccglue_parse_command_line_options (int argc, char **argv,
        ccglue_opts_t* opts)
{
    /* Parse our arguments; every option seen by parse_opt will
       be reflected in arguments. */
    argp_parse (&argp, argc, argv, 0, 0, opts);

}

