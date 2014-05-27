/**
* @file LineDetectorConstParameters.h
*
* @author <a href="mailto:critter@informatik.hu-berlin.de">Claas-Norman Ritter</a>
* Definition of constants for the LineDetector
*/

#ifndef __LineDetectorConstParameters_H_
#define __LineDetectorConstParameters_H_

// TODO: make a parameters class from it
#define MAX_POSSIBLE_LINE_THICKNESS 80
#define STEP_SIZE 1
#define GRAY_THRESHOLD 23
#define SLOPE_THRESHOLD 25
#define EDGEL_ANGLE_THRESHOLD 0.2
#define PIXEL_BORDER 2
#define SCANLINE_COUNT 31
#define SCANLINE_RESUME_COUNT 4
#define MAX_LINE_ANGLE_DIFF 0.085//0.17//0.1//0.035
#define MAX_FIELDLINE_ANGLE_DIFF 10 * MAX_LINE_ANGLE_DIFF//0.1//0.035
#define GREENAMOUNT 50//64
#define BRIGHTNESS 128
#define MAX_NUMBER_OF_SCANLINE_EDGELS 2 * SCANLINE_COUNT * (SCANLINE_RESUME_COUNT + 1)
#define MAX_NUMBER_OF_EDGELS_LINE_SEGMENT 4

#endif //__LineDetectorConstParameters_H_
