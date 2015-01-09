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
#include "ArtificialHorizonCalculator.h"
#include <Tools/CameraGeometry.h>


ArtificialHorizonCalculator::ArtificialHorizonCalculator()
:
  cameraID(CameraInfo::Bottom)
{
  DEBUG_REQUEST_REGISTER("ArtificialHorizonCalculator:draw_horizon", "draw the artificial horizon as a line in th image", false);  
}

void ArtificialHorizonCalculator::execute(CameraInfo::CameraID id)
{
  cameraID = id;
  Vector2d a,b;
  CameraGeometry::calculateArtificialHorizon(getCameraMatrix(), getCameraInfo(), a, b);
  getArtificialHorizon() = ArtificialHorizon(a, b);

  DEBUG_REQUEST("ArtificialHorizonCalculator:draw_horizon",
    LINE_PX(ColorClasses::red,
      (int)(getArtificialHorizon().begin().x +0.5), (int)(getArtificialHorizon().begin().y +0.5),
      (int)(getArtificialHorizon().end().x +0.5), (int)(getArtificialHorizon().end().y +0.5));
  );
}

ArtificialHorizonCalculator::~ArtificialHorizonCalculator()
{}
