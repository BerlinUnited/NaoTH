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
{}

void ArtificialHorizonCalculator::execute(CameraInfo::CameraID id)
{
  cameraID = id;
  Vector2d a,b;
  CameraGeometry::calculateArtificialHorizon(getCameraMatrix(), getCameraInfo(), a, b);
  getArtificialHorizon() = ArtificialHorizon(a, b);
}

ArtificialHorizonCalculator::~ArtificialHorizonCalculator()
{}
