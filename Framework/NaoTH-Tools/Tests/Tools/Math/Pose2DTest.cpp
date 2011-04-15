#include "Tools/Math/Pose2D.h"

#include <gtest/gtest.h>

using std::string;

TEST(Pose2D,GetAngle) {
    Pose2D nullPose(0,0,0);
    double n = nullPose.getAngle();
    EXPECT_EQ(0, n);    
}
TEST(Pose2D,GetCos) {
    Pose2D pose(Math::pi_2,0,0);
    //Comparing Floats: http://www.cygnus-software.com/papers/comparingfloats/comparingfloats.htm
    EXPECT_NEAR(0, pose.getCos(),0.0001);    
}
TEST(Pose2D,GetSin) {
    Pose2D pose(Math::pi,0,0);
    EXPECT_NEAR(0, pose.getSin(),0.0001);    
}
TEST(Pose2D,MultiplyWithVector2) {
  /**
   * Multiplication of a Vector2 with this Pose2D
   * In fact we transform the point from the global coordinates to the local coordinates
   * represented by this pose (A*x = y), it holds
   * pose/(pose*x) = pose*(pose/x) = x
   *
   * \param point The Vector2 that will be multiplicated with this Pose2D
   * \return The resulting Vector2
   */
 /* Vector2<double> operator*(const Vector2<double>& point) const
  {
    double s=sin(rotation);
    double c=cos(rotation);
    return (Vector2<double>(point.x*c-point.y*s , point.x*s+point.y*c) + translation);
  }*/
}
TEST(Pose2D,DivisionByVector2) {

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
/*  Vector2<double> operator/(const Vector2<double>& point) const
  {
    return ((point - translation).rotate(-rotation));
  }*/
}
TEST(Pose2D,ComparePoses) {
  /** Comparison of another pose with this one.
   * \param other The other pose that will be compared to this one
   * \return Whether the two poses are equal.
   */
/*  bool operator==(const Pose2D& other) const
  {
    return ((translation==other.translation)&&(rotation==other.rotation));
  }
*/
  }
TEST(Pose2D,UnequalComparePoses) {

  /** Comparison of another pose with this one.
   * \param other The other pose that will be compared to this one
   * \return Whether the two poses are unequal.
   */
  /*bool operator!=(const Pose2D& other) const
  {return !(*this == other);}
*/
  }
TEST(Pose2D,ConcatenatePosesPlusEqual) {
  /** Concatenation of this pose with another pose.
   * \param other The other pose that will be concatenated to this one.
   * \return A reference to this pose after concatenation.
   */
/*  Pose2D& operator+=(const Pose2D& other)
  {
    translation = *this * other.translation;
    rotation += other.rotation;
    rotation = Math::normalize(rotation);
    return *this;
  }
*/
  }
TEST(Pose2D,ConcatenatePosesPlus) {

  /** A concatenation of this pose and another pose.
   * \param other The other pose that will be concatenated to this one.
   * \return The resulting pose.
   */
/*  Pose2D operator+(const Pose2D& other) const
  {return Pose2D(*this) += other;}
*/
  }
TEST(Pose2D,MinusDiffPose) {

  /** Subtracts a difference pose from this one to get the source pose. So if A+B=C is the addition/concatenation, this calculates C-B=A.
   * \param diff The difference Pose2D that shall be subtracted.
   * \return The resulting source pose. Adding diff to it again would give the this pose back.
   */
/*  Pose2D minusDiff(const Pose2D& diff) const
  {
    double rot=rotation-diff.rotation;
    double s=sin(rot);
    double c=cos(rot);
    return Pose2D(
      rot,
      translation.x - c*diff.translation.x + s*diff.translation.y,
      translation.y - c*diff.translation.y - s*diff.translation.x);
  }
*/
  }
TEST(Pose2D,MinusDiffPoseEqual) {

  /** Difference of this pose relative to another pose. So if A+B=C is the addition/concatenation, this calculates C-A=B.
   * \param other The other pose that will be used as origin for the new pose.
   * \return A reference to this pose after calculating the difference.
   */
/*  Pose2D& operator-=(const Pose2D& other)
  {
    translation -= other.translation;
    Pose2D p(-other.rotation);
    return *this = p + *this;
  }
*/
  }
TEST(Pose2D,MinusOperatorDiffPose) {
  /** Difference of this pose relative to another pose.
   * \param other The other pose that will be used as origin for the new pose.
   * \return The resulting pose.
   */
/*  Pose2D operator-(const Pose2D& other) const
  {return Pose2D(*this) -= other;}
*/
  }
TEST(Pose2D,ConcatenatePose) {

  /** Concatenation of this pose with another pose
   * \param other The other pose that will be concatenated to this one.
   * \return A reference to this pose after concatenation
   */
/*  Pose2D& conc(const Pose2D& other)
  {return *this += other;}
*/
  }
TEST(Pose2D,TranslatePoseByVector) {
  /** Translate this pose by a translation vector
   * \param trans Vector to translate with
   * \return A reference to this pose after translation
   */
/*  Pose2D& translate(const Vector2<double>& trans)
  {
    translation = *this * trans;
    return *this;
  }
*/
  }
TEST(Pose2D,TranslatePoseByDouble) {

  /** Translate this pose by a translation vector
   * \param x x component of vector to translate with
   * \param y y component of vector to translate with
   * \return A reference to this pose after translation
   */
/*  Pose2D& translate(const double x, const double y)
  {
    translation = *this * Vector2<double>(x,y);
    return *this;
  }
  */
  }
TEST(Pose2D,RotatePoseByDouble) {
  /** Rotate this pose by a rotation
   * \param angle Angle to rotate.
   * \return A reference to this pose after rotation
   */
/*  Pose2D& rotate(const double angle)
  {
    rotation += angle;
    rotation = Math::normalizeAngle(rotation);
    return *this;
  }
*/
  }
TEST(Pose2D,InvertPose) {


  /**
   * Calculates the inverse transformation from the current pose
   * @return The inverse transformation pose.
   */
/*  Pose2D invert() const
  {
    const double& invRotation = -rotation;
    return Pose2D(invRotation, (Vector2<double>() - translation).rotate(invRotation));
  }
*/
  }
TEST(Pose2D,CreateRandomPose) {

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
          }
