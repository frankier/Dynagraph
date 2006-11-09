/**********************************************************
*            This software is part of Dynagraph           *
*                http://www.dynagraph.org/                *
*                                                         *
*            Copyright (c) 2006 Gordon Woodhull           *
*        Portions Copyright (c) 1994-2005 AT&T Corp.      *
*  Licensed under the Common Public License, Version 1.0  *
*                                                         *
*      Dynagraph originates in the Graphviz toolset       *
*                   http://graphviz.org                   *
*        Information and Software Systems Research        *
*              AT&T Research, Florham Park NJ             *
*                       Many thanks.                      *
**********************************************************/

#ifndef UpdateCurrentConfigurator_h
#define UpdateCurrentConfigurator_h

#include "Configurator.h"
#include "SimpleEngines.h"

namespace Dynagraph {

struct UpdateCurrentConfigurator {
	template<typename Configurators,typename Layout,typename SourceLayout> 
	static bool config(DString name,const StrAttrs &attrs,ChangingGraph<Layout> *world,EnginePair<Layout> engines, SourceLayout *source) {
		engines.Prepend(new UpdateCurrentProcessor<Layout>(world));
		return configureLayout<Configurators>(name,attrs,world,engines,source);
	}
};

} // namespace Dynagraph

#endif //UpdateCurrentConfigurator_h
