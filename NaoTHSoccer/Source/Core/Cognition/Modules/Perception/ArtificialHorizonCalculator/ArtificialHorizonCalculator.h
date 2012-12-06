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

#ifndef ARTIFICIALHORIZONCALCULATOR_H
#define ARTIFICIALHORIZONCALCULATOR_H

#include <ModuleFramework/Module.h>

#include <Representations/Infrastructure/CameraInfo.h>
#include <Representations/Perception/CameraMatrix.h>
#include "Representations/Perception/ArtificialHorizon.h"

BEGIN_DECLARE_MODULE(ArtificialHorizonCalculator)
  REQUIRE(CameraInfo)
  REQUIRE(CameraMatrix)
  PROVIDE(ArtificialHorizon)
END_DECLARE_MODULE(ArtificialHorizonCalculator)

class ArtificialHorizonCalculator : public ArtificialHorizonCalculatorBase
{
public:
  ArtificialHorizonCalculator();
  virtual ~ArtificialHorizonCalculator();

  virtual void execute();
};

#endif // ARTIFICIALHORIZONCALCULATOR_H
