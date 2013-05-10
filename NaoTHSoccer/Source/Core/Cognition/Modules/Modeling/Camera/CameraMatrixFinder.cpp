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

CameraMatrixFinder::CameraMatrixFinder()
{
}

void CameraMatrixFinder::execute()
{
  executeForSpecificCamera(getImage(), getCameraMatrix());
  executeForSpecificCamera(getImage2(), getCameraMatrix2());
}

void CameraMatrixFinder::executeForSpecificCamera(const Image &img, CameraMatrix &matrix)
{
  const CameraMatrixBuffer& buffer = getCameraMatrixBuffer();
  std::cout << "finding camera matrix" << std::endl;
  if(buffer.getNumberOfEntries() < 1)
  {
    return;
  }

  int smallestDiff = std::numeric_limits<int>::max();
  unsigned int smallestIndex = 0; // default to last one

  for(int i=0; i < buffer.getNumberOfEntries(); i++)
  {
    const CameraMatrix& m = buffer.getEntry(i);
    // compare time stamp
    int diff = abs((int) m.timestamp - (int) img.timestamp);
    std::cout << "diff: " << diff << " (matrix: " << m.timestamp << " image: "
              << img.timestamp << ")" << std::endl;
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
  std::cout << "finding camera matrix with index " << smallestIndex <<
            " (diff was " << smallestDiff << ")" << std::endl;
  matrix = buffer.getEntry(smallestIndex);
}

CameraMatrixFinder::~CameraMatrixFinder()
{

}

