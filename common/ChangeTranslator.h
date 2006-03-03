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

template<typename Layout1,typename Layout2>
struct ChangeTranslator {
	virtual void Translate(ChangeQueue<Layout1> &Q1,ChangeQueue<Layout2> &Q2) = 0;
	virtual ~ChangeTranslator() {}
};

template<typename OuterLayout,typename InnerLayout>
struct ChangeSubProcessor : ChangeProcessor<OuterLayout> {
	ChangeTranslator<OuterLayout,InnerLayout> *xlateIn;
	ChangeProcessor<InnerLayout> *processor;
	ChangeTranslator<InnerLayout,OuterLayout> *xlateOut;

	ChangeSubProcessor(OuterLayout *client,OuterLayout *current,
		ChangeTranslator<OuterLayout,InnerLayout> *xlateIn,
		ChangeProcessor<InnerLayout> *processor,
		ChangeTranslator<InnerLayout,OuterLayout> *xlateOut)
		: ChangeProcessor<OuterLayout>(client,current),xlateIn(xlateIn),processor(processor),xlateOut(xlateOut)
	{}

	void Process(ChangeQueue<OuterLayout> &Q) {
		ChangeQueue<InnerLayout> Q2;
		xlateIn->Translate(Q,Q2);
		processor->Process(Q2);
		xlateOut->Translate(Q2,Q);
		Q2.Okay(true);
	}
};

} // namespace Dynagraph

#endif //ChangeTranslator_h
