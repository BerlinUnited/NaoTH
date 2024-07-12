/**
* @file SIMD.h
* 
* Header file to be included for using SIMD instructions. Please never include intrinsic includes directly
* and include this one. This file makes sure that your SSE instructions are replaced by ARM NEON (or in
* future anything else) if someone has something else than an Intel CPU, e.g. an APPLE M CPU.
*
* @author <a href="mailto:oliver.urbann@tu-dortmund.de">Oliver Urbann</a>
*/

#if defined(__ARM_NEON) || defined(__ARM_NEON__)
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wconversion"
#pragma clang diagnostic ignored "-Wimplicit-int-conversion"
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
#endif
#include "sse2neon.h"
#define __EMMINTRIN_H
#define __PMMINTRIN_H
#define __TMMINTRIN_H
#define __IMMINTRIN_H
#define __XMMINTRIN_H
#ifdef __clang__
#pragma clang diagnostic pop
#endif
#else
#include <emmintrin.h>
#include <pmmintrin.h>
#include <tmmintrin.h>
#include <immintrin.h>
#include <xmmintrin.h>
#endif