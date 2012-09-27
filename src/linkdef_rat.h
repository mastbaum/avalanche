/** Generate CINT dictionary to wrap libavalanche for PyROOT
 *
 * RAT-specific features
 */

#include "linkdef.h"

#ifdef __CINT__

#pragma link C++ class avalanche::ratDocObjectMap!;

#endif

