
#ifndef Matrix_mxn_H
#define Matrix_mxn_H

#include "MVTools.h"
#include "Vector_n.h"
#include <math.h>
#include <vector>

using namespace std;

namespace Math
{

  template <class T> class Matrix_mxn
  {
    private:
      vector<T>* content;

    public:
      unsigned int m;
      unsigned int n;

      //----------------------------------------------------------------------------
      /**
      * Standard constructor
      *
      * Initializes the matrix with values supplied by the standard constructor
      * of T.
      *
      * Complexity: n*m
      */
      Matrix_mxn(unsigned int M, unsigned int N)
      {
        m = M;
        n = N;
        content = NULL;
        if(M > 0 && N > 0)
        {
          content = new vector<T>[m];
          vector<T>* init = content;
          for (unsigned int i = 0; i < m; ++i, ++init)
          {
            *init = vector<T>(n,0);
          }
        }
      }

      //----------------------------------------------------------------------------
      /**
      * Constructor that initializes the matrix with the values in the passed array
      * @param v Array with initialization values
      *
      * Complexity: n*m
      */
      Matrix_mxn(unsigned int M, unsigned int N, const T* v)
      {
        m = M;
        n = N;
        content = NULL;
        if(M > 0 && N > 0 && v != NULL)
        {
          content = new vector<T>[m];
          vector<T>* init = content;
          for (unsigned int i = 0; i < m; ++i, ++init, v += n)
          {
            vector<T> vec = vector<T>(n, 0);
            for(unsigned int j = 0; j < n; j++)
            {
              vec[j] = v[j];
            }
            *init = vec;
          }
        }
      }

      //----------------------------------------------------------------------------
      /**
      * Copy constructor
      * @param m Matrix to copy
      *
      * Complexity: n*m
      */
      Matrix_mxn(const Matrix_mxn<T>& matrix)
      {
        m = matrix.m;
        n = matrix.n;
        content = NULL;
        if(m > 0 && n > 0)
        {
          content = new vector<T>[m];
          vector<T>* init = content;
          for (unsigned int i = 0; i < m; ++i, ++init)
          {
            vector<T> vec = vector<T>(n, 0);
            for(unsigned int j = 0; j < n; j++)
            {
              vec[j] = matrix.content[i][j];
            }
            *init = vec;
          }
        }
      }

      //----------------------------------------------------------------------------
      /**
      * Destructor
      *
      * Complexity: n*m
      */
      ~Matrix_mxn()
      {
        if(content != NULL)
        {
          delete [] content;
        }
      }

      //----------------------------------------------------------------------------
      /**
      * Copy operator
      * @param m Matrix to copy
      *
      * Complexity: n*m
      */
      Matrix_mxn<T>& operator=(const Matrix_mxn<T>& matrix)
      {
        assert(matrix.n == n && matrix.m == m);
        for (unsigned int i = 0; i < m; ++i)
        {
          for (unsigned int j = 0; j < n; ++j)
          {
            content[i][j] = matrix.content[i][j];
          }
        }
        return  *this;
      }

      //----------------------------------------------------------------------------
      /**
      * Row vector access operator
      * @param i Index of row vector to access (first row vector has index 0)
      * @return Reference to row vector
      *
      * Complexity: 1
      */
      vector<T>& operator[](unsigned int i)
      {
        assert(m > 0 && n> 0);
        return content[i];
      }

      //----------------------------------------------------------------------------
      /**
      * cell access operator
      * @param i Index of row vector to access (first row vector has index 0)
      * @return Reference to row vector
      *
      * Complexity: 1
      */
      T& operator()(unsigned int i, unsigned int j)
      {
        assert(m > 0 && n> 0);
        return content[i][j];
      }

      T& operator()(unsigned int i, unsigned int j) const
      {
        assert(m > 0 && n> 0);
        return content[i][j];
      }

      //----------------------------------------------------------------------------
      /**
      * Operator +=
      * @param m Matrix to add
      *
      * Complexity: n*m
      */
      Matrix_mxn<T>& operator+=(const Matrix_mxn<T>& matrix)
      {
        assert(m == matrix.m && n == matrix.n);
        for (unsigned int i = 0; i < m; ++i)
        {
          for (unsigned int j = 0; j < n; ++j)
          {
            content[i][j] += matrix.content[i][j];
          }
        }
        return *this;
      }

      //----------------------------------------------------------------------------
      /**
      * Operator +
      * @param m Matrix to add
      *
      * Complexity: n*m
      */
      Matrix_mxn<T> operator+(const Matrix_mxn<T>& matrix)
      {
        assert(m == matrix.m && n == matrix.n);
        Matrix_mxn<T> M(matrix);
        for (unsigned int i = 0; i < m; ++i)
        {
          for (unsigned int j = 0; j < n; ++j)
          {
            M[i][j] = content[i][j] + matrix.content[i][j];
          }
        }
        return M;
      }

      //----------------------------------------------------------------------------
      /**
      * Operator -=
      * @param m Matrix to subtract
      *
      * Complexity: n*m
      */
      Matrix_mxn<T>& operator-=(const Matrix_mxn<T>& matrix)
      {
        assert(m == matrix.m && n == matrix.n);
        for (unsigned int i = 0; i < m; ++i)
        {
          for (unsigned int j = 0; j < n; ++j)
          {
            content[i][j] -= matrix.content[i][j];
          }
        }
        return *this;
      }


      //----------------------------------------------------------------------------
      /**
      * Operator -
      * @param m Matrix to sub
      *
      * Complexity: n*m
      */
      Matrix_mxn<T> operator-(const Matrix_mxn<T>& matrix)
      {
        assert(m == matrix.m && n == matrix.n);
        Matrix_mxn<T> M(matrix);
        for (unsigned int i = 0; i < m; ++i)
        {
          for (unsigned int j = 0; j < n; ++j)
          {
            M[i][j] = content[i][j] - matrix.content[i][j];
          }
        }
        return M;
      }

      //----------------------------------------------------------------------------
      /**
      * Operator *=
      * @param s Scalar to multiply
      *
      * Complexity: n*m
      */
      Matrix_mxn<T>& operator*=(T s)
      {
        for (unsigned int i = 0; i < m; ++i)
        {
          for (unsigned int j = 0; j < n; ++j)
          {
            content[i][j] *= s;
          }
        }
        return *this;
      }

      //----------------------------------------------------------------------------
      /**
      * Operator *
      * @param s Scalar to multiply
      *
      * Complexity: n*m
      */
      Matrix_mxn<T> operator*(T s)
      {
        Matrix_mxn<T> M(m, n);
        for (unsigned int i = 0; i < m; ++i)
        {
          for (unsigned int j = 0; j < n; ++j)
          {
            M[i][j] = content[i][j] * s;
          }
        }
        return M;
      }

      //----------------------------------------------------------------------------
      /**
      * Operator *
      * @param matrix Matrix this matrix is to multiplied with
      *
      * Complexity: n*m
      */
      Matrix_mxn<T> operator*(const Matrix_mxn<T>& matrix)
      {
        assert(matrix.m == n);
        Matrix_mxn<T> M(m, matrix.n);
        for (unsigned int i = 0; i < m; ++i)
        {
          for (unsigned int j = 0; j < matrix.n; ++j)
          {
             T value = 0;
            for (unsigned int k = 0; k < n; k++)
            {
              value += content[i][k] * matrix.content[k][j];
            }
            M[i][j] = value;
          }
        }
        return M;
      }



//      //----------------------------------------------------------------------------
//      /**
//      * Operator *=
//      * @param m Matrix this matrix is to multiplied with
//      *
//      * Complexity: n^3
//      */
//      Matrix_mxn<T>& operator*=(const Matrix_mxn<T>& matrix)
//      {
////        *this = *this * m;
////        return *this;
//        return (*this) * matrix;
//      }
//
//  //    //----------------------------------------------------------------------------
//  //    /**
//  //    * Operator /=
//  //    * @param m Matrix this matrix is to divided by
//  //    *
//  //    * Complexity: n^3
//  //    */
//  //    Matrix_mxn<T, N>& operator/=(const Matrix_mxn<T, N>& m)
//  //    {
//  //      *this = *this / m;
//  //      return *this;
//  //    }
//
      //----------------------------------------------------------------------------
      /**
      * Transposes this matrix
      */
      Matrix_mxn<T> transpose()
      {
        Matrix_mxn<T> M(n, m);
        for (unsigned int i = 0; i < m; ++i)
        {
          for (unsigned int j = 0; j < n; ++j)
          {
            M[j][i] = content[i][j];
          }
        }
        return M;
      }

      Matrix_mxn<T> subMatrix(unsigned int ii,unsigned int jj)
      {
        assert(m > 0 && n > 0);
        Matrix_mxn<T> M(m - 1, n - 1);
        for (unsigned int i = 0; i < m; i++)
        {
          for (unsigned int j = 0; j < n; j++)
          {
            if (i < ii)
            {
              if (j < jj)
              {
                M[i][j] = content[i][j];
              }
              if (j > jj)
              {
                M[i][j - 1] = content[i][j];
              }
            }
            if (i > ii)
            {
              if (j < jj)
              {
                M[i - 1][j] = content[i][j];
              }
              if (j > jj)
              {
                M[i - 1][j - 1] = content[i][j];
              }
            }
          }
        }
        return M;
      }

      T det()
      {
        if (n == m)
        {
          if (n == 2)
          {
            return content[0][0] * content[1][1] - content[1][0] * content[0][1];
          }
          else if (n == 3)
          {
            return
            (
              ( content[0][0] * content[1][1] * content[2][2] + content[0][1] * content[1][2] * content[2][0] + content[0][2] * content[1][0] * content[2][1] )
              - ( content[2][0] * content[1][1] * content[0][2] + content[2][1] * content[1][2] * content[0][0] + content[2][2] * content[1][0] * content[0][1] )
            );
          }
          else
          {
            T d = 0;
            for (unsigned int col = 0; col < n; col++)
            {
              d += content[0][col] * adjoint(0, col);
            }
            return d;
          }
        }
        else
        {
          return 0;
        }
      }

      T adjoint(unsigned int ii, unsigned int jj)
      {
          Matrix_mxn<T> M = subMatrix(ii, jj);
          T d = M.det();
          /*(-1)^(i+j) i,j of natural numbers without 0*/
          return pow(static_cast<T>(-1), static_cast<T>(ii + jj + 2)) * d;
      }

      Matrix_mxn<T> invert()
      {
        assert(n == m);
        Matrix_mxn<T> M(n, m);
        T d = 0;
        if (n == 2)
        {
          T d = content[0][0] * content[1][1] - content[0][1] * content[1][0];
          if(d != 0)
          {
            T scalar = 1 / ( d );
            M[0][0] = scalar * content[1][1];
            M[0][1] = -scalar * content[0][1];
            M[1][0] = -scalar * content[1][0];
            M[1][1] = scalar * content[0][0];
          }
        }
        else
        {
          d = det();
          assert(d != 0);
          for (unsigned int i = 0; i < n; i++)
          {
            for (unsigned int j = 0; j < m; j++)
            {
              M[j][i] = adjoint(i, j) / d;
            }
          }
        }
        return M;
      }

      void print()
      {
        cout << "Matrix (" << m << "x" << n << ")" << endl;
        for (unsigned int i = 0; i < m; i++)
        {
          for (unsigned int j = 0; j < n; j++)
          {
            cout << "\t" << content[i][j];
          }
          cout << "\n";
        }
        cout << "\n";
        cout << " Det = " << det();
        cout << "\n";
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
    Matrix_mxn<T> solve(Matrix_mxn<T> b) const
    {
      assert(m == n && n > 1 && b.n == 1);
      // create copy of actual matrix
      Matrix_mxn<T> matrix(*this);
  
      // initialize ranking vector
      Matrix_mxn<int> ranking(1, n);
      unsigned int i;
      for (i = 0; i < n; i++)
      {
        ranking[0][i] = i;
      }
      T z = T();
      unsigned int c;
      int r;
      for (c = 0; c < n - 1; c++)
      {
        // find row containing highest value
        int maxRow = c;
        T maxValue = matrix[ranking[0][maxRow]][c];
        if (maxValue < z)
        {
          maxValue = -maxValue;
        }
        for (r = c + 1; r < ((int) n); ++r)
        {
          T value = matrix[ranking[0][r]][c];
          if (value < z)
          {
            value = -value;
          }
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
          {
            throw MVException(MVException::DivByNegZero);
          }
          else
          {
            throw MVException(MVException::DivByPosZero);
          }
        }
        /*
        if (maxValue == z)
        return Vector_n<T, N>();
        */
  
        // swap rows in ranking
        int temp = ranking[0][c];
        ranking[0][c] = ranking[0][maxRow];
        ranking[0][maxRow] = temp;
  
        // process all following rows
        for (r = c + 1; r < ((int) n); r++)
        {
          // calc factor for subtracting
          T factor = matrix[ranking[0][r]][c] / matrix[ranking[0][c]][c];
          if (MVTools::isNearInf(factor))
          {
            if (MVTools::isNearPosInf(factor))
            {
              throw MVException(MVException::PosInfValue);
            }
            else
            {
              throw MVException(MVException::NegInfValue);
            }
          }
  
          T sub;
          sub = factor * b[ranking[0][c]][0];
          if (MVTools::isNearInf(sub))
          {
            if (MVTools::isNearPosInf(sub))
            {
              throw MVException(MVException::PosInfValue);
            }
            else
            {
              throw MVException(MVException::NegInfValue);
            }
          }
  
          // change vector b
          b[ranking[0][r]][0] -= sub;
  
          // change matrix
          matrix[ranking[0][r]][c] = T();
          for (unsigned int c2 = c + 1; c2 < n; c2++)
          {
            sub = factor * matrix[ranking[0][c]][c2];
            if (MVTools::isNearInf(sub))
            {
              if (MVTools::isNearPosInf(sub))
              {
                throw MVException(MVException::PosInfValue);
              }
              else
              {
                throw MVException(MVException::NegInfValue);
              }
            }
            matrix[ranking[0][r]][c2] -= sub;
          }
        }
      }
  
      // if last entry of matrix zero --> matrix is singular
      if (MVTools::isNearZero(matrix[ranking[0][n - 1]][n - 1]))
      {
        if (MVTools::isNearNegZero(matrix[ranking[0][n - 1]][n - 1]))
        {
          throw MVException(MVException::DivByNegZero);
        }
        else
        {
          throw MVException(MVException::DivByPosZero);
        }
      }
      /*
      if (matrix[ranking[0][n - 1]][n - 1] == z)
      {
        return Vector_n<T, N>();
      }
      */
  
      // matrix has triangle form
      // calculate solutions
      b[ranking[0][n - 1]][0] /= matrix[ranking[0][n - 1]][n - 1];
      for (r = n - 2; r >= 0; r--)
      {
        T sum = T();
        for (c = r + 1; c < n; c++)
        {
          sum += matrix[ranking[0][r]][c] * b[ranking[0][c]][0];
        }
        if (MVTools::isNearInf(sum))
        {
          if (MVTools::isNearPosInf(sum))
          {
            throw MVException(MVException::PosInfValue);
          }
          else
          {
            throw MVException(MVException::NegInfValue);
          }
        }
  
        if (MVTools::isNearZero(matrix[ranking[0][r]][r]))
        {
          if (MVTools::isNearNegZero(matrix[ranking[0][r]][r]))
          {
            throw MVException(MVException::DivByNegZero);
          }
          else
          {
            throw MVException(MVException::DivByPosZero);
          }
        }
        b[ranking[0][r]][0] = (b[ranking[0][r]][0] - sum) / matrix[ranking[0][r]][r];
  
        if (MVTools::isNearInf(b[ranking[0][r]][0]))
        {
          if (MVTools::isNearPosInf(b[ranking[0][r]][0]))
          {
            throw MVException(MVException::PosInfValue);
          }
          else
          {
            throw MVException(MVException::NegInfValue);
          }
        }
      }
  
      // create vector with correct order
      Matrix_mxn<T> x(n, 1);
      for (r = 0; r < ((int) n); r++)
      {
        x[r][0] = b[ranking[0][r]][0];
      }
      return x;
    }

    //----------------------------------------------------------------------------
    /**
    * Returns the identity matrix with same dimensions like this matrix
    * @return nxn identity matrix
    */
    Matrix_mxn<T> identity()
    {
      assert(m == n);
      Matrix_mxn<T> res(m, n);
      for (unsigned int i = 0; i < n; i++)
      {
        res[i][i] = (T) 1;
      }
      return res;
    }

  };


}
#endif

