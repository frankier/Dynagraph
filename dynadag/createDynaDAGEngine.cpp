#include "common/createEngine.h"
#include "DynaDAGLayout.h"

namespace Dynagraph {

using DynaDAG::DynaDAGLayout;
using namespace std;

template<>
creators<DynaDAGLayout>::creator_map &creators<DynaDAGLayout>::getMap() {
	static creator_map s_map;
	cerr << "map " << &s_map << endl;
	return *(creator_map*)0;//s_map;
}

} // namespace Dynagraph
