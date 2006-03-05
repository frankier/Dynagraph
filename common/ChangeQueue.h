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

#ifndef ChangeQueue_h
#define ChangeQueue_h

#include "LayoutAttrs.h"

namespace Dynagraph {

/*
	CHANGE QUEUE
*/
template<typename Graph>
struct ChangeQueue {
	// the client edits this supergraph of server's current layout
	// then calls the methods below to signal the changes in the subgraphs
	Graph * const client, * const current;
	Graph insN,modN,delN,
		insE,modE,delE;

	ChangeQueue(Graph *client,Graph *current) : client(client),current(current),
	insN(client),modN(client),delN(client),insE(client),modE(client),delE(client) {}
	ChangeQueue(ChangeQueue &copy) : client(copy.client),current(copy.current),
	insN(client),modN(client),delN(client),insE(client),modE(client),delE(client) {
		insN = copy.insN;
		modN = copy.modN;
		delN = copy.delN;
		insE = copy.insE;
		modE = copy.modE;
		delE = copy.delE;
	}
	typename Graph::Node *InsNode(typename Graph::Node *n) {
		if(current->find(n))
			throw InsertInserted();
		return insN.insert(n).first;
	}
	typename Graph::Edge *InsEdge(typename Graph::Edge *e) {
		if(current->find(e))
			throw InsertInserted();
		return insE.insert(e).first;
	}
	typename Graph::Node *ModNode(typename Graph::Node *n) {
		if(!insN.find(n) && !delN.find(n)) 
			return modN.insert(n).first;
		return 0;
	}
	typename Graph::Edge *ModEdge(typename Graph::Edge *e) {
		if(!insE.find(e) && !delE.find(e)) 
			return modE.insert(e).first;
		return 0;
	}
	Graph *ModGraph() {
		return &modN;
	}
	typename Graph::Node *DelNode(typename Graph::Node *n) {
		insN.erase(n);
		modN.erase(n);
		n = current->find(n); // remove edges that are currently inserted
		for(typename Graph::nodeedge_iter i = n->alledges().begin(); i!=n->alledges().end(); ++i)
			DelEdge(*i);
		return delN.insert(n).first;
	}

	typename Graph::Edge *DelEdge(typename Graph::Edge *e) {
		insE.erase(e);
		modE.erase(e);
		return delE.insert(e).first;
	}

	// called by server to update current subgraph based on current changes
	void UpdateCurrent();

	// called by client after server processing; clear subgraphs and maybe do deletions
	void Okay(bool doDelete = false);

	bool Empty() { return insN.nodes().empty()&&modN.nodes().empty()&&delN.nodes().empty()&&
		insE.nodes().empty()&&modE.nodes().empty()&&delE.nodes().empty(); }

	// copy
	ChangeQueue &operator=(ChangeQueue &Q);
	// accumulate
	ChangeQueue &operator+=(ChangeQueue &Q);

	// Exceptions

	// insertions must not already be inserted; modifications & deletions must already be inserted
	struct InsertInserted : DGException {
	  InsertInserted() : DGException("insertion of an already inserted object",true) {}
	};
	struct ModifyUninserted : DGException {
	  ModifyUninserted() : DGException("modify of an uninserted object",true) {}
	};
	struct DeleteUninserted : DGException {
	  DeleteUninserted() : DGException("deletion of an uninserted object",true) {}
	};
	struct EndnodesNotInserted : DGException {
	  EndnodesNotInserted() : DGException("insertion of edge without nodes",true) {}
	};
};
template<typename Graph>
void ChangeQueue<Graph>::UpdateCurrent() {
	typename Graph::node_iter ni;
	typename Graph::graphedge_iter ei;
	for(ni = insN.nodes().begin(); ni!=insN.nodes().end(); ++ni)
		if(!current->insert(*ni).second)
			throw InsertInserted();
	for(ei = insE.edges().begin(); ei!=insE.edges().end(); ++ei) {
		typename Graph::Node *t =(*ei)->tail,*h = (*ei)->head;
		if(!current->find(t) && !insN.find(t))
			throw EndnodesNotInserted();
		if(!current->find(h) && !insN.find(h))
			throw EndnodesNotInserted();
		if(!current->insert(*ei).second)
			throw InsertInserted();
	}
	for(ei = delE.edges().begin(); ei!=delE.edges().end(); ++ei)
		if(!current->erase(*ei))
			throw DeleteUninserted();
	for(ni = delN.nodes().begin(); ni!=delN.nodes().end(); ++ni)
		if(!current->erase(*ni))
			throw DeleteUninserted();
	for(ni = modN.nodes().begin(); ni!=modN.nodes().end(); ++ni)
		if(!current->find(*ni))
			throw ModifyUninserted();
	for(ei = modE.edges().begin(); ei!=modE.edges().end(); ++ei)
		if(!current->find(*ei))
			throw ModifyUninserted();
}
template<typename Graph>
void ChangeQueue<Graph>::Okay(bool doDelete) {
	insN.clear();
	insE.clear();
	modN.clear();
	modE.clear();
	if(doDelete) {
		for(typename Graph::graphedge_iter j = delE.edges().begin(); j!=delE.edges().end();) {
			typename Graph::Edge *e = *j++;
			check(client->erase_edge(e));
		}
        delE.clear(); // the nodes may still exist
		for(typename Graph::node_iter i = delN.nodes().begin(); i!=delN.nodes().end();) {
			typename Graph::Node *n = *i++;
			check(client->erase_node(n));
		}
	}
	else {
		delE.clear();
		delN.clear();
	}
    assert(Empty());
}
template<typename Graph>
ChangeQueue<Graph> &ChangeQueue<Graph>::operator=(ChangeQueue<Graph> &Q) {
	assert(client==Q.client);
	insN = Q.insN;
	modN = Q.modN;
	delN = Q.delN;
	insE = Q.insE;
	modE = Q.modE;
	delE = Q.delE;
	client->idat = Q.client->idat; 
	return *this;
}
template<typename Graph>
ChangeQueue<Graph> &ChangeQueue<Graph>::operator+=(ChangeQueue<Graph> &Q) {
	assert(client==Q.client);
	typename Graph::node_iter ni;
	typename Graph::graphedge_iter ei;
	for(ni = Q.insN.nodes().begin(); ni!=Q.insN.nodes().end(); ++ni)
		InsNode(*ni);
	for(ei = Q.insE.edges().begin(); ei!=Q.insE.edges().end(); ++ei)
		InsEdge(*ei);
	for(ni = Q.modN.nodes().begin(); ni!=Q.modN.nodes().end(); ++ni) 
		ModNode(*ni)->idat = (*ni)->idat;
	for(ei = Q.modE.edges().begin(); ei!=Q.modE.edges().end(); ++ei)
		ModEdge(*ei)->idat = (*ei)->idat;
	for(ni = Q.delN.nodes().begin(); ni!=Q.delN.nodes().end(); ++ni)
		DelNode(*ni);
	for(ei = Q.delE.edges().begin(); ei!=Q.delE.edges().end(); ++ei)
		DelEdge(*ei);
	return *this;
}

} // namespace Dynagraph

#endif // ChangeQueue_h
