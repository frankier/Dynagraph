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

namespace Dynagraph {

template<typename NGraph>
void emitChanges(std::ostream &os,ChangeQueue<NGraph> &Q,const char *view) {
	os << "lock graph " << view << std::endl;
	if(!igd<StrAttrChanges>(Q.ModGraph()).empty())
		os << "modify graph " << view << " " << changes(Q.ModGraph()) << std::endl;
	for(typename NGraph::graphedge_iter ei = Q.delE.edges().begin(); ei!=Q.delE.edges().end(); ++ei)
		os << "delete edge " << view << " " << mquote(gd<Name>(*ei).c_str()) << std::endl;
	for(typename NGraph::node_iter ni = Q.delN.nodes().begin(); ni!=Q.delN.nodes().end(); ++ni)
		os << "delete node " << view << " " << mquote(gd<Name>(*ni).c_str()) << std::endl;
	for(typename NGraph::node_iter ni = Q.insN.nodes().begin(); ni!=Q.insN.nodes().end(); ++ni)
		os << "insert node " << view << " " << mquote(gd<Name>(*ni).c_str())
			<< " " << gd<StrAttrs>(*ni) << std::endl;
	for(typename NGraph::graphedge_iter ei = Q.insE.edges().begin(); ei!=Q.insE.edges().end(); ++ei)
		os << "insert edge " << view << " " << mquote(gd<Name>(*ei))
			<< " " << mquote(gd<Name>((*ei)->tail))
			<< " " << mquote(gd<Name>((*ei)->head)) << " " << gd<StrAttrs>(*ei) << std::endl;
	for(typename NGraph::node_iter ni = Q.modN.nodes().begin(); ni!=Q.modN.nodes().end(); ++ni)
		if(!igd<StrAttrChanges>(*ni).empty())
			os << "modify node " << view << " " << mquote(gd<Name>(*ni).c_str())
				<< " " << changes(*ni) << std::endl;
	for(typename NGraph::graphedge_iter ei = Q.modE.edges().begin(); ei!=Q.modE.edges().end(); ++ei)
		if(!igd<StrAttrChanges>(*ei).empty())
			os << "modify edge " << view << " " << mquote(gd<Name>(*ei))
				<< " " << changes(*ei) << std::endl;
	os << "unlock graph " << view << std::endl;
}

} // namespace Dynagraph
