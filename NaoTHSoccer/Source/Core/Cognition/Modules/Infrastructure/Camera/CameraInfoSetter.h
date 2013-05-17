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

#ifndef CAMERAINFOSETTER_H
#define CAMERAINFOSETTER_H

#include <ModuleFramework/Module.h>
#include <Representations/Infrastructure/CameraInfo.h>
#include <Representations/Infrastructure/CameraSettings.h>
#include <Representations/Infrastructure/CameraInfo.h>
#include <Representations/Infrastructure/CameraInfo.h>
#include <Representations/Infrastructure/Image.h>


using namespace naoth;

BEGIN_DECLARE_MODULE(CameraInfoSetter)
  REQUIRE(CameraSettingsRequest)
  REQUIRE(CameraSettingsRequestTop)

  // we don't actually provide the Image but rather set the CameraInfo for it
  PROVIDE(Image)
  PROVIDE(ImageTop)

  PROVIDE(CameraInfoParameter)
  PROVIDE(CameraInfo)
  PROVIDE(CameraInfoTop)
END_DECLARE_MODULE(CameraInfoSetter)


/**
 * @brief Sets the CameraInfo into the Image, also loads CameraInfo from config.
 */
class CameraInfoSetter : public CameraInfoSetterBase
{
public:
  CameraInfoSetter();
  virtual ~CameraInfoSetter();

  virtual void execute();
private:
};

#endif // CAMERAINFOSETTER_H
