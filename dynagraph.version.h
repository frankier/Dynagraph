#define Dg_MAJOR 1
#define Dg_MINOR 1
#define Dg_MINIC 5
#define Dg_MNEST 2

#define Dg_STRINGIZE0(x) #x
#define Dg_STRINGIZE(x) Dg_STRINGIZE0(x)

#define DYNAGRAPH_VERSION_DOTS Dg_MAJOR.Dg_MINOR.Dg_MINIC.Dg_MNEST
#define DYNAGRAPH_VERSION_COMMAS Dg_MAJOR,Dg_MINOR,Dg_MINIC,Dg_MNEST
#define DYNAGRAPH_VERSION_DOTS_QUOTED Dg_STRINGIZE(DYNAGRAPH_VERSION_DOTS)

