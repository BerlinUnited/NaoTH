#ifndef NAOTH_OPENCV_H
#define NAOTH_OPENCV_H

#if defined(__GNUC__) && defined(_NAOTH_CHECK_CONVERSION_)
#if (__GNUC__ > 3 && __GNUC_MINOR__ > 5) || (__GNUC__ > 4)
#pragma GCC diagnostic push
#endif
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#if defined(__GNUC__) && defined(_NAOTH_CHECK_CONVERSION_)
#if (__GNUC__ > 3 && __GNUC_MINOR__ > 5) || (__GNUC__ > 4)
#pragma GCC diagnostic push
#endif
#pragma GCC diagnostic error "-Wconversion"
#pragma GCC diagnostic error "-Wfloat-conversion"
#endif

#endif // NAOTH_OPENCV_H
