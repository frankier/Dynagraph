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

#ifndef ChangeTranslator_h
#define ChangeTranslator_h

namespace Dynagraph {

template<typename Graph1,typename Graph2>
struct ChangeTranslator {
	virtual void Translate(ChangeQueue<Graph1> &Q1,ChangeQueue<Graph2> &Q2) = 0;
	virtual ~ChangeTranslator() {}
};

template<typename OuterGraph,typename InnerGraph>
struct ChangeSubProcessor : ChangeProcessor<OuterGraph> {
	ChangeTranslator<OuterGraph,InnerGraph> *xlateIn;
	ChangeProcessor<InnerGraph> *processor;
	ChangeTranslator<InnerGraph,OuterGraph> *xlateOut;

	ChangeSubProcessor(OuterGraph *client,OuterGraph *current,
		ChangeTranslator<OuterGraph,InnerGraph> *xlateIn,
		ChangeProcessor<InnerGraph> *processor,
		ChangeTranslator<InnerGraph,OuterGraph> *xlateOut)
		: ChangeProcessor<OuterGraph>(client,current),xlateIn(xlateIn),processor(processor),xlateOut(xlateOut)
	{}

	void Process(ChangeQueue<OuterGraph> &Q) {
		ChangeQueue<InnerGraph> Q2;
		xlateIn->Translate(Q,Q2);
		processor->Process(Q2);
		xlateOut->Translate(Q2,Q);
		Q2.Okay(true);
	}
};

} // namespace Dynagraph

#endif //ChangeTranslator_h
