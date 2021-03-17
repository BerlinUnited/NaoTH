/**
 * @file IntegralImageProvider.cpp
 *
 * Implementation of class IntegralImageProvider
 *
 */

#include "IntegralImageProvider.h"

IntegralImageProvider::IntegralImageProvider() {}

IntegralImageProvider::~IntegralImageProvider() {}

void IntegralImageProvider::execute(CameraInfo::CameraID id) {
    cameraID = id;
    makeIntegralBild(getBallDetectorIntegralImage());
}

void IntegralImageProvider::makeIntegralBild(BallDetectorIntegralImage& integralImage) const 
{
    const int32_t FACTOR = integralImage.FACTOR;
    const uint32_t MAX_COLOR = integralImage.MAX_COLOR;

    const uint32_t imgWidth = getImage().width() / FACTOR;
    const uint32_t imgHeight = getImage().height() / FACTOR;
    integralImage.setDimension(imgWidth, imgHeight);

    uint32_t* dataPtr = integralImage.getDataPointer();

    // NOTE: a pixel consists of two Y values (YUYV format).
    // When skipping a pixel using this pointer, you effectivly skip two Y
    // values.
    const Pixel* imgPtr = reinterpret_cast<Pixel*>(getImage().data());
    const int32_t FACTOR_HALF = integralImage.FACTOR / 2;

    // We need to skip FACTOR-1 lines in the image after after each processed
    // line. The image pixels contain 2 Y values, and thus only half of the
    // pixels are skipped.
    uint32_t pixels2SkipAfterLine = (FACTOR - 1) * (imgWidth * FACTOR_HALF);

    // iterate over first line
    uint32_t* curRowPtr = dataPtr;
    {
        uint32_t akk[MAX_COLOR] = {};
        for (uint16_t x = 0; x < imgWidth; ++x) {
            if (getFieldColorPercept().greenHSISeparator.isColor(*imgPtr)) {
                ++akk[1];
            } else {
                akk[0] += (imgPtr->y0);
            }

            for (uint16_t i = 0; i < MAX_COLOR; ++i) {
                curRowPtr[i] = akk[i];
            }

            // Increment current row by one step
            curRowPtr += MAX_COLOR;
            // The image pointer needs to skip 4 Y values == 2 image pixels
            imgPtr += FACTOR_HALF;
        }
    }

    imgPtr += pixels2SkipAfterLine;

    // set a pointer to the start of the first line
    uint32_t* prevRowPtr = dataPtr;

    // iterate over all remaining lines using the previously accumulated values
    for (uint16_t y = 1; y < imgHeight; ++y) {
        // reset accumalator in each line
        uint32_t akk[MAX_COLOR] = {};

        for (uint16_t x = 0; x < imgWidth; ++x) {
            if (getFieldColorPercept().greenHSISeparator.isColor(*imgPtr)) {
                ++akk[1];
            } else {
                akk[0] += (imgPtr->y0);
            }

            for (uint16_t i = 0; i < MAX_COLOR; ++i) {
                curRowPtr[i] = akk[i] + prevRowPtr[i];
            }
            // Both the current row and the previous row are incremented by one
            // step
            curRowPtr += MAX_COLOR;
            prevRowPtr += MAX_COLOR;
            // The image pointer needs to skip 4 Y values == 2 image pixels
            imgPtr += FACTOR_HALF;
        }

        imgPtr += pixels2SkipAfterLine;
    }
}