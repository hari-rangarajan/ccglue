#include "tracer.h"
#include "tracer_utils.h"
#include "tracer_options.h"
#include "tclap/CmdLine.h"


int main(int argc, char **argv) 
{
    ccglue_tracer_opts  opts;

    try {
        /* Parse our arguments; every option seen by parse_opt will
           be reflected in arguments. */
        ccglue_tracer_parse_command_line_options(argc, argv, &opts);
        tag_db  db(opts.xref_tag_file, opts.index_file);
        symbol_tracer   s_tracer(&db);
        symbol_tree     *s_tree;
        symbol_trace_query  qry(opts.symbol, opts.direction, opts.max_tree_depth);
        symbol_tree_visitor_pprinter  pprinter;

        s_tree = s_tracer.do_query(qry);
        if (s_tree != NULL) {
            s_tree->allow_visit(&pprinter);
            delete s_tree;
        } else {
            std::cout << "Symbol not found\n";
        }
    }
    catch ( TCLAP::ArgException& e ) { 
        std::cout << "ERROR: " << e.error() << " " << e.argId() << std::endl; 
    }
    catch (std::exception& e) {
        std::cerr << e.what() << "\n";
    }

    return 0;
}
