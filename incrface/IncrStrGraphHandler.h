/**********************************************************
*      This software is part of the graphviz toolset      *
*                http://www.graphviz.org/                 *
*                                                         *
*            Copyright (c) 1994-2005 AT&T Corp.           *
*                and is licensed under the                *
*            Common Public License, Version 1.0           *
*                      by AT&T Corp.                      *
*                                                         *
*        Information and Software Systems Research        *
*              AT&T Research, Florham Park NJ             *
*                                                         *
*               This version available from               *
*                   http://dynagraph.org                  *
**********************************************************/


#include "common/diff_strgraph.h"
#include 

namespace Dynagraph {

// NGraph is LGraph containing at least StrAttrs2
template<typename NGraph> 
struct IncrStrGraphHandler : IncrLangEvents {
	NGraph world_,current_;
	ChangeQueue<NGraph> Q_;
	ChangeProcessor<NGraph> *next_;
    int locks_;

    IncrStrGraphHandler() : world_(),current_(&world_),Q_(&world_,&current_),next_(0),locks_(0) {}

    bool maybe_go();

    // IncrLangEvents
	DString dinotype() { return "stringraph"; }
	void incr_ev_open_graph(DString graph,const StrAttrs &attrs);
	void incr_ev_close_graph();
	void incr_ev_mod_graph(const StrAttrs &attrs);
	void incr_ev_lock();
	void incr_ev_unlock();
	DString incr_ev_ins_node(DString name, const StrAttrs &attrs, bool merge);
	DString incr_ev_ins_edge(DString name, DString tailname, DString headname, const StrAttrs &attrs);
	void incr_ev_mod_node(DString name,const StrAttrs &attrs);
	void incr_ev_mod_edge(DString name,const StrAttrs &attrs);
	void incr_ev_del_node(DString name);
	void incr_ev_del_edge(DString name);
	void incr_ev_req_graph();
	void incr_ev_req_node(DString name);
	void incr_ev_req_edge(DString name);
    void incr_ev_load_strgraph(StrGraph *sg,bool merge, bool del);
};

// make changes immediately but only Process them when unlocked
template<typename NGraph>
bool IncrStrGraphHandler<NGraph>::maybe_go() {
    if(locks>0)
        return false;
	if(next_)
		next_->Process(Q_);
    return true;
}
template<typename NGraph>
void IncrStrGraphHandler<NGraph>::incr_ev_open_graph(DString graph,const StrAttrs &attrs) {
    gd<Name>(&world_) = graph;
    gd<StrAttrs>(&world_) = attrs;
    maybe_go();
}
template<typename NGraph>
void IncrStrGraphHandler<NGraph>::incr_ev_close_graph() {
}
template<typename NGraph>
void IncrStrGraphHandler<NGraph>::incr_ev_mod_graph(const StrAttrs &attrs) {
	for(StrAttrs::const_iterator ai = attrs.begin(); ai!=attrs.end(); ++ai)
		gd<StrAttrs2>(
    gd<StrAttrs>(g) = attrs;
    maybe_go();
}
template<typename NGraph>
void IncrStrGraphHandler<NGraph>::incr_ev_lock() {
    locks++;
}
template<typename NGraph>
void IncrStrGraphHandler<NGraph>::incr_ev_unlock() {
    --locks;
    maybe_go();
}
template<typename NGraph>
DString IncrStrGraphHandler<NGraph>::incr_ev_ins_node(DString name, const StrAttrs &attrs, bool merge) {
    if(name.empty())
        name = randomName('n');
    typename NGraph::Node *n = g->get_node(name);
    gd<StrAttrs>(n) += attrs;
    maybe_go();
    return name;
}
template<typename NGraph>
DString IncrStrGraphHandler<NGraph>::incr_ev_ins_edge(DString name, DString tailname, DString headname, const StrAttrs &attrs) {
    if(name.empty())
        name = randomName('e');
    typename NGraph::Edge *e = g->get_edge(tailname,headname,name);
    gd<StrAttrs>(e) += attrs;
    maybe_go();
    return name;
}
template<typename NGraph>
void IncrStrGraphHandler<NGraph>::incr_ev_mod_node(DString name,const StrAttrs &attrs) {
    typename NGraph::Node *n = g->ndict[name];
    if(!n)
        throw IncrNodeDoesNotExist(name);
    gd<StrAttrs>(n) += attrs;
    maybe_go();
    return true;
}
template<typename NGraph>
void IncrStrGraphHandler<NGraph>::incr_ev_mod_edge(DString name,const StrAttrs &attrs) {
    typename NGraph::Edge *e = g->edict[name];
    if(!e)
        throw IncrEdgeDoesNotExist(name);
    gd<StrAttrs>(e) += attrs;
    maybe_go();
    return true;
}
template<typename NGraph>
void IncrStrGraphHandler<NGraph>::incr_ev_del_node(DString name) {
    typename NGraph::Node *n = g->ndict[name];
    if(!n)
        throw AbGNodeUnknown(name);
    g->erase(n);
    maybe_go();
    return true;
}
template<typename NGraph>
void IncrStrGraphHandler<NGraph>::incr_ev_del_edge(DString name) {
    typename NGraph::Edge *e = g->edict[name];
    if(!e)
        throw AbGEdgeUnknown(name);
    g->erase(e);
    maybe_go();
    return true;
}
// NO
template<typename NGraph>
void IncrStrGraphHandler<NGraph>::incr_ev_req_graph() {
    return false;
}
template<typename NGraph>
void IncrStrGraphHandler<NGraph>::incr_ev_req_node(DString name) {
    return false;
}
template<typename NGraph>
void IncrStrGraphHandler<NGraph>::incr_ev_req_edge(DString name) {
    return false;
}
template<typename NGraph>
struct we_do_what_were_told {
    IncrStrGraphHandler<NGraph> *hand;
    we_do_what_were_told(IncrStrGraphHandler<NGraph> *hand) : hand(hand) {}
    void ins(StrGraph::Node *n) {
        hand->incr_ev_ins_node(gd<Name>(n),gd<StrAttrs>(n),true);
    }
    void ins(StrGraph::Edge *e) {
        hand->incr_ev_ins_edge(gd<Name>(e),gd<Name>(e->tail),gd<Name>(e->head),gd<StrAttrs>(e));
    }
    void mod(typename NGraph::Node *nn,StrGraph::Node *sn,StrAttrs *attrs) {
        hand->incr_ev_mod_node(gd<Name>(nn),*attrs);
    }
    void mod(typename NGraph::Edge *ne,StrGraph::Edge *se,StrAttrs *attrs) {
        hand->incr_ev_mod_edge(gd<Name>(ne),*attrs);
    }
    void del(typename NGraph::Node *n) {
        hand->incr_ev_del_node(gd<Name>(n));
    }
    void del(typename NGraph::Edge *e) {
        hand->incr_ev_del_edge(gd<Name>(e));
    }
};
template<typename NGraph>
void IncrStrGraphHandler<NGraph>::incr_ev_load_strgraph(StrGraph *sg,bool merge, bool del) {
    assert(merge && del);
    we_do_what_were_told<NGraph> react(this);
    incr_ev_lock();
    diff_strgraph(g,sg,react);
    incr_ev_unlock();
}

} // namespace Dynagraph
