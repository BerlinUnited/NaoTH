/**
* @file InertiaSensorFilter.h
* Declaration of module InertiaSensorFilter.
* @author Colin Graf (BH-2011)
* @author Heinrich Mellmann (NaoTH-2012)
* copied from BH-2011
*/

#ifndef _InertiaSensorFilter_H_
#define _InertiaSensorFilter_H_

#include "Representations/Infrastructure/FrameInfo.h"
#include "Tools/Math/Pose3D.h"
#include "Tools/Math/RotationMatrix.h"
#include "Motion/MotionBlackBoard.h"


template <class V = double> class Matrix2x3;
template <class V = double> class Matrix3x2;

/**
* @class Matrix3x2
* An incomplete implementation of Matrix3x2.
*/
template <class V> class Matrix3x2
{
public:
  Vector3<V> c0; /**< The first column of the matrix. */
  Vector3<V> c1; /**< The second column of the matrix. */

  /** Default constructor. */
  Matrix3x2<V>() {}

  /**
  * Constructor; initializes each column of the matrix.
  * @param c0 The first column of the matrix.
  * @param c1 The second column of the matrix.
  */
  Matrix3x2<V>(const Vector3<V>& c0, const Vector3<V>& c1) :
    c0(c0), c1(c1) {}

  /**
  * Adds another matrix to this one (component by component).
  * @param other The matrix to add.
  * @return A reference this object after the calculation.
  */
  Matrix3x2<V>& operator+=(const Matrix3x2<V>& other)
  {
    c0 += other.c0;
    c1 += other.c1;
    return *this;
  }

  /**
  * Computes the sum of two matrices
  * @param other Another matrix
  * @return The sum
  */
  Matrix3x2<V> operator+(const Matrix3x2<V>& other) const
  {
    return Matrix3x2<V>(*this) += other;
  }

  /**
  * Division of this matrix by a factor.
  * @param factor The factor this matrix is divided by
  * @return A reference to this object after the calculation.
  */
  Matrix3x2<V>& operator/=(const V& factor)
  {
    c0 /= factor;
    c1 /= factor;
    return *this;
  }

  /**
  * Multiplication of this matrix by a factor.
  * @param factor The factor this matrix is multiplied by
  * @return A reference to this object after the calculation.
  */
  Matrix3x2<V>& operator*=(const V& factor)
  {
    c0 *= factor;
    c1 *= factor;
    return *this;
  }

  /**
  * Transposes the matrix.
  * @return A new object containing transposed matrix
  */
  Matrix2x3<V> transpose()
  {
    return Matrix2x3<V>(Vector2<V>(c0.x, c1.x), Vector2<V>(c0.y, c1.y), Vector2<V>(c0.z, c1.z));
  }
};

/**
* @class Matrix2x3
* An incomplete implementation of Matrix2x3.
*/
template <class V> class Matrix2x3
{
public:
  Vector2<V> c0; /**< The first column of the matrix. */
  Vector2<V> c1; /**< The second column of the matrix. */
  Vector2<V> c2; /**< The third column of the matrix. */

  /** Default constructor. */
  Matrix2x3<V>() {}

  /**
  * Constructor; initializes each column of the matrix.
  * @param c0 The first column of the matrix.
  * @param c1 The second column of the matrix.
  * @param c2 The third column of the matrix.
  */
  Matrix2x3<V>(const Vector2<V>& c0, const Vector2<V>& c1, const Vector2<V>& c2) :
    c0(c0), c1(c1), c2(c2) {}

  /**
  * Multiplication of this matrix by vector.
  * @param vector The vector this one is multiplied by
  * @return A new vector containing the result of the calculation.
  */
  Vector2<V> operator*(const Vector3<V>& vector) const
  {
    return c0 * vector.x + c1 * vector.y + c2 * vector.z;
  }

  /**
  * Multiplication of this matrix by a 3x3 matrix.
  * @param matrix The other matrix this one is multiplied by .
  * @return A new matrix containing the result of the calculation.
  */
  Matrix2x3<V> operator*(const Matrix3x3<V>& matrix) const
  {
    return Matrix2x3<V>(*this * matrix[0], *this * matrix[1], *this * matrix[2]);
  }

  /**
  * Multiplication of this matrix by a 3x2 matrix.
  * @param matrix The other matrix this one is multiplied by .
  * @return A new matrix containing the result of the calculation.
  */
  Matrix2x2<V> operator*(const Matrix3x2<V>& matrix) const
  {
    return Matrix2x2<V>(*this * matrix.c0, *this * matrix.c1);
  }

  /**
  * Transposes the matrix.
  * @return A new object containing transposed matrix
  */
  Matrix3x2<V> transpose()
  {
    return Matrix2x3<V>(Vector2<V>(c0.x, c1.x, c2.x), Vector2<V>(c0.y, c1.y, c2.y));
  }
};



