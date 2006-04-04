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

#include "dynadag/DynaDAGLayout.h"
#include "fdp/FDPLayout.h"
#include "GeneralLayout.h"
#include "common/FindChangeRect.h"
#include "common/breakList.h"

namespace Dynagraph {

template<typename Graph>
struct EngineCreator {
	typedef LinkedChangeProcessor<Graph>* (*create_fn)(Graph *cli,Graph *curr);
};
template<typename Engine>
struct EngineCreatorInstance {
	typedef typename Engine::GraphType Graph;
	static LinkedChangeProcessor<Graph>* create(Graph *whole,Graph *current) {
		return new Engine(whole,current);
	}
};

template<typename Graph>
struct creators;

template<>
struct creators<GeneralLayout> : std::map<DString,EngineCreator<GeneralLayout>::create_fn> {
	creators();
	static creators the_creators;
};

template<>
struct creators<DynaDAG::DynaDAGLayout> : std::map<DString,EngineCreator<DynaDAG::DynaDAGLayout>::create_fn> {
	creators();
	static creators the_creators;
};

template<>
struct creators<FDP::FDPLayout> : std::map<DString,EngineCreator<FDP::FDPLayout>::create_fn> {
	creators();
	static creators the_creators;
};
template<typename Graph>
struct EnginePair : std::pair<LinkedChangeProcessor<Graph>*,LinkedChangeProcessor<Graph>*> {
	EnginePair() {}
	EnginePair(LinkedChangeProcessor<Graph> *first,LinkedChangeProcessor<Graph> *second) {
		this->first = first;
		this->second = second;
	}
};

template<typename Graph>
EnginePair<Graph> createEngine(DString engines,Graph *whole,Graph *current) {
	if(engines.empty())
		throw DGException("engine(s) were not specified on call to createLayoutEngine");

	FCRData<Graph> *fcrdata = new FCRData<Graph>(whole);
    FCRBefore<Graph> *fcrbefore = new FCRBefore<Graph>(fcrdata);
    FCRAfter<Graph> *fcrafter = new FCRAfter<Graph>(fcrdata);
	LinkedChangeProcessor<Graph> *first,*now,*last;
	first = last = fcrbefore;
	last->next_ = now = new UpdateCurrentProcessor<Graph>(whole,current);
	last = now;
    std::vector<DString> engs;
    breakList(engines,engs);
	creators<Graph> &the_creators = creators<Graph>::the_creators;
    for(std::vector<DString>::iterator ei = engs.begin(); ei!=engs.end(); ++ei) {
		typename EngineCreator<Graph>::create_fn crea = the_creators[*ei];
		if(!crea) {
			/*
			std::cout << the_creators.size() << " creators:" << std::endl;
			for(typename creators<Graph>::iterator ci = the_creators.begin(); ci!=the_creators.end(); ++ci)
				std::cout << reinterpret_cast<int>(ci->first.c_str()) << " " << ci->first << " -> " << ci->second << std::endl;
			*/
			delete first;
			throw DGException2("engine name not known or not appropriate for graph type",*ei);
		}
		last->next_ = now = crea(whole,current);
		last = now;
	}
	last->next_ = now = fcrafter;
	last = now;

	return EnginePair<Graph>(first,last);
}

} // namespace Dynagraph
