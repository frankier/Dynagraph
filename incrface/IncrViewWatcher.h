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

#ifndef IncrViewWatcher_h
#define IncrViewWatcher_h

#include "common/LayoutAttrs.h"
#include "common/ChangeQueue.h"

namespace Dynagraph {

template<typename Layout>
struct IncrViewWatcher {
	virtual void IncrHappened(ChangeQueue<Layout> &Q) = 0;
	virtual void IncrOpen(ChangeQueue<Layout> &Q) = 0;
	virtual void IncrClose(ChangeQueue<Layout> &Q) = 0;
	virtual void IncrNewNode(ChangeQueue<Layout> &Q,typename Layout::Node *n) = 0;
	virtual void IncrNewEdge(ChangeQueue<Layout> &Q,typename Layout::Edge *e) = 0;
};

} // namespace Dynagraph

#endif IncrViewWatcher_h
