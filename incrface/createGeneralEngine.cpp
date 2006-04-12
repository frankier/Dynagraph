#include "common/createEngine.h"
#include "common/GeneralLayout.h"
#include "common/ColorByAge.h"
#include "common/LabelPlacer.h"
#include "common/ShapeGenerator.h"

namespace Dynagraph {

creators<GeneralLayout>::creator_map &creators<GeneralLayout>::getMap() {
	static creator_map s_map;
	return s_map;
}

AddEngine<GeneralLayout, LabelPlacer<GeneralLayout> > GLLabels("labels");
AddEngine<GeneralLayout, ShapeGenerator<GeneralLayout> > GLShapes("shapegen");
AddEngine<GeneralLayout, ColorByAge<GeneralLayout> > GLColors("colorbyage");

} // namespace Dynagraph