/**
* @class InertiaSensorFilter
* A module for estimating velocity and orientation of the torso.
*/
class InertiaSensorFilter
{
public:
  /** Default constructor. */
  InertiaSensorFilter(const MotionBlackBoard& theBlackBoard, InertialModel& theInertialModel);

  /**
  * Updates the OrientationData representation.
  * @param orientationData The orientation data representation which is updated by this module.
  */
  Vector2<double> update();

  /** */
  InertialModel& theInertialModel;

  const MotionBlackBoard& theBlackBoard;

private:
  /**
  * A collection of parameters for this module.
  */
  class Parameters
  {
  public:
    Vector2<double> processNoise; /**< The standard deviation of the process. */
    Vector3<double> accNoise; /**< The standard deviation of the inertia sensor. */
    Vector2<double> calculatedAccLimit; /**< Use a calculated angle up to this angle (in rad). (We use the acceleration sensors otherwise.) */

    Matrix2x2<double> processCov; /**< The covariance matrix for process noise. */
    Matrix3x3<double> sensorCov; /**< The covariance matrix for sensor noise. */

    /** Default constructor. */
    Parameters() {}

    /**
    * Calculates parameter dependent constants used to speed up some calculations.
    */
    void calculateConstants();
  };


  /**
  * Represents the state to be estimated.
  */
  template <class V = double> class State
  {
  public:
    RotationMatrix rotation; /** The rotation of the torso. */

    /** Default constructor. */
    State() {}

    /**
    * Adds some world rotation given as angle axis.
    * @param value The flat vector to add.
    * @return A new object after the calculation.
    */
    State operator+(const Vector2<V>& value) const;

    /**
    * Adds some world rotation given as angle axis.
    * @param value The flat vector to add.
    * @return A reference this object after the calculation.
    */
    State& operator+=(const Vector2<V>& value);

    /**
    * Calculates a flat difference vector of two states.
    * @return The difference.
    */
    Vector2<V> operator-(const State& other) const;
  };

  Parameters p; /**< The parameters of this module. */

  unsigned int lastTime; /**< The frame time of the previous iteration. */
  Vector2<double> safeRawAngle; /**< The last not corrupted angle from aldebarans angle estimation algorithm. */

  State<> x; /**< The estimate */
  Matrix2x2<double> cov; /**< The covariance of the estimate. */
  Matrix2x2<double> l; /**< The last caculated cholesky decomposition of \c cov. */
  State<> sigmaPoints[5]; /**< The last calculated sigma points. */

  Vector3<double> sigmaReadings[5]; /**< The expected sensor values at the sigma points. */
  Vector3<double> readingMean; /**< The mean of the expected sensor values which was determined by using the sigma velocities. */
  Matrix3x3<double> readingsCov;
  Matrix3x2<double> readingsSigmaPointsCov;

  /**
  * Restores the initial state.
  */
  void reset();

  void predict(const RotationMatrix& rotationOffset);
  void readingUpdate(const Vector3<double>& reading);

  void cholOfCov();
  void generateSigmaPoints();
  void meanOfSigmaPoints();
  void covOfSigmaPoints();

  void readingModel(const State<>& sigmaPoint, Vector3<double>& reading);
  void meanOfSigmaReadings();
  void covOfSigmaReadingsAndSigmaPoints();
  void covOfSigmaReadings();

  inline Matrix2x2<double> tensor(const Vector2<double>& a, const Vector2<double>& b) const
  {
    return Matrix2x2<double>(a * b.x, a * b.y);
  }

  inline Matrix2x2<double> tensor(const Vector2<double>& a) const
  {
    return Matrix2x2<double>(a * a.x, a * a.y);
  }

  inline Matrix3x2<double> tensor(const Vector3<double>& a, const Vector2<double>& b)
  {
    return Matrix3x2<double>(a * b.x, a * b.y);
  }

  inline Matrix3x3<double> tensor(const Vector3<double>& a)
  {
    return Matrix3x3<double>(a * a.x, a * a.y, a * a.z);
  }
};

#endif //_InertiaSensorFilter_H_
