/**
* @file Vector_n.h
* Contains class Vector_n
*
* @author <a href="mailto:stefanuhrig@gmx.net">Stefan Uhrig</a>
*/
//------------------------------------------------------------------------------
#ifndef VECTOR_N_H_INCLUDED
#define VECTOR_N_H_INCLUDED
//------------------------------------------------------------------------------
#define VECTOR_N_STACK
//------------------------------------------------------------------------------
#include <math.h>
#include <stdlib.h>
#include "MVTools.h"
//------------------------------------------------------------------------------
// Ensure that memory version (use stack or heap) ist defined
#if !defined(VECTOR_N_STACK) && !defined(VECTOR_N_HEAP)
#error Define VECTOR_N_STACK if you'd like Vector_n to use stack memory or \
VECTOR_N_HEAP if you'd like Vector_n to use heap memory!
#endif
//------------------------------------------------------------------------------
/**
* @class Vector_n
* Represents a n-dimensional vector of type T.
*
* Vector_n represents a n-dimensional vector of type T. The class supplies
* standard arithmetic operations.
*/
template <class T, size_t N>
class Vector_n
{
public:
  //----------------------------------------------------------------------------
  /**
  * Standard constructor
  *
  * Initializes the vector with values supplied by the standard constructor
  * of T.
  *
  * Complexity: n
  */
  Vector_n()
  {
    #if defined(VECTOR_N_HEAP)
      content = new T[N];
    #endif

    T zero = T();
    for (size_t i = 0; i < N; ++i)
      content[i] = zero;
  }
  //----------------------------------------------------------------------------
  /**
  * Constructor that initializes the vector with the values in the passed array
  * @param v Array with initialization values
  *
  * Complexity: n
  */
  Vector_n(const T* v)
  {
    #if defined(VECTOR_N_HEAP)
      content = new T[N];
    #endif
    
    for (size_t i = 0; i < N; ++i)
      content[i] = v[i];
  }
  //----------------------------------------------------------------------------
  /** 
  * Copy constructor
  * @param v Vector to copy
  *
  * Complexity: n
  */
  Vector_n(const Vector_n<T, N>& v)
  {
    #if defined(VECTOR_N_HEAP)
      content = new T[N];
    #endif

    *this = v;
  }
  //----------------------------------------------------------------------------
  /**
  * Destructor
  *
  * Complexity: n
  */
  ~Vector_n()
  {
    #if defined(VECTOR_N_HEAP)
      delete [] content;
    #endif
  }
  //----------------------------------------------------------------------------
  /**
  * Copy operator
  * @param v Vector to copy
  *
  * Complexity: n
  */
  Vector_n<T, N>& operator=(const Vector_n<T, N>& v)
  {
    for (size_t i = 0; i < N; ++i)
      content[i] = v.content[i];
    return *this;
  }
  //----------------------------------------------------------------------------
  /**
  * Copy operator
  * @param v Array with initialization values
  *
  * Complexity: n
  */
  Vector_n<T, N>& operator=(const T* v)
  {
    for (size_t i = 0; i < N; ++i)
      content[i] = v[i];
    return *this;
  }
  //----------------------------------------------------------------------------
  /**
  * Copies contents of vector to passed array
  * @param v Array the values are stored to
  *
  * Complexity: n
  */
  void copyTo(T* v) const
  {
    const T* con = content;
    for (size_t i = 0; i < N; ++i, ++v, ++con)
      *v = *con;
  }
  //----------------------------------------------------------------------------
  /**
  * Constant element access operator
  * @param i Index of element to access (first element has index 0)
  * @return Constant reference to value
  *
  * Complexity: 1
  */
  const T& operator[](size_t i) const
  {
    return content[i];
  }
  //----------------------------------------------------------------------------
  /**
  * Element access operator
  * @param i Index of element to access (first element has index 0)
  * @return Reference to value
  *
  * Complexity: 1
  */
  T& operator[](size_t i)
  {
    return content[i];
  }
  //----------------------------------------------------------------------------
  /**
  * Operator +=
  * @param v Vector to add
  *
  * Complexity: n
  */
  Vector_n<T, N>& operator+=(const Vector_n<T, N>& v)
  {
    for (size_t i = 0; i < N; ++i)
      content[i] += v.content[i];
    return *this;
  }
  //----------------------------------------------------------------------------
  /**
  * Operator -=
  * @param v Vector to subtract
  *
  * Complexity: n
  */
  Vector_n<T, N>& operator-=(const Vector_n<T, N>& v)
  {
    for (size_t i = 0; i < N; ++i)
      content[i] -= v.content[i];
    return *this;
  }
  //----------------------------------------------------------------------------
  /**
  * Operator *=
  * @param s Scalar the vector is to multiplied with
  *
  * Complexity: n
  */
  Vector_n<T, N>& operator*=(const T& s)
  {
    for (size_t i = 0; i < N; ++i)
    {
      content[i] *= s;
      if (MVTools::isNearInf(content[i]))
      {
        if (MVTools::isNearPosInf(content[i]))
          throw MVException(MVException::PosInfValue);
        else
          throw MVException(MVException::NegInfValue);
      }
    }
    return *this;
  }
  //----------------------------------------------------------------------------
  /**
  * Operator /=
  * @param s Scalar the vector is to be divided by
  *
  * Complexity: n
  */
  Vector_n<T, N>& operator/=(const T& s)
  {
    if (MVTools::isNearZero(s))
    {
      if (MVTools::isNearNegZero(s))
        throw MVException(MVException::DivByNegZero);
      else
        throw MVException(MVException::DivByPosZero);
    }

    for (size_t i = 0; i < N; ++i)
    {
      content[i] /= s;
      if (MVTools::isNearInf(content[i]))
      {
        if (MVTools::isNearPosInf(content[i]))
          throw MVException(MVException::PosInfValue);
        else
          throw MVException(MVException::NegInfValue);
      }
    }
    return *this;
  }
  //----------------------------------------------------------------------------
    T length2()
    {
        T l = T(0);
        for (size_t i = 0; i < N; ++i)
            l += content[i] * content[i];
        return l;
    }
  //----------------------------------------------------------------------------
  /**
  * Get length of vector
  * @return Length of vector
  *
  * Complexity: n
  */
  T length()
  {
    return sqrt(length2());
  }
  //----------------------------------------------------------------------------
private:
  
