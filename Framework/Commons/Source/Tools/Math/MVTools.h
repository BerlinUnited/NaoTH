#ifndef MVTOOLS_H_INCLUDED
#define MVTOOLS_H_INCLUDED

class MVException
{
public:
  enum ExceptionType
  {
    Unknown,
    DivByPosZero,
    DivByNegZero,
    PosInfValue,
    NegInfValue,
    DetNegative,
    DetCloseToZero
  };

  MVException() : type(Unknown) {}
  MVException(ExceptionType t) : type(t) {}

public:
  const char* getDescription() const;

public:
  /// type of exception
  ExceptionType type;
};

namespace MVTools
{
  bool isNearZero(float value);
  bool isNearPosZero(float value);
  bool isNearNegZero(float value);

  bool isNearInf(float value);
  bool isNearPosInf(float value);
  bool isNearNegInf(float value);

  double getMaxExpDouble();
  double getMaxPosDouble();
  double getMinPosDouble();

  bool isNearZero(double value);
  bool isNearPosZero(double value);
  bool isNearNegZero(double value);

  bool isNearInf(double value);
  bool isNearPosInf(double value);
  bool isNearNegInf(double value);

  float getMaxExpFloat();
  float getMaxPosFloat();
  float getMinPosFloat();

  bool isNearZero(int value);
  bool isNearPosZero(int value);
  bool isNearNegZero(int value);

  bool isNearInf(int value);
  bool isNearPosInf(int value);
  bool isNearNegInf(int value);

}; 
#endif

