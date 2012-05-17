#include "tclap/CmdLine.h"
#include <iostream>
#include <string>
#include "tracer_options.h"

const char *ccglue_tracer_program_version = "0.4.0";

/* Program documentation. */
const char *ccglue_tracer_doc =
            "ccglue tracer - cscope ctags glue application. Produces cross-reference tags file"
            " for use with Vim CCTree plugin "
            "(http://www.vim.org/scripts/script.php?script_id=2368)."
            " CTags support not implemented yet."
            "\nReport bugs to: Hari Rangarajan "
            "<hariranga@users.sourceforge.net>";

void parse_options (int argc, char **argv, ccglue_tracer_opts *opts)
{
    TCLAP::CmdLine cmd(ccglue_tracer_doc, ' ', 
            ccglue_tracer_program_version);
    //
    // Define arguments
    //
    TCLAP::SwitchArg verbose_switch("v","verbose", 
            "Produce verbose output", cmd, false);
    TCLAP::SwitchArg quiet_switch("q","quiet", 
            "Don't produce any output", cmd, false);
    TCLAP::ValueArg<std::string> trace_symbol("s",
            "symbol",
            "Symbol text to trace",
            false,
            "",
            "function names, variables, macros, enums"
            );
    cmd.add(trace_symbol);

    TCLAP::ValueArg<std::string> xref_tag_file("f",
            "xref-tag-file",
            "ccglue generated cross-reference file. Default value: ccglue.out",
            false,
            "ccglue.out",
            "XREF_TAG_FILE"
            );
    cmd.add(xref_tag_file);

    TCLAP::ValueArg<std::string> index_file("F",
            "index-file",
            "ccglue generated cross-reference index file. Default value: XREF_TAG_FILE.idx, (i.e., ccglue.out.idx)",
            false,
            "",
            "XREF_TAG_INDEX_FILE"
            );
    cmd.add(index_file);


    TCLAP::ValueArg<char> trace_direction("c", 
            "direction", 
            "trace direction: r (reverse/caller), or f (forward/callee)",
            true,
            'r',
            "r/f"
            );
    cmd.add(trace_direction);
    
    
    TCLAP::ValueArg<int> tree_depth_max("d",
            "tree-depth-max",
            "depth of recursion",
            false,
            3,
            "0,1,2,3, ..."
            );
    cmd.add(tree_depth_max);

    cmd.parse(argc, argv);
    opts->symbol = trace_symbol.getValue(); 
    opts->direction = trace_direction.getValue(); 
    opts->verbose = verbose_switch.getValue(); 
    opts->silent = quiet_switch.getValue(); 
    opts->index_file = index_file.getValue();
    opts->xref_tag_file = xref_tag_file.getValue();
    opts->max_tree_depth = tree_depth_max.getValue();

    if (opts->index_file.empty()) {
        opts->index_file = opts->xref_tag_file + ".idx";
    }
}

int ccglue_tracer_parse_command_line_options (int argc, char **argv,
        ccglue_tracer_opts* opts)
{
    /* Parse our arguments; every option seen by parse_opt will
       be reflected in arguments. */
    parse_options (argc, argv, opts);

}