  /** The array holding the contents of the vector */
  #if defined(VECTOR_N_HEAP)
    T* content;
  #elif defined(VECTOR_N_STACK)
    T content[N];
  #endif
};
//------------------------------------------------------------------------------
/**
* Operator +
* @param v1 First vector
* @param v2 Second vector
* @return v1+v2
*
* Complexity: n
*/
template<class T, size_t N>
Vector_n<T, N> operator+(const Vector_n<T, N>& v1,
                         const Vector_n<T, N>& v2)
{
  Vector_n<T, N> res(v1);
  res += v2;
  return res;
}
//------------------------------------------------------------------------------
/**
* Operator -
* @param v1 First vector
* @param v2 Second vector
* @return v1-v2
*
* Complexity: n
*/
template<class T, size_t N>
Vector_n<T, N> operator-(const Vector_n<T, N>& v1,
                         const Vector_n<T, N>& v2)
{
  Vector_n<T, N> res(v1);
  res -= v2;
  return res;
}
//------------------------------------------------------------------------------
/**
* Operator *
* @param s Scalar the vector is to multiplied with
* @param v Vector
* @return s*v
*
* Complexity: n
*/
template<class T, size_t N>
Vector_n<T, N> operator*(const T& s,
                         const Vector_n<T, N>& v)
{
  Vector_n<T, N> res(v);
  res *= s;
  return res;
}
//------------------------------------------------------------------------------
/**
* Operator *
* @param v Vector
* @param s Scalar the vector is to multiplied with
* @return v*s
*
* Complexity: n
*/
template<class T, size_t N>
Vector_n<T, N> operator*(const Vector_n<T, N>& v,
                         const T& s)
{
  Vector_n<T, N> res(v);
  res *= s;
  return res;
}
//------------------------------------------------------------------------------
/**
* Operator *
* @param v1 First Vector
* @param v2 Second Vector
* @return Scalar product <v1,v2>
*
* Complexity: n
*/
template <class T, size_t N>
T operator*(const Vector_n<T, N>& v1,
            const Vector_n<T, N>& v2)
{
  T res = T();
  for (size_t i = 0; i < N; ++i)
    res += v1[i]*v2[i];

  if (MVTools::isNearInf(res))
  {
    if (MVTools::isNearPosInf(res))
      throw MVException(MVException::PosInfValue);
    else
      throw MVException(MVException::NegInfValue);
  }
  return res;
}
//------------------------------------------------------------------------------
/**
* Operator /
* @param v Vector
* @param s Scalar the vector is to be divided by
* @return v/s
*
* Complexity: n
*/
template<class T, size_t N>
Vector_n<T, N> operator/(const Vector_n<T, N>& v,
                         const T& s)
{
  Vector_n<T, N> res(v);
  res /= s;
  return res;
}
//------------------------------------------------------------------------------
/**
* Get length of vector
* @param v Vector to retrieve length of
* @return Length of vector
*
* Complexity: n
*/
template<class T, size_t N>
T length(const Vector_n<T, N>& v)
{
  T l = T();
  for (size_t i = 0; i < N; ++i)
    l += v[i]*v[i];
  return sqrt(l);
}
//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
