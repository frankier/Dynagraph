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

// a ChangeProcessor client should send changes on without expecting any response
// the server is allowed to change the Q and is expected to eventually realize the changes 
template<typename Graph>
struct ChangeProcessor {
	typedef Graph GraphType;
	virtual void Process(ChangeQueue<Graph> &Q) = 0; 
	virtual ~ChangeProcessor() {}
};
template<typename Graph>
struct LinkedChangeProcessor : ChangeProcessor<Graph> {
	LinkedChangeProcessor<Graph> *next_;
	LinkedChangeProcessor(LinkedChangeProcessor<Graph> *next=0) : next_(next) {}
	virtual ~LinkedChangeProcessor() {
		delete next_;
	}
	void NextProcess(ChangeQueue<Graph> &Q) {
		if(next_)
			next_->Process(Q);
	}
};
template<typename Graph1,typename Graph2>
struct ChangeTranslator : LinkedChangeProcessor<Graph1> {
	// oddity: intentional override of field because linked-list one must stay null
	ChangeProcessor<Graph2> *next_;
	ChangeQueue<Graph2> *nextQ_;
	ChangeTranslator(ChangeProcessor<Graph2> *next=0,ChangeQueue<Graph2> *nextQ=0) : next_(next),nextQ_(nextQ) {}
	virtual ~ChangeTranslator() {
		delete next_;
	}
	void NextProcess(ChangeQueue<Graph2> &Q) {
		if(next_)
			next_->Process(Q);
	}
};

// simple server that just updates the current subgraph based on changes.
// this must be done only once, that's why individual layout servers can't be responsible.
template<typename Graph>
struct UpdateCurrentProcessor : LinkedChangeProcessor<Graph> {
	UpdateCurrentProcessor(Graph*,Graph*) {}
	void Process(ChangeQueue<Graph> &Q) {
		Q.UpdateCurrent();
		NextProcess(Q);
	}
};

} // namespace Dynagraph

#endif //ChangeProcessor_h
