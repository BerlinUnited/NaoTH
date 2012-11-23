/**
* @file Sample.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class Sample
*/

#ifndef __AGLSample_h_
#define __AGLSample_h_

#include "Cognition/Modules/Modeling/SelfLocator/MonteCarloSelfLocator/Sample.h"
#include "Tools/ColorClasses.h"

class AGLSample: public Sample2D
{
public: 
  AGLSample() : likelihood(0.0){}
  ~AGLSample() {}

  double likelihood;
  Vector2<double> translation;
  ColorClasses::Color color;

  virtual const Vector2<double>& getPos() const
  {
    return translation;
  }

};

#endif //__AGLSample_h_
