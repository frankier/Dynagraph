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

#ifndef InternalTranslator_h
#define InternalTranslator_h

#include "ChangeTranslator.h"

namespace Dynagraph {
template<typename Graph,typename ChangeActions>
struct InternalTranslator : LinkedChangeProcessor<Graph,Graph> {
	ChangeActions actions_;
	InternalTranslator(const ChangeActions &action) : actions_(action) {}
	virtual void Process(ChangeQueue<Graph> &Q) {
		actions_.ModifyGraph(Q.ModGraph());
		for(typename Graph::node_iter ni = Q.insN.nodes().begin(); ni!=Q.insN.nodes().end(); ++ni)
			actions_.InsertNode(Q,*ni);
		for(typename Graph::graphedge_iter ei = Q.insE.nodes().begin(); ei!=Q.insE.nodes().end(); ++ei)
			actions_.InsertNode(Q,*ei);
		for(typename Graph::node_iter ni = Q.modN.nodes().begin(); ni!=Q.modN.nodes().end(); ++ni)
			actions_.ModifyNode(Q,*ni);
		for(typename Graph::graphedge_iter ei = Q.modE.nodes().begin(); ei!=Q.modE.nodes().end(); ++ei)
			actions_.ModifyEdge(Q,*ei);
		for(typename Graph::node_iter ni = Q.delN.nodes().begin(); ni!=Q.delN.nodes().end(); ++ni)
			actions_.DeleteNode(Q,*ni);
		for(typename Graph::graphedge_iter ei = Q.delE.nodes().begin(); ei!=Q.delE.nodes().end(); ++ei)
			actions_.DeleteEdge(Q,*ei);
		NextProcess(Q);
	}
};

} // namespace Dynagraph

#endif // InternalTranslator_h
