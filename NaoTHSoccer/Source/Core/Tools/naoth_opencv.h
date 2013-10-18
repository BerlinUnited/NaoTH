#ifndef NAOTH_OPENCV_H
#define NAOTH_OPENCV_H

#if defined(__GNUC__) && defined(_NAOTH_CHECK_CONVERSION_)
#if __GNUC__ > 3 && __GNUC_MINOR__ > 5
#pragma GCC diagnostic push
#endif
#pragma GCC diagnostic ignored "-Wconversion"
#endif
#include <opencv2/core/core.hpp>
#if defined(__GNUC__) && defined(_NAOTH_CHECK_CONVERSION_)
#if __GNUC__ > 3 && __GNUC_MINOR__ > 5
#pragma GCC diagnostic push
#endif
#pragma GCC diagnostic error "-Wconversion"
#endif

#endif // NAOTH_OPENCV_H
