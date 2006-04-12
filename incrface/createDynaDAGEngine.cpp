#include "common/createEngine.h"
#include "common/LabelPlacer.h"
#include "common/ShapeGenerator.h"
#include "dynadag/DynaDAG.h"


namespace Dynagraph {

using DynaDAG::DynaDAGLayout;

creators<DynaDAGLayout>::creator_map &creators<DynaDAGLayout>::getMap() {
	static creator_map s_map;
	return s_map;
}

AddEngine<DynaDAGLayout,DynaDAG::DynaDAGServer> DDEngine("dynadag");
AddEngine<DynaDAGLayout,LabelPlacer<DynaDAGLayout> > DDLabels("labels");
AddEngine<DynaDAGLayout,ShapeGenerator<DynaDAGLayout> > DDShapes("shapegen");

} // namespace Dynagraph
