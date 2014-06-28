/**
* @file Edgel.h
*
* Definition of one dimensional filter
*/

#ifndef _Filter_H_
#define _Filter_H_

#include <Tools/Math/Vector2.h>
#include <Tools/DataStructures/RingBuffer.h>

template<template<typename V, int SIZE> class F, class T, typename V>
class Filter
{
private:
  static const int SIZE = 5;
  RingBuffer<T, SIZE> pointBuffer;
  RingBuffer<V, SIZE> valueBuffer;
  F<V,SIZE> function;

public:
  inline bool ready() const {
    return pointBuffer.isFull();
  }

  inline void add(const T& point, V value) {
    pointBuffer.add(point);
    valueBuffer.add(value);
  }

  inline V value() const {
    ASSERT(valueBuffer.isFull());
    return function(valueBuffer);
  }

  inline const Vector2i& point() const {
    return pointBuffer[2];
  }
};

// some predefined filtering functions
template<typename V, int SIZE>
class Prewitt3x1 {
  public: inline V operator()(const RingBuffer<V, SIZE>& values) const {
    return  -values[2] /*+ 0.0*values[1]*/ + values[0];
  }
};

template<typename V, int SIZE>
class Diff5x1 {
  public: inline V operator()(const RingBuffer<V, SIZE>& values) const {
    return (-values[4] - 2.0*values[3] /*+ 0.0*values[2]*/ + 2.0*values[1] + values[0]) / 3.0;
  }
};

template<typename V, int SIZE>
class Gauss5x1 {
  public: inline V operator()(const RingBuffer<V, SIZE>& values) const {
    return (values[4] + 4.0*values[3] + 6.0*values[2] + 4.0*values[1] + values[0]) / 12.0;
  }
};

#endif //_Filter_H_
