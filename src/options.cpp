#include "tclap/CmdLine.h"
#include <iostream>
#include <string>
#include "options.h"
#include "misc_util.h"

const char *ccglue_program_version = "0.4.0";

/* Program documentation. */
const char *ccglue_doc =
            "ccglue (version 0.4.0) - cscope ctags glue application. Produces cross-reference tags file"
            " for use with Vim CCTree plugin "
            "(http://www.vim.org/scripts/script.php?script_id=2368)."
            " CTags support not implemented yet."
            "\nReport bugs to: Hari Rangarajan "
            "<hariranga@users.sourceforge.net>";


void parse_options (int argc, char **argv, ccglue_opts *opts)
{
    TCLAP::CmdLine cmd(ccglue_doc, ' ', 
            ccglue_program_version);
    //
    // Define arguments
    //
    TCLAP::SwitchArg verboseSwitch("v","verbose", 
            "Produce verbose output", cmd, false);
    TCLAP::SwitchArg quietSwitch("q","quiet", 
            "Don't produce any output", cmd, false);
    TCLAP::ValueArg<std::string> cscopeFiles("S",
            "cscopefiles",
            "List of cscope databases to parse "
            "(i.e, cscope1.out,cscope2.out,...,cscopeN.out)."
            " Default: cscope.out",
            false,
            "cscope.out",
            "cscope1.out,[cscope2,...,cscopeN]"
            );
    cmd.add(cscopeFiles);

#if 0
    TCLAP::ValueArg<std::string> ctagFiles(
            "T",
            "ctagfiles",
            "List of ctags databases to parse (i.e, tags1,tags2,...,tags3)."
            "Default: tags",
            false,
            "tags",
            "tags[,tags1,...,tagsN]"
            );
    cmd.add(ctagFiles);
#endif
    TCLAP::ValueArg<std::string> outputFile ("o", 
            "output", 
            "Output to FILE ( - for standard output). Default: ccglue.out",
            false,
            "ccglue.out",
            "FILE"
            );
    cmd.add(outputFile);

    cmd.parse(argc, argv);
    opts->verbose = verboseSwitch.getValue(); 
    opts->silent = quietSwitch.getValue(); 
    opts->output_file = outputFile.getValue(); 
    misc_utils::string_split(cscopeFiles.getValue(), ',', opts->cscope_dbs);
}

int ccglue_parse_command_line_options (int argc, char **argv,
        ccglue_opts* opts)
{
    /* Parse our arguments; every option seen by parse_opt will
       be reflected in arguments. */
    parse_options (argc, argv, opts);

}

