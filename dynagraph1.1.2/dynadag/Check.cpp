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


// Sanity checks.

#include "DynaDAG.h"

namespace Dynagraph {
namespace DynaDAG {

void FlexiRanks::Check() {
	iterator ri = begin(),
		next;
	if(ri==end())
		return;
	(next=ri)++;
	while(next!=end()) {
		index a = IndexOfIter(ri), b = IndexOfIter(next);
		assert(Xlator::Above(layout_,a,b));
		ri = next++;
	}
}
void Config::checkEdges(bool strict) {
	for(DDModel::graphedge_iter ei = model.edges().begin(); ei!=model.edges().end(); ++ei) {
		DDModel::Node *t = (*ei)->tail,
			*h = (*ei)->head;
		// edges must be path parts or node parts; edges must belong to one node only
		assert(gd<DDEdge>(*ei).amEdgePart() || gd<DDNode>(t).amNodePart() && gd<DDNode>(t).multi==gd<DDNode>(h).multi);
		Ranks::index tr = gd<DDNode>(t).rank,
			hr = gd<DDNode>(h).rank;
		if(strict) // all edges span one rank
			assert(ranking.Down(tr)==hr);
		else
			assert(Ranks::Xlator::Above(whole,tr,hr));
	}
	// nodes in paths belong to one path only
	for(DDModel::node_iter ni = model.nodes().begin(); ni!=model.nodes().end(); ++ni) {
		DDModel::Node *n = *ni;
		if(gd<DDNode>(n).amEdgePart()) {
			assert(n->ins().size()==1);
			assert(n->outs().size()==1);
			DDModel::Edge *e1 = *n->ins().begin(),
				*e2 = *n->outs().begin();
			assert(gd<DDEdge>(e1).path==gd<DDEdge>(e2).path);
		}
	}
	// view edges' paths connect the tops & bottoms of nodes
	for(DynaDAGLayout::graphedge_iter ei2 = current->edges().begin(); ei2!=current->edges().end(); ++ei2) {
		DDPath *path = DDp(*ei2);
		DDMultiNode *n1 = DDp((*ei2)->tail),
			*n2 = DDp((*ei2)->head);
		if(path && path->first)
			assert(path->first->tail==n1->bottom()&&path->last->head==n2->top()
				||path->first->tail==n2->bottom()&&path->last->head==n1->top());
	}
}
void Config::checkX() {
	for(Ranks::iterator ri = ranking.begin(); ri!=ranking.end(); ++ri) {
		Rank *r = *ri;
		for(NodeV::iterator ni = r->order.begin(); ni!=r->order.end(); ++ni)
			if(ni!=r->order.begin()) {
				double cx = gd<DDNode>(*ni).cur.x,
					px = gd<DDNode>(*(ni-1)).cur.x;
				assert(cx>=px);
			}
	}
}
void XSolver::checkLRConstraints() {
	bool missing = false;
	for(Config::Ranks::iterator ri = config.ranking.begin(); ri!=config.ranking.end(); ++ri) {
		Rank *r = *ri;
		for(NodeV::iterator ni = r->order.begin(); ni!=r->order.end(); ++ni)
			if(DDModel::Node *left = config.Left(*ni)) {
				DDCGraph::Node *l = gd<DDNode>(left).getXcon().n,
					*n = gd<DDNode>(*ni).getXcon().n;
				assert(l&&n);
				DDCGraph::Edge *e = cg.find_edge(l,n);
				if(!e) {
					report(r_error,"constraint missing between %x (%s %x) and %x (%s %x)\n",
						left,type(left),thing(left),*ni,type(*ni),thing(*ni));
					missing = true;
				}
				else
					assert(DDNS::NSd(e).minlen >= ROUND(xScale*config.UVSep(left,*ni)));
				/*
				// (hopeless)
				// don't allow extraneous constraints: only edge,stab, and L-R are good
				for(DDCGraph::edge_iter ei = n->ins().begin(); ei!=n->ins().end(); ++ei) {
					DDCGraph::Edge *e2 = *ei;
					assert(e2==e ||
						e2->tail == gd<DDNode>(*ni).getXcon().stab ||
						gd<ConstraintType>(e2->tail).why==ConstraintType::orderEdgeStraighten);
				}
				*/
			}
	}
	assert(!missing);
}
void XSolver::checkEdgeConstraints() {
	for(DDModel::graphedge_iter ei = config.model.edges().begin(); ei!=config.model.edges().end(); ++ei)
		if(gd<DDEdge>(*ei).amEdgePart()) {
			DDCGraph::Node *cn = gd<DDEdge>(*ei).cn;
			assert(cn);
			assert(cn->ins().size()==0);
			if(cn->outs().size()!=2) {
				report(r_error,"AARGH!  Why isn't node %p of %s %p constrained with\n",
					(*ei)->tail,gd<DDNode>((*ei)->tail).amEdgePart()?"path":"multinode",thing((*ei)->tail));
				report(r_error,"node %p of %s %p????\n",(*ei)->head,gd<DDNode>((*ei)->head).amEdgePart()?
					"path":"multinode",thing((*ei)->head));
				throw BadXConstraints();
			}
		}
}
/*
void DynaDAG::checkAll(ddview_t *view) {
	config.CheckRanks();
	int		r;

	for(r = view->config->low; r <= view->config->high; r++)
		dd_check_rank(view,r);
	dd_check_edges(view->layout);
}
void Config::CheckRanks() {
	for(int i = low; i<high; ++i) {
		Rank *r = GetRank(i);
		r->check(i);
	}
	for(RankV::iterator ri = ranking.begin(); ri!=ranking.end(); ++ri)
		(*ri)->check();
}
void Rank::check(int r) {
	Agnode_t	*ln,*rn,**list;
	int			i;
	rank_t		*rd;

	DDModel::Node *ln=0;
	for(NodeV::iterator ni = order.begin(); ni!=order.end(); ++ni) {
		assert(gd<DDNode>(*ni).inConfig);
		assert(gd<DDNode>(*ni).rank == r);
		dd_check_elts(*ni);
		if(ln) {
			assert(gd<DDNode>(ln).order + 1 == gd<DDNode>(*ni).order);
			assert(gd<DDNode>(ln).cur.x + BASE(view)->whole->separation.x <= dd_pos(rn).x);
		}
		ln = rn;
	}
	assert (i == rd->n);
}

void dd_check_containment(ddview_t *view, int r, Agnode_t *n, int must_be_in)
{
	Agnode_t	*rn;

	for(rn = dd_leftmost(view,r); rn; rn = dd_right(view,rn)) {
		if(must_be_in) { if(n == rn) break; }
		else assert (n != rn);
	}
	if(must_be_in) assert (n == rn);
}

ilbool dd_check_pathnode(ddview_t *view, Agnode_t *n)
{
	rank_t		*rd;
	int			i,r;

	i = dd_order(n);
	r = dd_rank(n);
	rd = dd_rankd(view,r);
	assert(rd->v[i] == n);
	return FALSE;
}

void dd_check_vnode_path(ddview_t *view, Agedge_t **vpath)
{
	int			i;
	Agedge_t	*e,*f;

	f = NILedge;
	for(i = 0; (e = vpath[i]); i++) {
		dd_check_pathnode(view,agtail(e));
		if(i > 0) assert(dd_is_a_vnode(agtail(e)));
		f = e;
	}
	dd_check_pathnode(view,aghead(f));
}

void dd_check_elts(ddview_t *view, Agnode_t *n)
{
	Agedge_t	*e,*f,*fst,*lst;

	if(dd_is_a_vnode(n)) return;
	for(e = agfstout(n); e; e = agnxtout(e)) {
		fst = dd_first_elt(e);
		lst = dd_last_elt(e);

		for(f = fst; f; f = agfstout(aghead(f))) {
			dd_check_pathnode(view,aghead(f));
			if(f == lst) break;
		}
	}
}

void dd_check_newranks(Agraph_t *g)
{
	Agnode_t	*n;
	Agedge_t	*e;

	for(n = agfstnode(g); n; n = agnxtnode(n)) {
		if(dd_is_a_vnode(n)) continue;
		for(e = agfstout(n); e; e = agnxtout(e)) {
			if(NOT(dd_constraint(e))) continue;
			assert (dd_newrank(dd_pathhead(e)) - dd_newrank(dd_pathtail(e)) >= 1);
		}
	}
}

static void check_mg(Agraph_t *g, Agraph_t *root)
{
	Agnode_t	*mn;
	Agedge_t	*me;

	for(mn = agfstnode(g); mn; mn = agnxtnode(mn)) {
		assert(mn->base.data);
		assert(agsubnode(root,mn,FALSE));
		for(me = agfstout(mn); me; me = agnxtout(me)) {
			assert(me->base.data);
			assert(agsubedge(root,me,FALSE));
		}
	}
}

void dd_check_model(ddview_t *view)
{
	Agraph_t	*root;

	root = BASE(view)->model.main;
	check_mg(root,root);
	check_mg(BASE(view)->model.v[IL_INS],root);
	check_mg(BASE(view)->model.e[IL_INS],root);
	check_mg(BASE(view)->model.v[IL_MOD],root);
	check_mg(BASE(view)->model.e[IL_MOD],root);
	check_mg(BASE(view)->model.v[IL_DEL],root);
	check_mg(BASE(view)->model.e[IL_DEL],root);
}

void dd_check_really_gone(Agraph_t *g, Agnode_t *n, ulong id)
{
	Agnode_t	*u;
	Agedge_t	*e;

	assert (agidnode(g,id,FALSE) == NILnode);

	for(u = agfstnode(g); u; u = agnxtnode(u)) {
		assert(u != n);
		for(e = agfstedge(u); e; e = agnxtedge(e,u))
			assert(e->node != n);
	}
}

void dd_check_vnodes(ddview_t *view)
{
	Agnode_t	*n;
	Agedge_t	*e;

	for(n = agfstnode(view->layout); n; n = agnxtnode(n)) {
		if(NOT(dd_is_a_vnode(n))) continue;
		e = agfstin(n);
		if(e == NILedge) abort();
		e = agfstout(n);
		if(e == NILedge) abort();
	}
}

static int CLcnt = 0;
void dd_check_links(ddview_t *view)
{
	Agraph_t	*model, *layout;
	Agnode_t	*mn, *ln;
	Agedge_t	*me, *le, *mme;
	ddpath_t	*path;

dd_check_model(view);
	model = BASE(view)->model.main;
	layout = view->layout;

	for(mn = agfstnode(model); mn; mn = agnxtnode(mn)) {
		ln = dd_rep(mn);
		if(ln == NILnode) continue;
		assert(dd_node(ln)->model == mn);

		for(me = agfstedge(mn); me; me = agnxtedge(me,mn)) {
			path = dd_pathrep(me);
			mme = path->model;
			if(mme == NILedge) continue;
			assert((mme == me) || (mme == AGOPP(me)));
		}
	}

	for(ln = agfstnode(layout); ln; ln = agnxtnode(ln)) {
		if(dd_is_a_vnode(ln) == FALSE) {
			mn = dd_node(ln)->model;
			assert(mn);
			assert(agsubnode(model,mn,FALSE) == mn);
			assert(ln == dd_rep(mn));
			for(le = agfstedge(ln); le; le = agnxtedge(le,ln)) {
				path = dd_edge(le)->path;
				me = path->model;
				assert(agsubedge(model,me,FALSE) == me);
			}
		}
		else {
			assert(agfstin(ln) != NILedge);
			assert(agfstout(ln) != NILedge);
		}
	}
CLcnt++;
}
*/

} // namespace DynaDAG
} // namespace Dynagraph

