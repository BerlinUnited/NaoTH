/**
* @file Tools/ImageProcessing/MaximumScan.h
* 
*
*
*/

#ifndef _MaximumScan_h_
#define _MaximumScan_h_

template<typename T, typename V>
class MaximumScan
{
private:
  V threshhold;
  V value_max;

  // results of the scan
  T& peakMax;
  bool maximum;

public:

  MaximumScan(T& peakMax, V threshhold)
    : threshhold(threshhold),
      value_max(threshhold),
      peakMax(peakMax),
      maximum(false)
  {
  }

  inline bool add(const T& point, V value)
  {
    if(maximum) {
      value_max = threshhold;
      maximum = false;
    }

    if(value > value_max)
    {
      value_max = value;
      peakMax = point;
    } else if(value_max > threshhold && value < threshhold) {
      maximum = true;
    }

    return maximum;
  }

  inline bool isMax() const { return maximum; }
  inline V maxValue() const { return value_max; }
};


// HIGHLY EXPERIMENTAL
template<template<class T, class V> class F, typename T, typename V>
class MaximumScanFiltered
{
private:
  MaximumScan<T,V> scan;
  F<T,V> filter;

public:

  MaximumScanFiltered(T& peakMax, V threshhold)
    : scan(peakMax, threshhold)
  {
  }

  inline bool add(const T& point, V value)
  {
    filter.add(point, value);
    if(filter.ready()) {
      scan.add(filter.point(), filter.value());
    }

    return scan.isMax();
  }

  inline bool isMax() const { return scan.isMax(); }
  inline V maxValue() const { return scan.maxValue(); }
};

#endif //_MaximumScan_h_
