#ifndef TRACER_UTILS_H
#define TRACER_UTILS_H

#include <list>
#include "sym_mgr.h"
#include "tagfilerdr.h"
#include "tag_db.h"

class symbol_tree;

class symbol_tree_visitor {
    public:
        virtual void process (symbol_tree *) = 0;
        void increase_depth() {depth++;}
        void decrease_depth() {depth--;}
    protected:
        int  depth;
};

struct tree_node {
    std::string sym_name;
    std::string sym_file;
    int         sym_line;

    tree_node():sym_line(0) {};
    tree_node (std::string& name, std::string& file, int line):
        sym_name(name), sym_file(file), sym_line(line){};
};

class symbol_tree {
    public:
        symbol_tree (const tree_node&);
        ~symbol_tree ();
        bool add_child (symbol_tree *);
        void allow_visit (symbol_tree_visitor *);
        const tree_node& get_node () const;
    private:
        std::list<symbol_tree *>   children;
        tree_node                  entry;
};

symbol_tree::~symbol_tree ()
{
    for (std::list<symbol_tree *>::iterator it = children.begin();
            it != children.end(); it++) {
        delete (*it);
    }
}


const tree_node& symbol_tree::get_node () const
{
    return entry;
}
        
void symbol_tree::allow_visit (symbol_tree_visitor *visitor)
{
    visitor->process(this);
    visitor->increase_depth();
    for (std::list<symbol_tree *>::iterator it = children.begin();
            it != children.end(); it++) {
        (*it)->allow_visit(visitor);
    }
    visitor->decrease_depth();
}

symbol_tree::symbol_tree (const tree_node& node):
    entry(node)
{
}

bool symbol_tree::add_child (symbol_tree *s_tree)
{
    children.push_back(s_tree);
    return true;
}


class symbol_trace_query {
    public:
        std::string             expr;
        int                     direction;
        int                     max_tree_depth;

        symbol_trace_query (std::string& query, int d, int max_d):
            expr(query), direction(d), max_tree_depth(max_d) {};
};


class symbol_tracer {
    public:
        symbol_tracer (tag_db *);
        symbol_tree* do_query (const symbol_trace_query&);
        void do_trace (symbol_tree* s_tree, const tag* atag,
                        int direction, int depth, int max_depth);
    private:
        tag_db    *trace_tag_db;

};


symbol_tracer::symbol_tracer (tag_db *t_db):
    trace_tag_db(t_db)
{
}

void symbol_tracer::do_trace (symbol_tree* s_tree, const tag* atag,
        int direction, int current_depth, int max_depth)
{
    debug(0) << "tracing symbol " << atag->get_symbol_name() << " direction: " <<
        direction << " depth: " << current_depth << "\n";

    const std::list<tag_xref_data>& node_list
                                    = atag->get_list_by_direction(direction);


    if (current_depth >= max_depth) {
        return;
    }
        
    for (std::list<tag_xref_data>::const_iterator it=node_list.begin(); 
            it != node_list.end(); it++) {
        tree_node   t_node;
    
        // leaking memory..
        const tag *child_tag = trace_tag_db->get_tag_by_id((*it).sym_id);
        t_node.sym_name = child_tag->get_symbol_name();
        t_node.sym_file = trace_tag_db->get_tag_by_id((*it).sym_file_id)->get_symbol_name();
        t_node.sym_line = (*it).sym_line_num;

        symbol_tree *s_child_tree = new symbol_tree(t_node);
        s_tree->add_child(s_child_tree);
        do_trace(s_child_tree, child_tag, direction, current_depth+1, max_depth);
    }
    // don't delete, we have it covered throuhg the cachcej
    // delete atag;

}

symbol_tree* symbol_tracer::do_query (const symbol_trace_query& query)
{
    tag* atag = trace_tag_db->get_tag_line(query.expr);
    if (atag->get_symbol_name() != query.expr) {
        delete atag;
        return NULL;
    }
    std::string unknown("");
    std::string a(atag->get_symbol_name());

    symbol_tree *s_root_tree = new symbol_tree(tree_node(a, unknown, 0));
    do_trace(s_root_tree, atag, query.direction, 0, query.max_tree_depth);
    
    return s_root_tree;
}

class symbol_tree_visitor_pprinter: public symbol_tree_visitor {
    public:
        void process (symbol_tree *);
        symbol_tree_visitor_pprinter():symbol_tree_visitor(){};
};

void symbol_tree_visitor_pprinter::process (symbol_tree *s_tree)
{
    const tree_node& atree_node = s_tree->get_node();
    for (int i=0; i < depth; i++) {
        std::cout << "\t";
    }
    for (int i=0; i < depth; i++) {
        std::cout << "<";
    }
    std::cout << atree_node.sym_name << "(" << atree_node.sym_file << "," << atree_node.sym_line << ")\n";
}


#endif
