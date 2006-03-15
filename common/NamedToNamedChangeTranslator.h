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

#ifndef NamedToNamedChangeTranslator_h
#define NamedToNamedChangeTranslator_h

#include "ChangeTranslator.h"

namespace Dynagraph {
/*
// 
struct NodeAlreadyExistedInconsistency : DGException2 {
	NodeAlreadyExistedInconsistency(DString name) : DGException2("Internal inconsistency: node already existed",name,true) {}
};
struct EdgeAlreadyExistedInconsistency : DGException2 {
	NodeAlreadyExistedInconsistency(DString name) : DGException2("Internal inconsistency: node already existed",name,true) {}
};
*/
struct NodeDoesntExistInconsistency : DGException2 {
	NodeDoesntExistInconsistency(DString name) : DGException2("Internal inconsistency: node doesn't exist",name,true) {}
};
struct EdgeDoesntExistInconsistency : DGException2 {
	EdgeDoesntExistInconsistency(DString name) : DGException2("Internal inconsistency: edge doesn't exist",name,true) {}
};
template<typename Graph1,typename Graph2,typename ChangeActions>
struct NamedToNamedChangeTranslator : ChangeTranslator<Graph1,Graph2> {
	ChangeActions actions_;
	NamedToNamedChangeTranslator(const ChangeActions &action) : actions_(action) {}
	virtual void Translate(ChangeQueue<Graph1> &Q1,ChangeQueue<Graph2> Q2) {
		actions_.ModifyGraph(Q1.ModGraph(),Q2.ModGraph());
		for(typename Graph1::node_iter ni = Q1.insN.nodes().begin(); ni!=Q1.insN.nodes().end(); ++ni) {
			std::pair<typename Graph2::Node *,bool> nb2 = Q2.client->fetch_node(gd<Name>(*ni),true);
			//if(!nb2.second)
			//	throw NodeAlreadyExistedInconsistency(gd<Name>(*ni));
			typename Graph2::Node *n = Q2.InsNode(nb2.first);
			actions_.InsertNode(Q2,*ni,n);
		}
		for(typename Graph1::graphedge_iter ei = Q1.insE.nodes().begin(); ei!=Q1.insE.nodes().end(); ++ei) {
			std::pair<typename Graph2::Edge *,bool> eb2 = Q2.client->fetch_edge(gd<Name>((*ei)->tail),gd<Name>((*ei)->head),gd<Name>(*ei),true);
			//if(!eb2.second)
			//	throw EdgeAlreadyExistedInconsistency(gd<Name>(*ei));
			typename Graph2::Edge *e = Q2.InsEdge(eb2.first);
			actions_.InsertNode(Q2,*ei,e);
		}
		for(typename Graph1::node_iter ni = Q1.modN.nodes().begin(); ni!=Q1.modN.nodes().end(); ++ni) {
			std::pair<typename Graph2::Node *,bool> nb2 = Q2.client->fetch_node(gd<Name>(*ni),false);
			if(nb2.second)
				throw NodeDoesntExistInconsistency(gd<Name>(*ni));
			typename Graph2::Node *n = Q2.ModNode(nb2.first);
			actions_.ModifyNode(Q2,*ni,n);
		}
		for(typename Graph1::graphedge_iter ei = Q1.modE.nodes().begin(); ei!=Q1.modE.nodes().end(); ++ei) {
			typename Graph2::Edge *e2 = Q2.client->fetch_edge(gd<Name>(*ei));
			if(!e2)
				throw EdgeDoesntExistInconsistency(gd<Name>(*ei));
			typename Graph2::Node *e = Q2.ModEdge(e2);
			actions_.ModifyEdge(Q2,e2,e);
		}
		for(typename Graph1::node_iter ni = Q1.delN.nodes().begin(); ni!=Q1.delN.nodes().end(); ++ni) {
			std::pair<typename Graph2::Node *,bool> nb2 = Q2.client->fetch_node(gd<Name>(*ni),false);
			if(nb2.second)
				throw NodeDoesntExistInconsistency(gd<Name>(*ni));
			typename Graph2::Node *n = Q2.DelNode(nb2.first);
			actions_.DeleteNode(Q2,*ni,n);
		}
		for(typename Graph1::graphedge_iter ei = Q1.delE.nodes().begin(); ei!=Q1.delE.nodes().end(); ++ei) {
			typename Graph2::Edge *e2 = Q2.client->fetch_edge(gd<Name>(*ei));
			if(!e2)
				throw EdgeDoesntExistInconsistency(gd<Name>(*ei));
			typename Graph2::Node *e = Q2.DelEdge(e2);
			actions_.DeleteEdge(Q2,e2,e);
		}
	}
};

} // namespace Dynagraph

#endif // NamedToNamedChangeTranslator_h
