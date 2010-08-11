/**
 * @file Pose2D.h
 * Contains class Pose2D
 *
 * @author <a href="mailto:martin.kallnik@gmx.de">Martin Kallnik</a>
 * @author Max Risler
 */

#ifndef __Pose2D_h__
#define __Pose2D_h__

#include "Common.h"
#include "Vector2.h"


/** representation for 2D Transformation and Position (Location + Orientation)*/
template <class T>
class Pose2T
{
public:

  /** Rotation as an angle*/
  T rotation;

  /** translation as an vector2*/
  Vector2<T> translation;

  /** noargs-constructor*/
  Pose2T():rotation(0),translation(0,0) {}

  /** constructor from rotation and translation
   * \param rotation rotation (double)
   * \param translation translation (Vector2)
   */
  Pose2T(const T rotation, const Vector2<T>& translation):rotation(rotation),translation(translation) {}

  /** constructor from rotation and translation
   * \param rot rotation (double)
   * \param x translation.x (double)
   * \param y translation.y (double)
   */
  Pose2T(const T rot, const T x, const T y):rotation(rot),translation(x,y)
  {}

  /** constructor from rotation
   * \param rotation rotation (double)
   */
  Pose2T(const T rotation):rotation(rotation),translation(0,0) {}
  
  /** constructor from translation
   * \param translation translation (Vector2)
   */
  Pose2T(const Vector2<T>& translation):rotation(0),translation(translation) {}

   /** constructor from translation
   * \param translation translation (Vector2)
   */
  template<class T2>
  Pose2T(const Vector2<T2>& translation):rotation(0),translation(translation.x,translation.y) {}

  /** constructor from two translation values
   * \param x translation x component
   * \param y translation y component
   */
  Pose2T(const T x, const T y):rotation(0),translation(x,y) {}

  /** get the Angle
   * @return Angle the Angle which defines the rotation
   */
  inline T getAngle() const {return rotation;}


  /** set rotation from Angle
   * @return the new Pose2D
   */
  inline T fromAngle(const T a) {rotation=a; return *this;}

  /** get the cos of the angle
   * @return the cos of the angle
   */
  inline double getCos() const {return cos(rotation);}

 /** get the sin of the angle
  * @return the sin of the angle
  */
  inline double getSin() const {return sin(rotation);}
  
  /** 
   * Multiplication of a Vector2 with this Pose2D
   * In fact we transform the point from the global coordinates to the local coordinates
   * represented by this pose (A*x = y), it holds
   * pose/(pose*x) = pose*(pose/x) = x
   *
   * \param point The Vector2 that will be multiplicated with this Pose2D
   * \return The resulting Vector2
   */
  Vector2<T> operator*(const Vector2<T>& point) const
  {  
    double s=sin(rotation);
    double c=cos(rotation);
    return (Vector2<T>(point.x*c-point.y*s , point.x*s+point.y*c) + translation);
  }

  /** 
   * Division of a Vector2 with this Pose2D
   * In fact we transform the point from the local coordinates represented by this pose
   * to the global coordinates to, i.e. we solve the equation A*x = y (see * Operator).
   * The Operation pose/x is equivalent to pose.invert()*x, i.e., it holds
   * pose/(pose*x) = pose*(pose/x) = x
   *
   * \param point The Vector2 that will be divided with this Pose2D
   * \return The resulting Vector2
   */
  Vector2<T> operator/(const Vector2<T>& point) const
  {
    return ((point - translation).rotate(-rotation));
  }

  /** Comparison of another pose with this one.
   * \param other The other pose that will be compared to this one
   * \return Whether the two poses are equal.
   */
  bool operator==(const Pose2T& other) const
  {
    return ((translation==other.translation)&&(rotation==other.rotation));
  }

  /** Comparison of another pose with this one.
   * \param other The other pose that will be compared to this one
   * \return Whether the two poses are unequal.
   */
  bool operator!=(const Pose2T& other) const
  {return !(*this == other);}

