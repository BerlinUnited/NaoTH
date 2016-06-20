/**
* @file PatchWork.h
*
* Sorted list evaluated non-overlaping patches
*/

#ifndef _PatchWork_H_
#define _PatchWork_H_

#include <Representations/Infrastructure/Image.h>
#include <vector>

class PatchWork
{
public:
  static void subsampling(const naoth::Image& image, std::vector<unsigned char>& result, int x0, int y0, int x1, int y1, int size);
};

#endif // _PatchWork_H_
