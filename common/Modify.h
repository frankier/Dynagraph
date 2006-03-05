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

template<typename Layout>
inline void ModifyNode(ChangeQueue<Layout> &Q,typename Layout::Node *n,Update upd) {
	if(typename Layout::Node *n2 = Q.ModNode(n))
		igd<Update>(n2) |= upd;
}
template<typename Layout>
inline void ModifyEdge(ChangeQueue<Layout> &Q,typename Layout::Edge *e,Update upd) {
	if(typename Layout::Edge *e2 = Q.ModEdge(e))
		igd<Update>(e2) |= upd;
}
template<typename Layout>
inline Update &ModifyFlags(ChangeQueue<Layout> &Q) {
	return igd<Update>(Q.ModGraph());
}

} // namespace Dynagraph