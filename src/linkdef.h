/** Generate CINT dictionary to wrap libavalanche for PyROOT */

#ifdef __CINT__

#pragma link off all global;
#pragma link off all class;
#pragma link off all function;

#pragma link C++ nestedtypedefs;
#pragma link C++ nestedclasses;

#pragma link C++ class avalanche::docObjectMap!;
#pragma link C++ class avalanche::client!;

#pragma link C++ class map<string, vector<string> >;
#pragma link C++ class vector<string>;

#endif

