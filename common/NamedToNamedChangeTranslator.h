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
	Graph2 *world2_,*current2_;
	ChangeActions actions_;
	NamedToNamedChangeTranslator(Graph2 *world2,Graph2 *current2,const ChangeActions &action = ChangeActions()) : world2_(world2),current2_(current2),actions_(action) {}
	virtual void Process(ChangeQueue<Graph1> &Q1) {
		ChangeQueue<Graph2> *pQ2=0;
		ChangeQueue<Graph2> &Q2 = nextQ_?*nextQ_:*(pQ2 = new ChangeQueue<Graph2>(world2_,current2_));
		actions_.ModifyGraph(Q1.ModGraph(),Q2.ModGraph());
		for(typename Graph1::node_iter ni = Q1.insN.nodes().begin(); ni!=Q1.insN.nodes().end(); ++ni) {
			std::pair<typename Graph2::Node *,bool> nb2 = Q2.whole->fetch_node(gd<Name>(*ni),true);
			//if(!nb2.second)
			//	throw NodeAlreadyExistedInconsistency(gd<Name>(*ni));
			typename Graph2::Node *n = Q2.InsNode(nb2.first).object;
			actions_.InsertNode(*ni,n);
		}
		for(typename Graph1::graphedge_iter ei = Q1.insE.edges().begin(); ei!=Q1.insE.edges().end(); ++ei) {
			std::pair<typename Graph2::Edge *,bool> eb2 = Q2.whole->fetch_edge(gd<Name>((*ei)->tail),gd<Name>((*ei)->head),gd<Name>(*ei),true);
			//if(!eb2.second)
			//	throw EdgeAlreadyExistedInconsistency(gd<Name>(*ei));
			typename Graph2::Edge *e = Q2.InsEdge(eb2.first).object;
			actions_.InsertEdge(*ei,e);
		}
		for(typename Graph1::node_iter ni = Q1.modN.nodes().begin(); ni!=Q1.modN.nodes().end(); ++ni) {
			std::pair<typename Graph2::Node *,bool> nb2 = Q2.whole->fetch_node(gd<Name>(*ni),false);
			if(nb2.second)
				throw NodeDoesntExistInconsistency(gd<Name>(*ni));
			typename Graph2::Node *n = Q2.ModNode(nb2.first).object;
			actions_.ModifyNode(*ni,n);
		}
		for(typename Graph1::graphedge_iter ei = Q1.modE.edges().begin(); ei!=Q1.modE.edges().end(); ++ei) {
			typename Graph2::Edge *e2 = Q2.whole->fetch_edge(gd<Name>(*ei));
			if(!e2)
				throw EdgeDoesntExistInconsistency(gd<Name>(*ei));
			typename Graph2::Edge *e = Q2.ModEdge(e2).object;
			actions_.ModifyEdge(*ei,e);
		}
		for(typename Graph1::node_iter ni = Q1.delN.nodes().begin(); ni!=Q1.delN.nodes().end(); ++ni) {
			std::pair<typename Graph2::Node *,bool> nb2 = Q2.whole->fetch_node(gd<Name>(*ni),false);
			if(nb2.second)
				throw NodeDoesntExistInconsistency(gd<Name>(*ni));
			typename Graph2::Node *n = Q2.DelNode(nb2.first).object;
			actions_.DeleteNode(*ni,n);
		}
		for(typename Graph1::graphedge_iter ei = Q1.delE.edges().begin(); ei!=Q1.delE.edges().end(); ++ei) {
			typename Graph2::Edge *e2 = Q2.whole->fetch_edge(gd<Name>(*ei));
			if(!e2)
				throw EdgeDoesntExistInconsistency(gd<Name>(*ei));
			typename Graph2::Edge *e = Q2.DelEdge(e2).object;
			actions_.DeleteEdge(*ei,e);
		}
		NextProcess(Q2);
		if(pQ2)
			delete pQ2;
	}
};

} // namespace Dynagraph

#endif // NamedToNamedChangeTranslator_h
