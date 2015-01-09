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

#ifndef ARTIFICIALHORIZON_H
#define ARTIFICIALHORIZON_H

#include <Tools/Math/Line.h>

class ArtificialHorizon : public Math::LineSegment
{
public:
  ArtificialHorizon() : Math::LineSegment() {};
  ArtificialHorizon(const Vector2d& begin, const Vector2d& end) : Math::LineSegment(begin, end) {};
  ~ArtificialHorizon() {};
};

class ArtificialHorizonTop : public ArtificialHorizon
{
public:
  virtual ~ArtificialHorizonTop() {}
};

#endif // ARTIFICIALHORIZON_H
