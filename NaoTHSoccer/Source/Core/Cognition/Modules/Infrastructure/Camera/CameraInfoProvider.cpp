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

#include "CameraInfoProvider.h"

CameraInfoProvider::CameraInfoProvider()
{
}

void CameraInfoProvider::execute()
{
  const CameraSettings& camSettings = getCameraSettings();
  // check if the camera settings actually changed something in the CameraInfo
  // uses
  param.resolutionWidth = (unsigned int) camSettings.data[CameraSettings::ResolutionWidth];
  param.resolutionHeight = (unsigned int) camSettings.data[CameraSettings::ResolutionHeight];

  // set param for image
  getImage().cameraInfo = param;
}

CameraInfoProvider::~CameraInfoProvider()
{

}

