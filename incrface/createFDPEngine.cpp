#include "common/createEngine.h"
#include "voronoi/voronoi.h"
#include "fdp/fdp.h"
#include "shortspline/ObAvSplinerEngine.h"
#include "common/LabelPlacer.h"
#include "common/ShapeGenerator.h"

namespace Dynagraph {

using FDP::FDPLayout;

creators<FDPLayout>::creator_map &creators<FDPLayout>::getMap() {
	static creator_map s_map;
	return s_map;
}

AddEngine<FDPLayout,FDP::FDPServer> FDPServer("fdp");
AddEngine<FDPLayout,Voronoi::VoronoiServer> FDPVoronoi("voronoi");
AddEngine<FDPLayout,ObAvSplinerEngine<FDPLayout> > FDPSplines("visspline");
AddEngine<FDPLayout,LabelPlacer<FDPLayout> > FDPLabels("labels");
AddEngine<FDPLayout,ShapeGenerator<FDPLayout> > FDPShapes("shapegen");

} // namespace Dynagraph
