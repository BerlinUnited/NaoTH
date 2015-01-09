/** 
* @file MathSymbols.cpp
*
* Implementation of class MathSymbols
*
* @author Max Risler
*/

#include "MathSymbols.h"

MathSymbols* MathSymbols::theInstance = 0;

void MathSymbols::registerSymbols(xabsl::Engine& engine)
{
  // "atan2"
  engine.registerDecimalInputSymbol("atan2", &getAtan2);
  engine.registerDecimalInputSymbolDecimalParameter("atan2", "atan2.y", &atan2y);
  engine.registerDecimalInputSymbolDecimalParameter("atan2", "atan2.x", &atan2x);
  // "min"
  engine.registerDecimalInputSymbol("min", &getMin);
  engine.registerDecimalInputSymbolDecimalParameter("min", "min.value0", &min0Value);
  engine.registerDecimalInputSymbolDecimalParameter("min", "min.value1", &min1Value);

  // "max"
  engine.registerDecimalInputSymbol("max", &getMax);
  engine.registerDecimalInputSymbolDecimalParameter("max", "max.value0", &max0Value);
  engine.registerDecimalInputSymbolDecimalParameter("max", "max.value1", &max1Value);

  // "abs"
  engine.registerDecimalInputSymbol("abs", &getAbs);
  engine.registerDecimalInputSymbolDecimalParameter("abs", "abs.value", &absValue);

  // "sgn"
  engine.registerDecimalInputSymbol("sgn", &getSgn);
  engine.registerDecimalInputSymbolDecimalParameter("sgn", "sgn.value", &sgnValue);

  // "sin"
  engine.registerDecimalInputSymbol("sin", &getSin);
  engine.registerDecimalInputSymbolDecimalParameter("sin", "sin.angle", &alphaValue);

  // "cos"
  engine.registerDecimalInputSymbol("cos", &getCos);
  engine.registerDecimalInputSymbolDecimalParameter("cos", "cos.angle", &alphaValue);

  // "tan"
  engine.registerDecimalInputSymbol("tan", &getTan);
  engine.registerDecimalInputSymbolDecimalParameter("tan", "tan.angle", &alphaValue);

  // "random"
  engine.registerDecimalInputSymbol("random", &getRandom);

  // "normalize"
  engine.registerDecimalInputSymbol("normalize", &getNormalize);
  engine.registerDecimalInputSymbolDecimalParameter("normalize", "normalize.angle", &normalizeAngle);

  // "clip"
  engine.registerDecimalInputSymbol("clip", &getClip);
  engine.registerDecimalInputSymbolDecimalParameter("clip", "clip.value", &clipValue);
  engine.registerDecimalInputSymbolDecimalParameter("clip", "clip.min", &clipMin);
  engine.registerDecimalInputSymbolDecimalParameter("clip", "clip.max", &clipMax);

  // "vector.abs"
  engine.registerDecimalInputSymbol("vector.abs", &getVectorAbs);
  engine.registerDecimalInputSymbolDecimalParameter("vector.abs", "vector.abs.x", &vectorAbsX);
  engine.registerDecimalInputSymbolDecimalParameter("vector.abs", "vector.abs.y", &vectorAbsY);

}

double MathSymbols::getSgn()
{
  return theInstance->sgnValue > 0 ? 1 : -1;
}

double MathSymbols::getAbs()
{
  return fabs(theInstance->absValue);
}

double MathSymbols::getMin()
{
  if (theInstance->min0Value < theInstance->min1Value)
    return theInstance->min0Value;
  return theInstance->min1Value;
}

double MathSymbols::getAtan2()
{
  return toDegrees(atan2(theInstance->atan2y,theInstance->atan2x));
}

double MathSymbols::getMax()
{
  if (theInstance->max0Value > theInstance->max1Value)
    return theInstance->max0Value;
  return theInstance->max1Value;
}

double MathSymbols::getSin()
{
  return sin(fromDegrees(theInstance->alphaValue));
}

double MathSymbols::getCos()
{
  return cos(fromDegrees(theInstance->alphaValue));
}

double MathSymbols::getTan()
{
  return tan(fromDegrees(theInstance->alphaValue));
}

double MathSymbols::getRandom()
{
  return Math::random();
}

double MathSymbols::getNormalize()
{
  return toDegrees(normalize(fromDegrees(theInstance->normalizeAngle)));
}

double MathSymbols::getClip()
{
  if (theInstance->clipValue < theInstance->clipMin) return theInstance->clipMin;
  if (theInstance->clipValue > theInstance->clipMax) return theInstance->clipMax;
  return theInstance->clipValue;
}

double MathSymbols::getVectorAbs()
{
  return sqrt(sqr(theInstance->vectorAbsX)+sqr(theInstance->vectorAbsY));
}
