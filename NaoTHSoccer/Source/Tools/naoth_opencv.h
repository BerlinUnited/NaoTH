#ifndef NAOTH_OPENCV_H
#define NAOTH_OPENCV_H

#if defined(__GNUC__) && defined(_NAOTH_CHECK_CONVERSION_)
#if (__GNUC__ > 3 && __GNUC_MINOR__ > 5) || (__GNUC__ > 4) // version >= 4.6
#pragma GCC diagnostic push
#endif
#pragma GCC diagnostic ignored "-Wconversion"
#if (__GNUC__ > 3 && __GNUC_MINOR__ > 8) || (__GNUC__ > 4) // version >= 4.9
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif
#endif

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/ml/ml.hpp>

#if defined(__GNUC__) && defined(_NAOTH_CHECK_CONVERSION_)
#if (__GNUC__ > 3 && __GNUC_MINOR__ > 5) || (__GNUC__ > 4)  // version >= 4.6
#pragma GCC diagnostic push
#endif
#pragma GCC diagnostic error "-Wconversion"
#if (__GNUC__ > 3 && __GNUC_MINOR__ > 8) || (__GNUC__ > 4) // version >= 4.9
#pragma GCC diagnostic error "-Wfloat-conversion"
#endif
#endif

#endif // NAOTH_OPENCV_H
