#ifndef ALINCLUDES
#define ALINCLUDES

// Special header to include all al* (naoqi) headers, but with special pragmas to supress GCC warnings.

// This is to suppress the following gcc warning
// thrown because by the old version of boost used by naoqi
// albroker.h and alproxy.h
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"


#include <boost/shared_ptr.hpp>

#include <almemoryfastaccess/almemoryfastaccess.h>
#include <alproxies/almemoryproxy.h>
#include <alproxies/dcmproxy.h>
#include <alcommon/albroker.h>
#include <alcommon/almodule.h>
#include <alcommon/albrokermanager.h>

#endif // ALINCLUDES

