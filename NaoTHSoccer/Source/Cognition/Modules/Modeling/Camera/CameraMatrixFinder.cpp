/*
   Copyright 2011 Thomas Krause
   
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
   
       http://www.apache.org/licenses/LICENSE-2.0
       
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include "CameraMatrixFinder.h"
#include <Tools/Debug/DebugBufferedOutput.h>
CameraMatrixFinder::CameraMatrixFinder()
{
}

void CameraMatrixFinder::execute()
{
  // BOTTOM CAMERA
  if(getCameraMatrixBuffer().size() > 0)
  {

    int smallestDiff = std::numeric_limits<int>::max();
    unsigned int smallestIndex = 0; // default to last one

    for(int i=0; i < getCameraMatrixBuffer().size(); i++)
    {
      const CameraMatrix& m = getCameraMatrixBuffer().getEntry(i);
      // compare time stamp
      int diff = abs((int) m.timestamp - (int) getImage().timestamp);
      if(diff < smallestDiff)
      {
        smallestDiff = diff;
        smallestIndex = i;
      }
      else
      {
        // since the time is continous, we can't get better if we are already
        // got worse once
        break;
      }
    }

    // actually set the cameraMatrix
    getCameraMatrix() = getCameraMatrixBuffer().getEntry(smallestIndex);

    double oldDiff = fabs((double) getCameraMatrixBuffer().getEntry(0).timestamp -
                          (double) getImage().timestamp);
    PLOT("bottom-cammatrix-old-diff", oldDiff);
    double bestDiff = smallestDiff;
    PLOT("bottom-cammatrix-best-diff", bestDiff);

  }

  // TOP CAMERA
  if(getCameraMatrixBuffer2().size() > 0)
  {
    int smallestDiff = std::numeric_limits<int>::max();
    unsigned int smallestIndex = 0; // default to last one

    for(int i=0; i < getCameraMatrixBuffer2().size(); i++)
    {
      const CameraMatrix& m = getCameraMatrixBuffer2().getEntry(i);
      // compare time stamp
      int diff = abs((int) m.timestamp - (int) getImageTop().timestamp);
      if(diff < smallestDiff)
      {
        smallestDiff = diff;
        smallestIndex = i;
      }
      else
      {
        // since the time is continous, we can't get better if we are already
        // got worse once
        break;
      }
    }

    // actually set the cameraMatrix
    getCameraMatrixTop() = getCameraMatrixBuffer2().getEntry(smallestIndex);

    double oldDiff = fabs((double) getCameraMatrixBuffer2().getEntry(0).timestamp
                          - (double) getImageTop().timestamp);
    PLOT("top-cammatrix-old-diff", oldDiff);
    double bestDiff = smallestDiff;
    PLOT("top-cammatrix-best-diff", bestDiff);

  }
}


CameraMatrixFinder::~CameraMatrixFinder()
{

}

