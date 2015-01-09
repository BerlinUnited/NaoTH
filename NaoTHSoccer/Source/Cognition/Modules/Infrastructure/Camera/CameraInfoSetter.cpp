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

#include "CameraInfoSetter.h"

CameraInfoSetter::CameraInfoSetter()
{
}

void CameraInfoSetter::execute()
{
  const CameraSettings& camSettings = getCameraSettingsRequest();
  // copy parameter based representation to the "pure" one
  getCameraInfo().cameraID = CameraInfo::Bottom;

  // check if the camera settings actually changed something in the CameraInfo
  // uses
  if(camSettings.data[CameraSettings::ResolutionWidth] != 0)
    getCameraInfo().resolutionWidth = (unsigned int) camSettings.data[CameraSettings::ResolutionWidth];
  if(camSettings.data[CameraSettings::ResolutionHeight] != 0)
    getCameraInfo().resolutionHeight = (unsigned int) camSettings.data[CameraSettings::ResolutionHeight];

  // set param for image
  getImage().cameraInfo = getCameraInfo();

  const CameraSettings& camSettingsTop = getCameraSettingsRequestTop();
  // copy parameter based representation to the "pure" one
  getCameraInfoTop().cameraID = CameraInfo::Top;

  // check if the camera settings actually changed something in the CameraInfo
  // uses
  if(camSettingsTop.data[CameraSettings::ResolutionWidth] != 0)
    getCameraInfoTop().resolutionWidth = (unsigned int) camSettingsTop.data[CameraSettings::ResolutionWidth];
  if(camSettingsTop.data[CameraSettings::ResolutionHeight] != 0)
    getCameraInfoTop().resolutionHeight = (unsigned int) camSettingsTop.data[CameraSettings::ResolutionHeight];

  // set param for image
  getImageTop().cameraInfo = getCameraInfoTop();

  if(getCameraInfo().resolutionWidth != naoth::IMAGE_WIDTH || getCameraInfo().resolutionHeight != naoth::IMAGE_HEIGHT) {
    THROW("Image size of the CameraInfo doesn't correspond to the static values IMAGE_WIDTH and IMAGE_HEIGHT.");
  }
}

CameraInfoSetter::~CameraInfoSetter()
{

}

