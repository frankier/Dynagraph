#include "common/createEngine.h"
#include "DynaDAGLayout.h"

namespace Dynagraph {

using DynaDAG::DynaDAGLayout;

creators<DynaDAGLayout>::creator_map &creators<DynaDAGLayout>::getMap() {
	static creator_map s_map;
	return s_map;
}

} // namespace Dynagraph
