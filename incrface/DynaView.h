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



#include "common/LayoutAttrs.h"
#include "common/ChangeQueue.h"
#include "common/ChangeProcessor.h"
#include "common/stringsIn.h"
#include "common/stringsOut.h"
#include "common/emitGraph.h"
//#include "common/engine_replacement.h"
#include "createLayoutServer.h"
#include "common/randomName.h"
#include "common/Transform.h"
#include "DinoMachine.h"
#include "IncrViewWatcher.h"
#include "incrxep.h"

namespace Dynagraph {

// basic stuff to handle an engine/layout pair with named nodes & edges.
// applications (e.g. the dynagraph executable & dynagraph COM component)
// derive from this and override the Incr...() methods to share a basic
// understanding of how to deal with names, how to create/replace the layout server, etc.

template<typename Layout>
struct DynaView : IncrLangEvents {
	Layout layout, // everything the client has created
        current, // what's currently being controlled by engine
        old; // when switching engines, the stuff that was already here
	ChangeQueue<Layout> Q;
	ChangeProcessor<Layout> *dgserver;
	int locks;
    Transform *m_transform;
    bool m_useDotDefaults;
	bool m_replacementFlag; // whether we're in the middle of engine replacement
	IncrViewWatcher<Layout> *watcher;

	DynaView(Name name = Name(),Transform *transform=0, bool useDotDefaults=false);
	virtual ~DynaView();
    // create or replace engine based on "engines" attribute of layout
    void createServer();
    // complete an engine replacement by changing the insertions into new engine into modifies
    // (call after dgserver->Process but before dealing with Q
    void completeReplacement();
	std::pair<typename Layout::Node*,bool> getNode(DString name,bool create=false) { // vc++ gave me erroneous errors when i outalined
		if(name.empty())
			name = randomName('n');
		return layout.fetch_node(name,create);
		/*
		typename Layout::Node *n = nodes[name];
		if(n)
			return std::make_pair(n,false);
		if(!create)
			return std::make_pair<typename Layout::Node*>(0,false);
		n = layout.create_node();
		gd<Name>(n) = name;
		nodes[name] = n;
		return std::make_pair(n,true);
		*/
	}
	std::pair<typename Layout::Edge*,bool> getEdge(DString id,typename Layout::Node *t,typename Layout::Node *h,bool create);
	std::pair<typename Layout::Edge*,bool> getEdge(DString id,DString tail,DString head,bool create);
	typename Layout::Edge *getEdge(DString id,DString tail,DString head);
	typename Layout::Edge *getEdge(DString id);
	void rename(DString newName);
	void rename(typename Layout::Node *n,DString newName);
	void rename(typename Layout::Edge *e,DString newName);
	void forget(typename Layout::Node *n);
	void forget(typename Layout::Edge *e);
	// important to destroy edges because graph representation won't allow
	// you to draw two edges t->h
	void destroy(typename Layout::Node *n);
	void destroy(typename Layout::Edge *e);

    bool maybe_go();
    Update open_layout(const StrAttrs &attrs);
	
