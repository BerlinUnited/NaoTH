/**
 * @file Pose3D.h
 * Contains class Pose3D
 *
 * @author <a href="mailto:martin.kallnik@gmx.de">Martin Kallnik</a>
 * @author Max Risler
 */

#ifndef __Pose3D_h__
#define __Pose3D_h__

#include "Matrix.h"
#include "Pose2D.h"

/** representation for 3D Transformation (Location + Orientation)*/
template<typename DATATYPE>
class Pose3T
{
protected:

  public:

  /** rotation as a RotationMatrix*/
  RotationMatrixT<DATATYPE> rotation;

  /** translation as a Vector3*/
  Vector3<DATATYPE> translation;

  /** constructor*/
  Pose3T() {}

  /** constructor from rotation and translation
   * \param rot Rotation
   * \param trans Translation
   */
  Pose3T(const RotationMatrix& rot, const Vector3<DATATYPE>& trans): rotation(rot), translation(trans){}

  /** constructor from rotation
   * \param rot Rotation
   */
  Pose3T(const RotationMatrix& rot): rotation(rot) {}

  /** constructor from translation
   * \param trans Translation
   */
  Pose3T(const Vector3<DATATYPE>& trans): translation(trans) {}

  /** constructor from three translation values
   * \param x translation x component
   * \param y translation y component
   * \param z translation z component
   */
  Pose3T(const DATATYPE x, const DATATYPE y, const DATATYPE z) : translation(x,y,z) {}

  /** Assignment operator
  *\param other The other Pose3D that is assigned to this one
  *\return A reference to this object after the assignment.
  */
  Pose3T& operator=(const Pose3T& other)
  {
    rotation=other.rotation;
    translation=other.translation;
    
    return *this;
  }

  /** Copy constructor
  *\param other The other vector that is copied to this one
  */
  Pose3T(const Pose3T& other) {*this = other;}

  /** Multiplication with Point
  *\param point (Vector3&lt;double&gt;)
  */
  Vector3<DATATYPE> operator*(const Vector3<DATATYPE>& point) const
  {
    return rotation * point + translation;
  }
  
  /** Comparison of another vector with this one.
  *\param other The other vector that will be compared to this one
  *\return Whether the two vectors are equal.
  */
  bool operator==(const Pose3T& other) const
  {
    return ((translation==other.translation)&&(rotation==other.rotation));
  }

  /** Comparison of another vector with this one.
  *\param other The other vector that will be compared to this one
  *\return Whether the two vectors are unequal.
  */
  bool operator!=(const Pose3T& other) const
    {return !(*this == other);}

  /**Concatenation of this pose with another pose
  *\param other The other pose that will be concatenated to this one.
  *\return A reference to this pose after concatenation
  *\deprecated the name is not goo,d use *= instead
  */
  Pose3T& conc(const Pose3T& other)
  {
    return (*this) *= other;
  }

  Pose3T& operator *=(const Pose3T& other)
  {
    translation = *this * other.translation;
    rotation *= other.rotation;
    return *this;
  }

  /**
     * calculate the local matrix, while it is in the same
     * coordinate (global) as this matrix
     *
     * @param T the global matrix
     *
     * @return the local matrix in this Pose coordinate 
     */
  Pose3T local(const Pose3T& other) const
  {
    return invert() * other;
  }
  
  /** Calculates the inverse transformation from the current pose
  * @return The inverse transformation pose.
  */
  Pose3T invert() const
  {
    Pose3T result;
    result.rotation = this->rotation.transpose();
    result.translation = (result.rotation * this->translation) * (-1);
    return result;
  }

  /**Translate this pose by a translation vector
  *\param trans Vector to translate with
  *\return A reference to this pose after translation
  */
  Pose3T& translate(const Vector3<DATATYPE>& trans)
  {
    translation = *this * trans;
    return *this;
  }

  /**Translate this pose by a translation vector
  *\param x x component of vector to translate with
  *\param y y component of vector to translate with
  *\param z z component of vector to translate with
  *\return A reference to this pose after translation
  */
  Pose3T& translate(const DATATYPE x, const DATATYPE y, const DATATYPE z)
  {
    translation = *this * Vector3<DATATYPE>(x,y,z);
    return *this;
  }

  /**Rotate this pose by a rotation
  *\param rot Rotationmatrix to rotate with
  *\return A reference to this pose after rotation
  */
  Pose3T& rotate(const RotationMatrix& rot)
  {
    rotation *= rot;
    return *this;
  }

  /**Rotate this pose around its x-axis
  *\param angle angle to rotate with
  *\return A reference to this pose after rotation
  */
  Pose3T& rotateX(const DATATYPE angle)
  {
    rotation.rotateX(angle);
    return *this;
  }
  
  /**Rotate this pose around its y-axis
  *\param angle angle to rotate with
  *\return A reference to this pose after rotation
  */
  Pose3T& rotateY(const DATATYPE angle)
  {
    rotation.rotateY(angle);
    return *this;
  }
  
  /**Rotate this pose around its z-axis
  *\param angle angle to rotate with
  *\return A reference to this pose after rotation
  */
  Pose3T& rotateZ(const DATATYPE angle)
  {
    rotation.rotateZ(angle);
    return *this;
  }

  void toCompactString(std::ostream& ost) const
  {
      rotation.toCompactString(ost);
      ost<<' '<<translation;
  }

  void toDataStream(std::ostream& stream) const
  {
    rotation.toDataStream(stream);
    translation.toDataStream(stream);
  }

  void fromDataStream(std::istream& stream, size_t size)
  {
    if ( 12*sizeof(DATATYPE) == size ){
      rotation.fromDataStream(stream, 9*sizeof(DATATYPE));
      translation.fromDataStream(stream, 3*sizeof(DATATYPE));
    }
  }

  /**Concatenation of this pose with another pose
   *\param other The other pose that will be concatenated to this one.
   *\return The Concatenation of this pose with the other pose
   */
  Pose3T operator*(const Pose3T& other) const
  {
    Pose3T v(*this);
    return v *= other;
  }
};

template <typename DATATYPE>
std::ostream& operator <<(std::ostream& ost, const Pose3T<DATATYPE>& v)
{
    ost << v.rotation.c[0]<<" 0 "
        << v.rotation.c[1]<<" 0 "
        << v.rotation.c[2]<<" 0 "
        << v.translation <<" 1";
    return ost;
}

typedef Pose3T<double> Pose3D;

/* covert Pose3T to Pose2T, drop some information */
template<typename DATATYPE>
Pose2T<DATATYPE> reduceDimen(const Pose3T<DATATYPE>& in)
{
  Pose2T<DATATYPE> out;
  out.translation.x = in.translation.x;
  out.translation.y = in.translation.y;
  out.rotation = in.rotation.getZAngle();
  return out;
}

#endif // __Pose3D_h__
