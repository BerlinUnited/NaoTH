/**
 * @file NaoTHAssert.h
 *
 * This file contains macros for low level debugging.
 * (partly copied from GT07 GTAssert.h)
 *
 * @author Thomas Röfer (GT07)
 * @author Heinrich Mellmann
 */

#ifndef __NaoTHAssert_h_
#define __NaoTHAssert_h_

#include <assert.h>
#include <iostream>
#include "Trace.h"

#undef ASSERT
#undef VERIFY
#undef THROW

#ifdef DEBUG

/**
 * ASSERT prints a message and stops the programm (!) if cond is false and DEBUG is defined.
 * ASSERT does not evaluate cond if DEBUG is not defined.
 * @param cond The condition to be checked.
 */
// TODO: make it better
#define ASSERT(cond) { if(!(cond)){ GT_TRACE(#cond); Trace::getInstance().dump(); assert(cond); } }

/**
 * VERIFY prints a message if cond is false and DEBUG is defined.
 * VERIFY does evaluate cond even if DEBUG is not defined.
 * @param cond The condition to be checked.
 */
#define VERIFY(cond) ASSERT(cond)

/**
 * Throw a error message and then exit
 * @param msg The debug message
 */
#define THROW(msg) { std::cerr<<msg<<std::endl; GT_TRACE(msg); Trace::getInstance().dump(); assert(false); }

#else //DEBUG
/* ((void)0) - that's a do-nothing statement */
#define ASSERT(cond) ((void)0)
#define VERIFY(cond) ((void)(cond))
#define THROW(msg) { std::cerr<<msg<<std::endl; }
#endif //DEBUG

#endif //__NaoTHAssert_h_