    // IncrLangEvents
	DString dinotype() { return "layout"; }
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

template<typename Layout>
DynaView<Layout>::DynaView(Name name,Transform *transform, bool useDotDefaults) :
	current(&layout),
    old(&layout),
	Q(&layout,&current),
	dgserver(0),
	locks(0),
    m_transform(transform),
    m_useDotDefaults(useDotDefaults),
    m_replacementFlag(false),
	watcher(0)
{
	if(name.empty())
		name = randomName('v');
	gd<Name>(&layout) = name;
}
template<typename Layout>
DynaView<Layout>::~DynaView() {
	if(dgserver)
		delete dgserver;
}
template<typename Layout>
void DynaView<Layout>::createServer() {
	if(dgserver) {
		delete dgserver;
		dgserver = 0;
	}
	dgserver = createLayoutServer<Layout>(&layout,&current);
}
template<typename Layout>
void DynaView<Layout>::completeReplacement() {
	//post_engine_replacement(Q,old);
}
template<typename Layout>
std::pair<typename Layout::Edge*,bool> DynaView<Layout>::getEdge(DString name,typename Layout::Node *t,typename Layout::Node *h,bool create) {
	if(name.empty())
		name = randomName('e');
	return layout.fetch_edge(t,h,name,create);
}
template<typename Layout>
std::pair<typename Layout::Edge*,bool> DynaView<Layout>::getEdge(DString name,DString tail,DString head,bool create) {
	return layout.fetch_edge(tail,head,name,create);
}
template<typename Layout>
typename Layout::Edge *DynaView<Layout>::getEdge(DString name,DString tail,DString head) {
	return getEdge(name,tail,head,false).first;
}
template<typename Layout>
typename Layout::Edge *DynaView<Layout>::getEdge(DString name) {
	return layout.fetch_edge(name);
}
template<typename Layout>
void DynaView<Layout>::rename(DString newName) {
    DinoMachine::Node *n = g_dinoMachine.ndict[gd<Name>(&layout)];
    assert(n);
    g_dinoMachine.rename(n,newName);
    gd<Name>(&layout) = newName;
}
template<typename Layout>
void DynaView<Layout>::rename(typename Layout::Node *n,DString newName) {
	layout.rename(n,newName);
}
template<typename Layout>
void DynaView<Layout>::rename(typename Layout::Edge *e,DString newName) {
	layout.rename(e,newName);
}
template<typename Layout>
void DynaView<Layout>::forget(typename Layout::Node *n) {
	layout.forget(n);
}
template<typename Layout>
void DynaView<Layout>::forget(typename Layout::Edge *e) {
	layout.forget(e);
}
template<typename Layout>
void DynaView<Layout>::destroy(typename Layout::Node *n) {
	layout.erase(n);
}
template<typename Layout>
void DynaView<Layout>::destroy(typename Layout::Edge *e) {
	layout.erase(e);
}

// incr_ev
template<typename Layout>
bool DynaView<Layout>::maybe_go() {
    if(locks>0)
        return false;
	bool ch = false;
	while(!Q.Empty() || igd<Update>(Q.ModGraph()).flags || igd<StrAttrChanges>(Q.ModGraph()).size()) {
        dgserver->Process(Q);
        if(m_replacementFlag) {
            completeReplacement();
            m_replacementFlag = false;
        }
        stringsOut(m_transform,Q);
        for(typename Layout::graphedge_iter ei = Q.delE.edges().begin(); ei!=Q.delE.edges().end(); ++ei)
            forget(*ei);
        for(typename Layout::node_iter ni = Q.delN.nodes().begin(); ni!=Q.delN.nodes().end(); ++ni)
            forget(*ni);
        for(typename Layout::graphedge_iter ei = Q.unbornE.edges().begin(); ei!=Q.unbornE.edges().end(); ++ei)
            forget(*ei);
        for(typename Layout::node_iter ni = Q.unbornN.nodes().begin(); ni!=Q.unbornN.nodes().end(); ++ni)
            forget(*ni);
		if(watcher)
			watcher->IncrHappened(Q); // must clear Q but the events might cause more changes
		else
			Q.Okay(true);
		ch = true;
    }
	if(ch)
		g_dinoMachine.changed(gd<Name>(&layout));
    return true;
}
template<typename Layout>
Update DynaView<Layout>::open_layout(const StrAttrs &attrs) {
    bool create = false;
    if(!dgserver)
        create = true;
    else {
        StrAttrs::const_iterator ai = attrs.find("engines");
        if(ai!=attrs.end())
            if(ai->second!=gd<StrAttrs>(&current)["engines"])
                create = true;
    }
	Update ret = stringsIn(m_transform,m_useDotDefaults,Q.ModGraph(),attrs,false);
    if(create)
        createServer();
    return ret;
}
template<typename Layout>
void DynaView<Layout>::incr_ev_open_graph(DString graph,const StrAttrs &attrs) {
    if(dgserver)
        throw IncrReopenXep(graph);
    open_layout(attrs);
    incr_set_handler(gd<Name>(&layout) = graph,this);
	if(watcher)
		watcher->IncrOpen(Q);
}
template<typename Layout>
void DynaView<Layout>::incr_ev_close_graph() {
	if(watcher)
		watcher->IncrClose(Q);
}
template<typename Layout>
void DynaView<Layout>::incr_ev_mod_graph(const StrAttrs &attrs) {
    ModifyFlags(Q) |= open_layout(attrs);
    maybe_go();
}
template<typename Layout>
void DynaView<Layout>::incr_ev_lock() {
    locks++;
}
template<typename Layout>
void DynaView<Layout>::incr_ev_unlock() {
    --locks;
    maybe_go();
}
template<typename Layout>
DString DynaView<Layout>::incr_ev_ins_node(DString name, const StrAttrs &attrs, bool merge) {
    DString rename = name;
    std::pair<typename Layout::Node *,bool> nb = getNode(name,true);
    if(!nb.second&&!merge) {
        // name already was used, so make an anónimo and label with the name
        nb.second = true;
        nb.first = getNode(0,true).first;
        gd<StrAttrs>(nb.first)["label"] = name;
        rename = gd<Name>(nb.first);
    }
	// all this nonsense because we allow reinsert to modify (new behavior will reject reinsert)
	bool isInsert = nb.second 
		|| Q.delN.find(nb.first) 
		|| Q.unbornN.find(nb.first);
	typename ChangeQueue<Layout>::NodeResult result = isInsert
		?Q.InsNode(nb.first)
		:Q.ModNode(nb.first);
    Update upd = stringsIn<Layout>(m_transform,result.object,attrs,true);
	if(isInsert) {
		if(watcher)
	        watcher->IncrNewNode(Q,result.object);
	}
	else 
        ModifyNode(Q,result.object,upd);
    maybe_go();
    return rename;
}
template<typename Layout>
DString DynaView<Layout>::incr_ev_ins_edge(DString name, DString tailname, DString headname, const StrAttrs &attrs) {
    std::pair<typename Layout::Edge*,bool> eb = getEdge(name,tailname,headname,true);
	if(!eb.second&&gd<Name>(eb.first)!=name)
		throw DGParallelEdgesNotSupported(name);
	bool isInsert = eb.second 
		|| Q.delE.find(eb.first)
		|| Q.unbornE.find(eb.first);
	typename ChangeQueue<Layout>::EdgeResult result = isInsert
		?Q.InsEdge(eb.first)
		:Q.ModEdge(eb.first);
    Update upd = stringsIn<Layout>(m_transform,result.object,attrs,true);
	if(isInsert) {
		if(watcher)
	        watcher->IncrNewEdge(Q,result.object);
	}
    else
        ModifyEdge(Q,result.object,upd);
    maybe_go();
    return gd<Name>(eb.first);
}
template<typename Layout>
void DynaView<Layout>::incr_ev_mod_node(DString name,const StrAttrs &attrs) {
    typename Layout::Node *n = getNode(name).first;
    if(!n)
        throw DGNodeDoesNotExist(name);
	typename ChangeQueue<Layout>::NodeResult result = Q.ModNode(n);
    ModifyNode(Q,result.object,stringsIn<Layout>(m_transform,result.object,attrs,false));
    maybe_go();
}
template<typename Layout>
void DynaView<Layout>::incr_ev_mod_edge(DString name,const StrAttrs &attrs) {
    typename Layout::Edge *e = getEdge(name);
    if(!e)
        throw DGEdgeDoesNotExist(name);
	typename ChangeQueue<Layout>::EdgeResult result = Q.ModEdge(e);
    ModifyEdge(Q,result.object,stringsIn<Layout>(m_transform,result.object,attrs,false));
    maybe_go();
}
template<typename Layout>
void DynaView<Layout>::incr_ev_del_node(DString name) {
	typename Layout::Node *n = getNode(name).first;
    if(!n)
        throw DGNodeDoesNotExist(name);
    Q.DelNode(n);
    maybe_go();
}
template<typename Layout>
void DynaView<Layout>::incr_ev_del_edge(DString name) {
    typename Layout::Edge *e = getEdge(name);
    if(!e)
        throw DGEdgeDoesNotExist(name);
    Q.DelEdge(e);
    maybe_go();
}
template<typename Layout>
void DynaView<Layout>::incr_ev_req_graph() {
	if(watcher)
		watcher->FulfilGraph(&layout);
}
template<typename Layout>
void DynaView<Layout>::incr_ev_req_node(DString name) {
    typename Layout::Node *n = getNode(name).first;
    if(!n)
        throw DGNodeDoesNotExist(name);
	if(watcher)
		watcher->FulfilNode(n);
}
template<typename Layout>
void DynaView<Layout>::incr_ev_req_edge(DString name) {
    typename Layout::Edge *e = getEdge(name);
    if(!e)
        throw DGEdgeDoesNotExist(name);
	if(watcher)
		watcher->FulfilEdge(e);
}
template<typename Layout>
void DynaView<Layout>::incr_ev_load_strgraph(StrGraph *sg,bool merge, bool del) {
    typedef std::map<DString,StrGraph::Node*> strnode_dict;
    incr_ev_lock();
    if(merge && del) { // find deletions
        strnode_dict nd;
        for(StrGraph::node_iter sni = sg->nodes().begin(); sni!=sg->nodes().end(); ++sni)
            nd[gd<Name>(*sni)] = *sni;
        for(typename Layout::node_iter ni = current.nodes().begin(); ni!=current.nodes().end(); ++ni)
            if(nd.find(gd<Name>(*ni))==nd.end()) {
                for(typename Layout::nodeedge_iter ei = (*ni)->alledges().begin(); ei!=(*ni)->alledges().end(); ++ei)
                    incr_ev_del_edge(gd<Name>(*ei));
                incr_ev_del_node(gd<Name>(*ni));
            }
        for(typename Layout::graphedge_iter ei = current.edges().begin(); ei!=current.edges().end(); ++ei) {
            // find edges by head and tail because prob has no name.
            StrGraph::Node *t=0,*h=0;
            strnode_dict::iterator di = nd.find(gd<Name>((*ei)->tail));
            if(di!=nd.end()) {
                t = di->second;
                di = nd.find(gd<Name>((*ei)->head));
                if(di!=nd.end()) {
                    h = di->second;
                    if(!sg->find_edge(t,h))
                        incr_ev_del_edge(gd<Name>(*ei));
                }
            }
            // if tail or head missing we've already called DelNode thus don't need to call DelEdge
        }
    }
    { // find insertions & modifications
        std::map<DString,DString> nd; // an override dictionary, only if !merge
        for(StrGraph::node_iter ni = sg->nodes().begin(); ni!=sg->nodes().end(); ++ni) {
            DString rename = incr_ev_ins_node(gd<Name>(*ni),gd<StrAttrs>(*ni),merge);
            if(!merge)
                nd[gd<Name>(*ni)] = rename;
        }
        for(StrGraph::graphedge_iter ei = sg->edges().begin(); ei!=sg->edges().end(); ++ei) {
            DString id = gd<Name>(*ei),
                t = gd<Name>((*ei)->tail),
                h = gd<Name>((*ei)->head);
            if(poorEdgeName(id.c_str()))
                id = 0;
            if(!merge) {
                t = nd[t];
                h = nd[h];
            }
			if(typename Layout::Edge *oldE = getEdge(id,t,h))
				incr_ev_mod_edge(gd<Name>(oldE),gd<StrAttrs>(*ei));
			else
				incr_ev_ins_edge(id,t,h,gd<StrAttrs>(*ei));
        }
    }
    incr_ev_unlock();
}

} // namespace Dynagraph
