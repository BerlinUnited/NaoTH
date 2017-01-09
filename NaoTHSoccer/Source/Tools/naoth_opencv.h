#ifndef NAOTH_OPENCV_H
#define NAOTH_OPENCV_H

// NOTE: we assume GCC version >= 4.9
#if defined(__GNUC__)
// save the current state
#pragma GCC diagnostic push
// ignore warnings
#pragma GCC diagnostic ignored "-Wconversion"

#if (__GNUC__ > 3 && __GNUC_MINOR__ > 8) || (__GNUC__ > 4) // version >= 4.9
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif
#endif

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/ml/ml.hpp>

#if defined(__GNUC__)
// restore the old state
#pragma GCC diagnostic pop
#endif

#endif // NAOTH_OPENCV_H
