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


class symbol_tree {
    public:
        symbol_tree (const tag *);
        ~symbol_tree ();
        bool add_child (symbol_tree *);
        void allow_visit (symbol_tree_visitor *);
        const tag& get_symbol_tag () const;
    private:
        std::list<symbol_tree *>   children;
        const tag                 *entry;
};

symbol_tree::~symbol_tree ()
{
    if (entry) {
        delete entry;
    }
    for (std::list<symbol_tree *>::iterator it = children.begin();
            it != children.end(); it++) {
        delete (*it);
    }
}


const tag& symbol_tree::get_symbol_tag () const
{
    return *entry;
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

symbol_tree::symbol_tree (const tag *tag_entry):
    entry(tag_entry)
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

    const std::list<tag_id_type_t>& node_list
                                    = atag->get_list_by_direction(direction);

    if (current_depth >= max_depth) {
        return;
    }
        
    for (std::list<tag_id_type_t>::const_iterator it=node_list.begin(); 
            it != node_list.end(); it++) {
        tag *child_tag = trace_tag_db->get_tag_by_id(*it);
        symbol_tree *s_child_tree = new symbol_tree(child_tag);
        s_tree->add_child(s_child_tree);
        do_trace(s_child_tree, child_tag, direction, current_depth+1, max_depth);
    }
}

symbol_tree* symbol_tracer::do_query (const symbol_trace_query& query)
{
    tag* atag = trace_tag_db->get_tag_line(query.expr);
    if (atag->get_symbol_name() != query.expr) {
        delete atag;
        return NULL;
    }
    symbol_tree *s_root_tree = new symbol_tree(atag);
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
    const tag& atag = s_tree->get_symbol_tag();
    for (int i=0; i < depth; i++) {
        std::cout << "\t";
    }
    for (int i=0; i < depth; i++) {
        std::cout << "<";
    }
    std::cout << atag.get_symbol_name() << "\n";
}


#endif
