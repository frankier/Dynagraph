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


#include "voronoi/voronoi.h"
#include "fdp/fdp.h"
#include "shortspline/ObAvSplinerEngine.h"
#include "dynadag/DynaDAG.h"
#include "common/ColorByAge.h"
#include "createEngine.h"
#include "common/LabelPlacer.h"
#include "common/ShapeGenerator.h"

using namespace std;

namespace Dynagraph {

using DynaDAG::DynaDAGLayout;
using FDP::FDPLayout;

creators<GeneralLayout> creators<GeneralLayout>::the_creators;
creators<GeneralLayout>::creators()  {
	creators &me = *this;
	me["labels"] = EngineCreatorInstance<LabelPlacer<GeneralLayout> >::create;
	me["shapegen"] = EngineCreatorInstance<ShapeGenerator<GeneralLayout> >::create;
	me["colorbyage"] = EngineCreatorInstance<ColorByAge<GeneralLayout> >::create;
}
creators<DynaDAGLayout> creators<DynaDAGLayout>::the_creators;
creators<DynaDAGLayout>::creators()  {
	creators &me = *this;
	me["dynadag"] = EngineCreatorInstance<DynaDAG::DynaDAGServer>::create;
	me["labels"] = EngineCreatorInstance<LabelPlacer<DynaDAGLayout> >::create;
	me["shapegen"] = EngineCreatorInstance<ShapeGenerator<DynaDAGLayout> >::create;
}
creators<FDPLayout> creators<FDPLayout>::the_creators;
creators<FDPLayout>::creators() {
	creators &me = *this;
	me["fdp"] = EngineCreatorInstance<FDP::FDPServer>::create;
	me["voronoi"] = EngineCreatorInstance<Voronoi::VoronoiServer>::create;
	me["visspline"] = EngineCreatorInstance<ObAvSplinerEngine<FDPLayout> >::create;
	me["labels"] = EngineCreatorInstance<LabelPlacer<FDPLayout> >::create;
	me["shapegen"] = EngineCreatorInstance<ShapeGenerator<FDPLayout> >::create;
}

} // namespace Dynagraph
