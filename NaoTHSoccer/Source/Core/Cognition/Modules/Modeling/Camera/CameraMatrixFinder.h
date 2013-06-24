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

#ifndef CAMERAMATRIXFINDER_H
#define CAMERAMATRIXFINDER_H

#include <ModuleFramework/Module.h>
#include <Representations/Infrastructure/Image.h>
#include <Representations/Perception/CameraMatrix.h>
#include <Representations/Perception/CameraMatrixBuffer.h>

using namespace naoth;

BEGIN_DECLARE_MODULE(CameraMatrixFinder)
  REQUIRE(CameraMatrixBuffer)
  REQUIRE(CameraMatrixBuffer2)
  REQUIRE(Image)
  REQUIRE(ImageTop)

  PROVIDE(CameraMatrix)
  PROVIDE(CameraMatrixTop)
END_DECLARE_MODULE(CameraMatrixFinder)


/**
 * @brief Get the nearest camera matrix for the images from the buffer and
 *        sets them on the global blackboard.
 */
class CameraMatrixFinder : public CameraMatrixFinderBase
{
public:
  CameraMatrixFinder();
  virtual ~CameraMatrixFinder();

  virtual void execute();
private:
  void executeForSpecificCamera(const Image& img, CameraMatrix& matrix, const CameraMatrixBuffer &buffer);
};

#endif // CAMERAMATRIXFINDER_H
