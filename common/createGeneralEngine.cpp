#include "common/createEngine.h"
#include "common/GeneralLayout.h"

namespace Dynagraph {

creators<GeneralLayout>::creator_map &creators<GeneralLayout>::getMap() {
	static creator_map s_map;
	return s_map;
}

} // namespace Dynagraph
