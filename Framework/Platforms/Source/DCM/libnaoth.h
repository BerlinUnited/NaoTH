/**
* @file libnaoth.h
*
* @author <a href="mailto:welter@informatik.hu-berlin.de">Oliver Welter</a>
* 
*/

#ifndef _LIB_NAOTH_H
#define	_LIB_NAOTH_H

// ..::: Headers ::
#include <fstream>
#include <sstream>
#include <signal.h>

#include <alcore/alptr.h>
#include <alcommon/albroker.h>
#include <alcommon/almodule.h>
#include <alcommon/albrokermanager.h>
#include <alcommon/altoolsmain.h>

// ..::: Version Information ::
/** Major release version */
#define NAOTH_VERSION_MAJOR    "1"

/** Minor release version */
#define NAOTH_VERSION_MINOR    "6.0"

/** Revision release version */
#define NAOTH_VERSION_REVISION "2463"

/** Full string version: (ex. 0.1-r2) */
#define NAOTH_VERSION NAOTH_VERSION_MAJOR"."NAOTH_VERSION_MINOR"-r"NAOTH_VERSION_REVISION

/** Internal version name */
#define NAOTH_VERSION_CODENAME "internal"

/** Release date */
#define NAOTH_VERSION_DATE     "20100406"


#endif	/* _LIB_NAOTH_H */