  /** Concatenation of this pose with another pose.
   * \param other The other pose that will be concatenated to this one.
   * \return A reference to this pose after concatenation.
   */
  Pose2T& operator+=(const Pose2T& other)
  {
    translation = *this * other.translation;
    rotation += other.rotation;
    rotation = Math::normalize(rotation);
    return *this;
  }

  /** A concatenation of this pose and another pose.
   * \param other The other pose that will be concatenated to this one.
   * \return The resulting pose.
   */
  Pose2T operator+(const Pose2T& other) const
  {return Pose2T(*this) += other;}

  /** Subtracts a difference pose from this one to get the source pose. So if A+B=C is the addition/concatenation, this calculates C-B=A.
   * \param diff The difference Pose2D that shall be subtracted.
   * \return The resulting source pose. Adding diff to it again would give the this pose back.
   */
  Pose2T minusDiff(const Pose2T& diff) const
  {
    double rot=rotation-diff.rotation;
    double s=sin(rot);
    double c=cos(rot);
    return Pose2T(
      rot,
      translation.x - c*diff.translation.x + s*diff.translation.y,
      translation.y - c*diff.translation.y - s*diff.translation.x);
  }

  /** Difference of this pose relative to another pose. So if A+B=C is the addition/concatenation, this calculates C-A=B.
   * \param other The other pose that will be used as origin for the new pose.
   * \return A reference to this pose after calculating the difference.
   */
  Pose2T& operator-=(const Pose2T& other)
  {
    translation -= other.translation;
    Pose2T p(-other.rotation);
    return *this = p + *this;
  }

  /** Difference of this pose relative to another pose.
   * \param other The other pose that will be used as origin for the new pose.
   * \return The resulting pose.
   */
  Pose2T operator-(const Pose2T& other) const
  {return Pose2T(*this) -= other;}

  /** Concatenation of this pose with another pose
   * \param other The other pose that will be concatenated to this one.
   * \return A reference to this pose after concatenation
   */
  Pose2T& conc(const Pose2T& other)
  {return *this += other;}

  /** Translate this pose by a translation vector
   * \param trans Vector to translate with
   * \return A reference to this pose after translation
   */
  Pose2T& translate(const Vector2<T>& trans)
  {
    translation = *this * trans;
    return *this;
  }

  /** Translate this pose by a translation vector
   * \param x x component of vector to translate with
   * \param y y component of vector to translate with
   * \return A reference to this pose after translation
   */
  Pose2T& translate(const T x, const T y)
  {
    translation = *this * Vector2<double>(x,y);
    return *this;
  }


  /** Rotate this pose by a rotation
   * \param angle Angle to rotate.
   * \return A reference to this pose after rotation
   */
  Pose2T& rotate(const T angle)
  {
    rotation += angle;
    rotation = Math::normalizeAngle(rotation);
    return *this;
  }


  /** 
   * Calculates the inverse transformation from the current pose
   * @return The inverse transformation pose.
   */
  Pose2T invert() const
  {
    const T& invRotation = -rotation;
    return Pose2T(invRotation, (Vector2<T>() - translation).rotate(invRotation));
  }

  /**
   * The function creates a random pose.
   * @param x The range for x-values of the pose.
   * @param y The range for y-values of the pose.
   * @param angle The range for the rotation of the pose.
   */
  /*
  static Pose2D random(const Range<double>& x,
                       const Range<double>& y,
                       const Range<double>& angle)
  { // angle should even work in wrap around case!  
    return Pose2D(::randomDouble() * (angle.max - angle.min) + angle.min,
                  Vector2<double>(::randomDouble() * (x.max - x.min) + x.min,
                                  ::randomDouble() * (y.max - y.min) + y.min));
  }
  */
};

template<class T>
std::ostream & operator <<(std::ostream& ost, const Pose2T<T>& v)
{
  ost << v.rotation << " " << v.translation;
  return ost;
}

typedef Pose2T<double> Pose2D;

#endif // __Pose2D_h__
