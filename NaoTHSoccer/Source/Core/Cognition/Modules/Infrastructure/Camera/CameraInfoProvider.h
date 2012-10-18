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

#ifndef CAMERAINFOPROVIDER_H
#define CAMERAINFOPROVIDER_H

#include <ModuleFramework/Module.h>
#include <Representations/Infrastructure/CameraInfo.h>
#include <Representations/Infrastructure/CameraSettings.h>
#include <Representations/Infrastructure/CameraInfo.h>

using namespace naoth;

BEGIN_DECLARE_MODULE(CameraInfoProvider)
  REQUIRE(CameraSettings)
  PROVIDE(CameraInfo)
END_DECLARE_MODULE(CameraInfoProvider)


class CameraInfoProvider : public CameraInfoProviderBase
{
public:
  CameraInfoProvider();
  virtual ~CameraInfoProvider();

  virtual void execute();
};

#endif // CAMERAINFOPROVIDER_H
