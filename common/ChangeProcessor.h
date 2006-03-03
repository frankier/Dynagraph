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

#ifndef ChangeProcessor_h
#define ChangeProcessor_h

#include "Modify.h"

namespace Dynagraph {

template<typename Layout>
struct ChangeProcessor {
	typedef Layout LayoutType;
	Layout *const client, *const current;

	virtual void Process(ChangeQueue<Layout> &Q) = 0;
	ChangeProcessor(Layout *client,Layout *current) : client(client), current(current) {}
	virtual ~ChangeProcessor() {}
};
template<typename Layout>
struct CompoundChangeProcessor : ChangeProcessor<Layout> {
	typedef std::vector<ChangeProcessor<Layout>*> ServerV;
	ServerV actors;
	void Process(ChangeQueue<Layout> &Q);
	CompoundChangeProcessor(Layout *client,Layout *currentLayout) : ChangeProcessor<Layout>(client,currentLayout) {}
	~CompoundChangeProcessor();
};
template<typename Layout>
void CompoundChangeProcessor<Layout>::Process(ChangeQueue<Layout> &Q) {
	for(typename ServerV::iterator i = actors.begin(); i!=actors.end(); ++i)
		(*i)->Process(Q);
}
template<typename Layout>
CompoundChangeProcessor<Layout>::~CompoundChangeProcessor() {
	for(typename ServerV::iterator i = actors.begin(); i!=actors.end(); ++i)
		delete *i;
	actors.clear();
}

// simple server that just updates the current subgraph based on changes.
// this must only be done once, that's why individual layout servers can't be responsible.
template<typename Layout>
struct UpdateCurrentProcessor : ChangeProcessor<Layout> {
	UpdateCurrentProcessor(Layout *client,Layout *currentLayout) : ChangeProcessor<Layout>(client,currentLayout) {}
	void Process(ChangeQueue<Layout> &Q) {
		Q.UpdateCurrent();
	}
};

} // namespace Dynagraph

#endif //ChangeProcessor_h
