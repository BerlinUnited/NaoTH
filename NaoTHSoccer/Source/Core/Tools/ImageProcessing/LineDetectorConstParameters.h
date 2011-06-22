/**
* @file LineDetectorConstParameters.h
*
* @author <a href="mailto:critter@informatik.hu-berlin.de">Claas-Norman Ritter</a>
* Definition of constants for the LineDetector
*/

#ifndef __LineDetectorConstParameters_H_
#define __LineDetectorConstParameters_H_

#define MAX_POSSIBLE_LINE_THICKNESS 80
#define STEP_SIZE 1
#define GRAY_THRESHOLD 23
#define EDGEL_ANGLE_THRESHOLD 0.2
#define PIXEL_BORDER 2
#define SCANLINE_COUNT 22//11
#define SCANLINE_RESUME_COUNT 3//1
#define MAX_LINE_ANGLE_DIFF 0.085//0.1//0.035
#define MAX_FIELDLINE_ANGLE_DIFF 10 * MAX_LINE_ANGLE_DIFF//0.1//0.035
#define GREENAMOUNT 50//64
#define BRIGHTNESS 128

#endif //__LineDetectorConstParameters_H_
