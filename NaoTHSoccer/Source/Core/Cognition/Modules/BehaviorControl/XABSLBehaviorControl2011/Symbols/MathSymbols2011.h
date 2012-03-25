/** 
* @file MathSymbols.h
*
* Declaration of class MathSymbols.
*
* @author Max Risler
*/

#ifndef __MathSymbols2011_h_
#define __MathSymbols2011_h_

#include <ModuleFramework/Module.h>
#include <XabslEngine/XabslEngine.h>

#include "Tools/Math/Common.h"

using namespace Math;


BEGIN_DECLARE_MODULE(MathSymbols2011)
END_DECLARE_MODULE(MathSymbols2011)

/**
* The Xabsl symbols that are defined in "math_symbols.xabsl"
*
* @author Max Risler
*/ 
class MathSymbols2011: public MathSymbols2011Base
{
public:
  /*
  * Constructor.
  */
  MathSymbols2011()
  :
    normalizeAngle(0.0),
    sgnValue(0.0),
    absValue(0.0),
    min0Value(0.0),
    min1Value(0.0),
    max0Value(0.0),
    max1Value(0.0),
    alphaValue(0.0),
    atan2y(0.0),
    atan2x(0.0),
    clipValue(0.0),
    clipMin(0.0),
    clipMax(0.0),
    vectorAbsX(0.0),
    vectorAbsY(0.0)
  {
    theInstance = this;
  }

  /** registers the symbols at an engine */
  void registerSymbols(xabsl::Engine& engine);
    
  virtual void execute(){}
private:

  static MathSymbols2011* theInstance; /**< Points to the only instance of this class in this process or is 0 if there is none. */

  /** The parameter "normalize.angle" of the function "normalize" */
  double normalizeAngle;

  /** calculates the decimal input function "sgn" */
  static double getSgn();

  /** calculates the decimal input function "min" */
  static double getMin();

  /** calculates the decimal input function "atan2" */
  static double getAtan2();

  /** calculates the decimal input function "max" */
  static double getMax();

  /** calculates the decimal input function "abs" */
  static double getAbs();

  /** calculates the decimal input function "random" */
  static double getRandom();

  /** calculates the decimal input function "sin" */
  static double getSin();

  /** calculates the decimal input function "cos" */
  static double getCos();

  /** calculates the decimal input function "tan" */
  static double getTan();

  /** calculates the decimal input function "normalize" */
  static double getNormalize();

  static double getClip();

  static double getVectorAbs();

  /** The parameter "sgn.value" of the function "sgn" */
  double sgnValue;

  /** The parameter "abs.value" of the function "abs" */
  double absValue;

  /** The parameter "min0.value" of the function "min" */
  double min0Value;

  /** The parameter "min1.value" of the function "min" */
  double min1Value;

  /** The parameter "max.value0" of the function "max" */
  double max0Value;

  /** The parameter "max.value1" of the function "max" */
  double max1Value;

  /** The parameter "*.alpha" of the function "sin" and "cos" */
  double alphaValue;

  /** The parameter "atan2.y" of the function "atan2" */
  double atan2y;

  /** The parameter "atan2.x" of the function "atan2" */
  double atan2x;

  double clipValue;
  double clipMin;
  double clipMax;

  double vectorAbsX;
  double vectorAbsY;
};

#endif // __MathSymbols2011_h_

