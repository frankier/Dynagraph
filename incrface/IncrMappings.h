// how to map between all the graphs (of many different types) being handled
// through one incrface interpreter

#include "two_way_map.h"

struct NEID {
    bool isEdge;
    DString name;
    NEID() : isEdge(false),name() {}
    bool operator <(NEID o) {
        return isEdge<o.isEdge || name < o.name;
    }
    bool operator ==(NEID o) {
        return return isEdge==o.isEdge && name == o.name;
    }
    bool operator !=(NEID o) {
        return !(*this==o);
    }
};

typedef two_way_map<NEID> NEID_map;

typedef LGraph<ADTisCDT,Nothing,Name,NEID_map>