/*
 * File:   CameraInfo.h
 * Author: Oliver Welter
 *
 * Created on 1. Februar 2009, 20:07
 */

#ifndef _CAMERAINFOCONSTANTS_H
#define _CAMERAINFOCONSTANTS_H

namespace naoth
{

#ifdef QVGA
  static const unsigned int IMAGE_WIDTH = 320;
  static const unsigned int IMAGE_HEIGHT = 240;
#else
  static const unsigned int IMAGE_WIDTH = 640;
  static const unsigned int IMAGE_HEIGHT = 480;
#endif

}
#endif  /* _CAMERAINFOCONSTANTS_H */

