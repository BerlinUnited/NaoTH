//------------------------------------------------------------------------------
#ifndef MATRIX_NXN_H_INCLUDED
#define MATRIX_NXN_H_INCLUDED
//------------------------------------------------------------------------------
#define MATRIX_NXN_HEAP
//------------------------------------------------------------------------------

#ifdef _WIN32
#pragma warning(disable:4786) 
// The VC6 compiler produced the uncritical warning 4786 (too long debug identifier)
#endif

#include "MVTools.h"
#include "Vector_n.h"
//------------------------------------------------------------------------------
// Ensure that memory version (use stack or heap) ist defined
#if !defined(MATRIX_NXN_STACK) && !defined(MATRIX_NXN_HEAP)
#error Define MATRIX_NXN_STACK if you'd like Matrix_nxn to use stack memory or \
MATRIX_NXN_HEAP if you'd like Matrix_nxn to use heap memory!
#endif
//------------------------------------------------------------------------------
// Forward-Deklarationen
//------------------------------------------------------------------------------
template <class T, size_t N>
class Matrix_nxn;
template<class T, size_t N>
Matrix_nxn<T, N> operator*(const Matrix_nxn<T, N>& m1,
                           const Matrix_nxn<T, N>& m2);
//------------------------------------------------------------------------------
/**
* @class Matrix_nxn
* Represents a nxn matrix of type T
*
* Matrix_nxn represents a nxn matrix of type T. The class supplies
* standard arithmetic operations.
*/
template <class T, size_t N>
class Matrix_nxn
{
public:
  //----------------------------------------------------------------------------
  /**
  * Standard constructor
  *
  * Initializes the matrix with values supplied by the standard constructor
  * of T.
  *
  * Complexity: n^2
  */
  Matrix_nxn()
  {
#ifdef MATRIX_NXN_HEAP
    content = new Vector_n<T, N>[N];
#endif
    
    Vector_n<T, N>* init = content;
    Vector_n<T, N> zero = Vector_n<T, N>();
    for (size_t i = 0; i < N; ++i, ++init)
      *init = zero;
  }
  //----------------------------------------------------------------------------
  /**
  * Constructor that initializes the matrix with the values in the passed array
  * @param v Array with initialization values
  *
  * Complexity: n^2
  */
  Matrix_nxn(const T* v)
  {
#ifdef MATRIX_NXN_HEAP
    content = new Vector_n<T, N>[N];
#endif
    
    Vector_n<T, N>* init = content;
    for (size_t i = 0; i < N; ++i, ++init, v+=N)
      *init = Vector_n<T, N>(v);
  }
  //----------------------------------------------------------------------------
  /** 
  * Copy constructor
  * @param m Matrix to copy
  *
  * Complexity: n^2
  */
  Matrix_nxn(const Matrix_nxn<T, N>& m)
  {
#ifdef MATRIX_NXN_HEAP
    content = new Vector_n<T, N>[N];
#endif
    
    *this = m;
  }
  //----------------------------------------------------------------------------
  /**
  * Destructor
  *
  * Complexity: n^2
  */
  ~Matrix_nxn()
  {
#ifdef MATRIX_NXN_HEAP
    delete [] content;
#endif
  }
  //----------------------------------------------------------------------------
  /**
  * Copy operator
  * @param m Matrix to copy
  *
  * Complexity: n^2
  */
  Matrix_nxn<T, N>& operator=(const Matrix_nxn<T, N>& m)
  {
    for (size_t i = 0; i < N; ++i)
      content[i] = m.content[i];
    return *this;
  }
  //----------------------------------------------------------------------------
  /**
  * Copy operator
  * @param m Array with values to assign to this matrix
  *
  * Complexity: n^2
  */
  Matrix_nxn<T, N>& operator=(const T* m)
  {
    for (size_t i = 0; i < N; ++i)
      for (size_t j = 0; j <N; ++j, ++m)
        content[i][j] = *m;
      return *this;
  }
  //----------------------------------------------------------------------------
  /**
  * Copies contents of matrix to passed array
  * @param m Array the values are stored to
  *
  * Complexity: n^2
  */
  void copyTo(T* m) const
  {
    const Vector_n<T, N>* con = content;
    for (size_t i = 0; i < N; ++i, m+=N, ++con)
      con->copyTo(m);
  }
  //----------------------------------------------------------------------------
  /**
  * Constant row vector access operator
  * @param i Index of row vector to access (first row vector has index 0)
  * @return Constant reference to row vector
  *
  * Complexity: 1
  */
  const Vector_n<T, N>& operator[](size_t i) const
  {
    return content[i];
  }
  //----------------------------------------------------------------------------
  /**
  * Row vector access operator
  * @param i Index of row vector to access (first row vector has index 0)
  * @return Reference to row vector
  *
  * Complexity: 1
  */
  Vector_n<T, N>& operator[](size_t i)
  {
    return content[i];
  }
  //----------------------------------------------------------------------------
  /**
  * Operator +=
  * @param m Matrix to add
  *
  * Complexity: n^2
  */
  Matrix_nxn<T, N>& operator+=(const Matrix_nxn<T, N>& m)
  {
    for (size_t i = 0; i < N; ++i)
      content[i] += m.content[i];
    return *this;
  }
  //----------------------------------------------------------------------------
  /**
  * Operator -=
  * @param m Matrix to subtract
  *
  * Complexity: n^2
  */
  Matrix_nxn<T, N>& operator-=(const Matrix_nxn<T, N>& m)
  {
    for (size_t i = 0; i < N; ++i)
      content[i] -= m.content[i];
    return *this;
  }
  //----------------------------------------------------------------------------
  /**
  * Operator *=
  * @param m Matrix this matrix is to multiplied with
  *
  * Complexity: n^3
  */
  Matrix_nxn<T, N>& operator*=(const Matrix_nxn<T, N>& m)
  {
    *this = *this * m;
    return *this;
  }
  //----------------------------------------------------------------------------
  /**
  * Operator /=
  * @param m Matrix this matrix is to divided by
  *
  * Complexity: n^3
  */
  Matrix_nxn<T, N>& operator/=(const Matrix_nxn<T, N>& m)
  {
    *this = *this / m;
    return *this;
  }
  //----------------------------------------------------------------------------
  /**
  * Transposes this matrix
  */
  Matrix_nxn<T, N> transpose()
  {
    //template<class T, size_t N>
    Matrix_nxn<T, N> res;
    for (size_t i = 0; i < N; ++i)
      for (size_t j = 0; j < N; ++j)
        res[i][j] = (*this)[j][i];
      return res;
  }
  //------------------------------------------------------------------------------
  /**
  * Inverts this matrix. Matrix must not be singular
  * @return Inverted matrix
  */
  Matrix_nxn<T, N> invert()
  {
    Matrix_nxn<T, N> left(*this);
    Matrix_nxn<T, N> right(Matrix_nxn::identity());
    Vector_n<int, N> ranking;

    int i;
    for (i = 0; i < (int)N; ++i) 
    {
       ranking[i] = i;
    }
    
    T zero = T();
    int r, r2, maxrow;
    for (r = 0; r < (int)(N-1); ++r)
    {
      // find highest value
      T maxval = left[ranking[r]][r];
      maxrow = r;
      if (maxval < zero)
        maxval = -maxval;
      for (r2 = r+1; r2 < (int)N; ++r2)
      {
        T val = left[ranking[r2]][r];
        if (val < zero)
          val = -val;
        if (val > maxval)
        {
          maxval = val;
          maxrow = r2;
        }
      }
      
      // swap rows
      int temp = ranking[r];
      ranking[r] = ranking[maxrow];
      ranking[maxrow] = temp;
      
      if (MVTools::isNearZero(left[ranking[r]][r]))
      {
        if (MVTools::isNearNegZero(left[ranking[r]][r]))
          throw MVException(MVException::DivByNegZero);
        else
          throw MVException(MVException::DivByPosZero);
      }
      
      for (r2 = r+1; r2 < (int)N; ++r2)
      {
        // calc factor for subtracting
        T factor = left[ranking[r2]][r] / left[ranking[r]][r];
        if (MVTools::isNearInf(factor))
        {
          if (MVTools::isNearPosInf(factor))
            throw MVException(MVException::PosInfValue);
          else
            throw MVException(MVException::NegInfValue);
        }
        
        // change left matrix
        left[ranking[r2]] -= factor*left[ranking[r]];
        
        // change right matrix
        right[ranking[r2]] -= factor*right[ranking[r]];
      }
    }
    
    // matrix has triangle form
    // bring to diagonal form
    for (r = (int)(N-1); r > 0; --r)
    {
      if (MVTools::isNearZero(left[ranking[r]][r]))
      {
        if (MVTools::isNearNegZero(left[ranking[r]][r]))
          throw MVException(MVException::DivByNegZero);
        else
          throw MVException(MVException::DivByPosZero);
      }
      for (r2 = r-1; r2 >= 0; --r2)
      {
        T factor = left[ranking[r2]][r] / left[ranking[r]][r];
        if (MVTools::isNearInf(factor))
        {
          if (MVTools::isNearPosInf(factor))
            throw MVException(MVException::PosInfValue);
          else
            throw MVException(MVException::NegInfValue);
        }
        
        // change left matrix
        left[ranking[r2]] -= factor*left[ranking[r]];
        
        // change right matrix
        right[ranking[r2]] -= factor*right[ranking[r]];
      }
    }
    
    // matrix has diagonal form
    // set entries of left matrix to 1 and apply multiplication to right
    Matrix_nxn<T, N> res;
    for (r = 0; r < (int)N; ++r)
    {
      res[r] = right[ranking[r]];
      
      if (MVTools::isNearZero(left[ranking[r]][r]))
      {
        if (MVTools::isNearNegZero(left[ranking[r]][r]))
          throw MVException(MVException::DivByNegZero);
        else
          throw MVException(MVException::DivByPosZero);
      }
      res[r] /= left[ranking[r]][r];
    }
    
    return res;
  }  
  //------------------------------------------------------------------------------
  /**
  * Inverts this matrix. Matrix must not be singular
  * @return Inverted matrix
  */
  Matrix_nxn<T, N> invert2()
  {
    Matrix_nxn<T, N> res;
    for (size_t i = 0; i < N; ++i)
    {
      Vector_n<T, N> e;
      e[i] = (T)1;
      res[i] = *this.solve(e);
    }
    return res.transpose();
  }
  //----------------------------------------------------------------------------
  /**
  * Solves the system A*x=b where A is the actual matrix
  * @param b Vector b
  * @return Solution x
  *
  * Complexity: n^3
  *
  */
  Vector_n<T, N> solve(Vector_n<T, N> b) const
  {
    // create copy of actual matrix
    Matrix_nxn<T, N> m(*this);
    
    // initialize ranking vector
    Vector_n<int, N> ranking;
    int i;
    for (i = 0; i < (int)N; ++i)
      ranking[i] = i;
    
    T z = T();
    int c;
    int r;
    for (c = 0; c < (int)N-1; ++c)
    {
      // find row containing highest value
      int maxRow = c;
      T maxValue = m[ranking[maxRow]][c];
      if (maxValue < z)
        maxValue = -maxValue;
      for (r = c+1; r < (int)N; ++r)
      {
        T value = m[ranking[r]][c];
        if (value < z)
          value = -value;
        if (value > maxValue)
        {
          maxRow = r;
          maxValue = value;
        }
      }
      
      // if maximum value zero --> matrix is singular
      if (MVTools::isNearZero(maxValue))
      {
        if (MVTools::isNearNegZero(maxValue))
          throw MVException(MVException::DivByNegZero);
        else
          throw MVException(MVException::DivByPosZero);
      }
      /*
      if (maxValue == z)
      return Vector_n<T, N>();
      */
      
      // swap rows in ranking
      int temp = ranking[c];
      ranking[c] = ranking[maxRow];
      ranking[maxRow] = temp;
      
      // process all following rows
      for (r = c+1; r < (int)N; ++r)
      {
        // calc factor for subtracting
        T factor = m[ranking[r]][c] / m[ranking[c]][c];
        if (MVTools::isNearInf(factor))
        {
          if (MVTools::isNearPosInf(factor))
            throw MVException(MVException::PosInfValue);
          else
            throw MVException(MVException::NegInfValue);
        }
        
        T sub;
        sub = factor*b[ranking[c]];
        if (MVTools::isNearInf(sub))
        {
          if (MVTools::isNearPosInf(sub))
            throw MVException(MVException::PosInfValue);
          else
            throw MVException(MVException::NegInfValue);
        }
        
        // change vector b
        b[ranking[r]] -= sub;
        
        // change matrix
        m[ranking[r]][c] = T();
        for (int c2 = c+1; c2 < (int)N; ++c2)
        {
          sub = factor*m[ranking[c]][c2];
          if (MVTools::isNearInf(sub))
          {
            if (MVTools::isNearPosInf(sub))
              throw MVException(MVException::PosInfValue);
            else
              throw MVException(MVException::NegInfValue);
          }
          m[ranking[r]][c2] -= sub;
        }
      }
    }
    
    // if last entry of matrix zero --> matrix is singular
    if (MVTools::isNearZero(m[ranking[N-1]][N-1]))
    {
      if (MVTools::isNearNegZero(m[ranking[N-1]][N-1]))
        throw MVException(MVException::DivByNegZero);
      else
        throw MVException(MVException::DivByPosZero);
    }
    /*
    if (m[ranking[N-1]][N-1] == z)
    return Vector_n<T, N>();
    */
    
    // matrix has triangle form
    // calculate solutions
    b[ranking[N-1]] /= m[ranking[N-1]][N-1];
    for (r = N-2; r >= 0; --r)
    {
      T sum = T();
      for (c = r+1; c < (int)N; ++c)
        sum += m[ranking[r]][c] * b[ranking[c]];
      if (MVTools::isNearInf(sum))
      {
        if (MVTools::isNearPosInf(sum))
          throw MVException(MVException::PosInfValue);
        else
          throw MVException(MVException::NegInfValue);
      }
      
      if (MVTools::isNearZero(m[ranking[r]][r]))
      {
        if (MVTools::isNearNegZero(m[ranking[r]][r]))
          throw MVException(MVException::DivByNegZero);
        else
          throw MVException(MVException::DivByPosZero);
      }
      b[ranking[r]] = (b[ranking[r]] - sum) / m[ranking[r]][r];
      
      if (MVTools::isNearInf(b[ranking[r]]))
      {
        if (MVTools::isNearPosInf(b[ranking[r]]))
          throw MVException(MVException::PosInfValue);
        else
          throw MVException(MVException::NegInfValue);
      }
    }
    
    // create vector with correct order
    Vector_n<T, N> x;
    for (r = 0; r < (int)N; ++r)
      x[r] = b[ranking[r]];
    
    return x;
  }
  //----------------------------------------------------------------------------
  /**
  * Returns determinant of this matrix
  * @return determinant
  *
  * Complexity: n^3
  */
  T det() const
  {
    // create copy of actual matrix
    Matrix_nxn<T, N> m(*this);
    
    // initialize ranking vector
    Vector_n<int, N> ranking;
    size_t i;
    for (i = 0; i < N; ++i)
      ranking[i] = i;
    
    T z = T();
    bool bPositive = true;
    int c;
    int r;
    for (c = 0; c < (int)(N-1); ++c)
    {
      // find row containing highest value
      int maxRow = c;
      T maxValue = m[ranking[maxRow]][c];
      if (maxValue < z)
        maxValue = -maxValue;
      for (r = c+1; r < (int)N; ++r)
      {
        T value = m[ranking[r]][c];
        if (value < z)
          value = -value;
        if (value > maxValue)
        {
          maxRow = r;
          maxValue = value;
        }
      }
      
      // if maximum value zero --> determinant is zero
      if (MVTools::isNearZero(maxValue))
        return z;
        /*if (maxValue == z)
      return z;*/
      
      // swap rows in ranking
      if (c != maxRow)
      {
        int temp = ranking[c];
        ranking[c] = ranking[maxRow];
        ranking[maxRow] = temp;
        bPositive = !bPositive;
      }
      
      // process all following rows
      for (r = c+1; r < (int)N; ++r)
      {
        // calc factor for subtracting
        T factor = m[ranking[r]][c] / m[ranking[c]][c];
        if (MVTools::isNearInf(factor))
        {
          if (MVTools::isNearPosInf(factor))
            throw MVException(MVException::PosInfValue);
          else
            throw MVException(MVException::NegInfValue);
        }
        
        // change matrix
        m[ranking[r]][c] = T();
        for (int c2 = c+1; c2 < (int)N; ++c2)
        {
          m[ranking[r]][c2] -= factor*m[ranking[c]][c2];
          T sub;
          sub = factor*m[ranking[c]][c2];
          if (MVTools::isNearInf(sub))
          {
            if (MVTools::isNearPosInf(sub))
              throw MVException(MVException::PosInfValue);
            else
              throw MVException(MVException::NegInfValue);
          }
        }
      }
    }
    
    // if last entry of matrix zero --> determinant is zero
    if (MVTools::isNearZero(m[ranking[N-1]][N-1]))
      return z;
      /*if (m[ranking[N-1]][N-1] == z)
    return z;*/
    
    // matrix has triangle form
    // calculate determinant
    T res = m[ranking[0]][0];
    for (r = 1; r < (int)N; ++r)
    {
      res *= m[ranking[r]][r];
      if (MVTools::isNearInf(res))
      {
        if (MVTools::isNearPosInf(res))
          throw MVException(MVException::PosInfValue);
        else
          throw MVException(MVException::NegInfValue);
      }
    }    
    
    if (!bPositive)
      res = -res;
    return res;
  }
  //----------------------------------------------------------------------------
  /**
  * Returns the identity matrix with same dimensions like this matrix
  * @return nxn identity matrix
  */
  static Matrix_nxn<T, N> identity()
  {
    Matrix_nxn<T, N> res;
    for (size_t i = 0; i < N; ++i)
      res[i][i] = (T)1;
    return res;
  }
  //----------------------------------------------------------------------------
private:
  /** The vectors holding the contents of the matrix */
#if defined(MATRIX_NXN_HEAP)
  Vector_n<T, N>* content;
#elif defined(MATRIX_NXN_STACK)
  Vector_n<T, N> content[N];
#endif
};
//------------------------------------------------------------------------------
/**
* Operator + for matrix additions
* @param m1 First matrix
* @param m2 Second matrix
* @return m1+m2
*
* Complexity: n^2
*/
template<class T, size_t N>
Matrix_nxn<T, N> operator+(const Matrix_nxn<T, N>& m1,
                           const Matrix_nxn<T, N>& m2)
{
  Matrix_nxn<T, N> res(m1);
  res += m2;
  return res;
}
//------------------------------------------------------------------------------
/**
* Operator - for matrix subtractions
* @param m1 First matrix
* @param m2 Second matrix
* @return m1-m2
*
* Complexity: n^2
*/
template<class T, size_t N>
Matrix_nxn<T, N> operator-(const Matrix_nxn<T, N>& m1,
                           const Matrix_nxn<T, N>& m2)
{
  Matrix_nxn<T, N> res(m1);
  res -= m2;
  return res;
}
//------------------------------------------------------------------------------
/**
* Operator * for matrix-matrix multiplication
* @param m1 First matrix
* @param m2 Second matrix
* @return m1*m2
*
* Complexity: n^3
*/
template<class T, size_t N>
Matrix_nxn<T, N> operator*(const Matrix_nxn<T, N>& m1,
                           const Matrix_nxn<T, N>& m2)
{
  Matrix_nxn<T, N> res;
  size_t i, j, k;
  for (i = 0; i < N; ++i)
  {
    for (j = 0; j < N; ++j)
    {
      for (k = 0; k < N; ++k)
      {
        res[i][j] += m1[i][k] * m2[k][j];
      }
      if (MVTools::isNearInf(res[i][j]))
      {
        if (MVTools::isNearPosInf(res[i][j]))
          throw MVException(MVException::PosInfValue);
        else
          throw MVException(MVException::NegInfValue);
      }
    }
  }
  return res;
}
//------------------------------------------------------------------------------
/**
* Operator * for matrix-vector multiplication
* @param m Matrix
* @param v Vector
* @return m*v
*
* Complexity: n^2
*/
template<class T, size_t N>
Vector_n<T, N> operator*(const Matrix_nxn<T, N>& m,
                         const Vector_n<T, N>& v)
{
  Vector_n<T, N> res;
  for (size_t i = 0; i < N; ++i)
  {
    for (size_t j = 0; j < N; ++j)
    {
      res[i] += m[i][j]*v[j];
    }
    if (MVTools::isNearInf(res[i]))
    {
      if (MVTools::isNearPosInf(res[i]))
        throw MVException(MVException::PosInfValue);
      else
        throw MVException(MVException::NegInfValue);
    }
  }
  return res;
}
//------------------------------------------------------------------------------
/**
* Divides first matrix by second matrix (multiplicates with inverse of
* second matrix).
* @param m1 First matrix
* @param m2 Second matrix
* @return m1 & m2
*
* Complexity: n^3
*/
template<class T, size_t N>
Matrix_nxn<T, N> operator/(const Matrix_nxn<T, N>& m1,
                           const Matrix_nxn<T, N>& m2)
{
  return m1*m2.invert();
}
//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
