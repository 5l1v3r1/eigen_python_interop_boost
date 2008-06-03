// This file is part of Eigen, a lightweight C++ template library
// for linear algebra. Eigen itself is part of the KDE project.
//
// Copyright (C) 2008 Gael Guennebaud <g.gael@free.fr>
//
// Eigen is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
//
// Alternatively, you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// Eigen is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License or the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License and a copy of the GNU General Public License along with
// Eigen. If not, see <http://www.gnu.org/licenses/>.

#ifndef EIGEN_QUATERNION_H
#define EIGEN_QUATERNION_H

/** \class Quaternion
  *
  * \brief The quaternion class used to represent 3D orientations and rotations
  *
  * \param _Scalar the scalar type, i.e., the type of the coefficients
  *
  * This class represents a quaternion that is a convenient representation of
  * orientations and rotations of objects in three dimensions. Compared to other
  * representations like Euler angles or 3x3 matrices, quatertions offer the
  * following advantages:
  *   - compact storage (4 scalars)
  *   - efficient to compose (28 flops),
  *   - stable spherical interpolation
  *
  */
template<typename _Scalar>
struct ei_traits<Quaternion<_Scalar> >
{
  typedef _Scalar Scalar;
  enum {
    RowsAtCompileTime = 4,
    ColsAtCompileTime = 1,
    MaxRowsAtCompileTime = 4,
    MaxColsAtCompileTime = 1,
    Flags = ei_corrected_matrix_flags<_Scalar, 4, 0>::ret,
    CoeffReadCost = NumTraits<Scalar>::ReadCost
  };
};

template<typename _Scalar>
class Quaternion : public MatrixBase<Quaternion<_Scalar> >
{
public:

  public:

    EIGEN_GENERIC_PUBLIC_INTERFACE(Quaternion)

  private:

    EIGEN_ALIGN_128 Scalar m_data[4];

    inline int _rows() const { return 4; }
    inline int _cols() const { return 1; }

    inline const Scalar& _coeff(int i, int) const { return m_data[i]; }

    inline Scalar& _coeffRef(int i, int) { return m_data[i]; }

    template<int LoadMode>
    inline PacketScalar _packetCoeff(int row, int) const
    {
      ei_internal_assert(Flags & VectorizableBit);
      if (LoadMode==Eigen::Aligned)
        return ei_pload(&m_data[row]);
      else
        return ei_ploadu(&m_data[row]);
    }

    template<int StoreMode>
    inline void _writePacketCoeff(int row, int , const PacketScalar& x)
    {
      ei_internal_assert(Flags & VectorizableBit);
      if (StoreMode==Eigen::Aligned)
        ei_pstore(&m_data[row], x);
      else
        ei_pstoreu(&m_data[row], x);
    }

    inline int _stride(void) const { return _rows(); }

  public:

    typedef Matrix<Scalar,3,1> Vector3;
    typedef Matrix<Scalar,3,3> Matrix3;

    // FIXME what is the prefered order: w x,y,z or x,y,z,w ?
    inline Quaternion(Scalar w = 1.0, Scalar x = 0.0, Scalar y = 0.0, Scalar z = 0.0)
    {
      m_data[0] = x;
      m_data[1] = y;
      m_data[2] = z;
      m_data[3] = w;
    }

    /** Constructor copying the value of the expression \a other */
    template<typename OtherDerived>
    inline Quaternion(const Eigen::MatrixBase<OtherDerived>& other)
    {
      *this = other;
    }
    /** Copy constructor */
    inline Quaternion(const Quaternion& other)
      : Base()
    {
      *this = other;
    }

    /** Copies the value of the expression \a other into \c *this.
      */
    template<typename OtherDerived>
    inline Quaternion& operator=(const MatrixBase<OtherDerived>& other)
    {
      return Base::operator=(other.derived());
    }

    /** This is a special case of the templated operator=. Its purpose is to
      * prevent a default operator= from hiding the templated operator=.
      */
    inline Quaternion& operator=(const Quaternion& other)
    {
      return operator=<Quaternion>(other);
    }

    /** \overload
      * \returns a quaternion representing an identity rotation
      */
    static Quaternion identity() { return Quaternion(1, 0, 0, 0); }

    /** \overload
      * \sa Quaternion::identity()
      */
    Quaternion& setIdentity() { *this << 1, 0, 0, 0; return *this; }

    template<typename Derived>
    Quaternion& fromRotationMatrix(const MatrixBase<Derived>& m);
    Matrix3 toRotationMatrix(void) const;

    template<typename Derived>
    Quaternion& fromAngleAxis (const Scalar& angle, const MatrixBase<Derived>& axis);
    void toAngleAxis(Scalar& angle, Vector3& axis) const;

    Quaternion& fromEulerAngles(Vector3 eulerAngles);

    Vector3 toEulerAngles(void) const;

    template<typename Derived1, typename Derived2>
    Quaternion& fromTwoVectors(const MatrixBase<Derived1>& a, const MatrixBase<Derived2>& b);

    inline Quaternion operator* (const Quaternion& q) const;
    inline Quaternion& operator*= (const Quaternion& q);

    Quaternion inverse(void) const;
    Quaternion conjugate(void) const;

    Quaternion slerp(Scalar t, const Quaternion& other) const;

    template<typename Derived>
    Vector3 operator* (const MatrixBase<Derived>& vec) const;

private:
    // TODO discard here unreliable members.

};

/** \returns the concatenation of two rotations as a quaternion-quaternion product */
template <typename Scalar>
inline Quaternion<Scalar> Quaternion<Scalar>::operator* (const Quaternion& other) const
{
  return Quaternion
  (
    this->w() * other.w() - this->x() * other.x() - this->y() * other.y() - this->z() * other.z(),
    this->w() * other.x() + this->x() * other.w() + this->y() * other.z() - this->z() * other.y(),
    this->w() * other.y() + this->y() * other.w() + this->z() * other.x() - this->x() * other.z(),
    this->w() * other.z() + this->z() * other.w() + this->x() * other.y() - this->y() * other.x()
  );
}

template <typename Scalar>
inline Quaternion<Scalar>& Quaternion<Scalar>::operator*= (const Quaternion& other)
{
  return (*this = *this * other);
}

/** Rotation of a vector by a quaternion.
  * \remarks If the quaternion is used to rotate several points (>1)
  * then it is much more efficient to first convert it to a 3x3 Matrix.
  * Comparison of the operation cost for n transformations:
  *   - Quaternion:    30n
  *   - Via a Matrix3: 24 + 15n
  */
template <typename Scalar>
template<typename Derived>
inline typename Quaternion<Scalar>::Vector3
Quaternion<Scalar>::operator* (const MatrixBase<Derived>& v) const
{
    // Note that this algorithm comes from the optimization by hand
    // of the conversion to a Matrix followed by a Matrix/Vector product.
    // It appears to be much faster than the common algorithm found
    // in the litterature (30 versus 39 flops). It also requires two
    // Vector3 as temporaries.
    Vector3 uv;
    uv = 2 * this->template start<3>().cross(v);
    return v + this->w() * uv + this->template start<3>().cross(uv);
}

/** Convert the quaternion to a 3x3 rotation matrix */
template<typename Scalar>
inline typename Quaternion<Scalar>::Matrix3
Quaternion<Scalar>::toRotationMatrix(void) const
{
  // FIXME another option would be to declare toRotationMatrix like that:
  // OtherDerived& toRotationMatrix(MatrixBase<OtherDerived>& m)
  // it would fill m and returns a ref to m.
  // the advantages is that way we can accept 4x4 and 3x4 matrices filling the rest of the
  // matrix with I... ??
  Matrix3 res;

  Scalar tx  = 2*this->x();
  Scalar ty  = 2*this->y();
  Scalar tz  = 2*this->z();
  Scalar twx = tx*this->w();
  Scalar twy = ty*this->w();
  Scalar twz = tz*this->w();
  Scalar txx = tx*this->x();
  Scalar txy = ty*this->x();
  Scalar txz = tz*this->x();
  Scalar tyy = ty*this->y();
  Scalar tyz = tz*this->y();
  Scalar tzz = tz*this->z();

  res(0,0) = 1-(tyy+tzz);
  res(0,1) = txy-twz;
  res(0,2) = txz+twy;
  res(1,0) = txy+twz;
  res(1,1) = 1-(txx+tzz);
  res(1,2) = tyz-twx;
  res(2,0) = txz-twy;
  res(2,1) = tyz+twx;
  res(2,2) = 1-(txx+tyy);

  return res;
}

/** updates \c *this from the rotation matrix \a m and returns a reference to \c *this
  * \warning the size of the input matrix expression \a m must be 3x3 at compile time
  */
template<typename Scalar>
template<typename Derived>
Quaternion<Scalar>& Quaternion<Scalar>::fromRotationMatrix(const MatrixBase<Derived>& m)
{
  // FIXME maybe this function could accept 4x4 and 3x4 matrices as well ? (simply update the assert)
  ei_assert(Derived::RowsAtCompileTime==3 && Derived::ColsAtCompileTime==3);
  // This algorithm comes from  "Quaternion Calculus and Fast Animation",
  // Ken Shoemake, 1987 SIGGRAPH course notes
  Scalar t = m.trace();
  if (t > 0)
  {
    t = ei_sqrt(t + 1.0);
    this->w() = 0.5*t;
    t = 0.5/t;
    this->x() = (m.coeff(2,1) - m.coeff(1,2)) * t;
    this->y() = (m.coeff(0,2) - m.coeff(2,0)) * t;
    this->z() = (m.coeff(1,0) - m.coeff(0,1)) * t;
  }
  else
  {
    int i = 0;
    if (m(1,1) > m(0,0))
      i = 1;
    if (m(2,2) > m(i,i))
      i = 2;
    int j = (i+1)%3;
    int k = (j+1)%3;

    t = ei_sqrt(m.coeff(i,i)-m.coeff(j,j)-m.coeff(k,k) + 1.0);
    this->coeffRef(i) = 0.5 * t;
    t = 0.5/t;
    this->w() = (m.coeff(k,j)-m.coeff(j,k))*t;
    this->coeffRef(j) = (m.coeff(j,i)+m.coeff(i,j))*t;
    this->coeffRef(k) = (m.coeff(k,i)+m.coeff(i,k))*t;
  }

  return *this;
}

/** updates \c *this from the rotation defined by axis \a axis and angle \a angle
  * and returns a reference to \c *this
  * \warning the size of the input vector expression \a axis must be 3 at compile time
  */
template<typename Scalar>
template<typename Derived>
inline Quaternion<Scalar>& Quaternion<Scalar>
::fromAngleAxis(const Scalar& angle, const MatrixBase<Derived>& axis)
{
  ei_assert(Derived::SizeAtCompileTime==3);
  Scalar ha = 0.5*angle;
  this->w() = ei_cos(ha);
  this->template start<3>() = ei_sin(ha) * axis;
  return *this;
}

/** Computes and returns the angle and axis of the rotation represented by the quaternion.
  * The values are returned in the arguments \a angle and \a axis respectively.
  * The returned axis is normalized.
  */
template <typename Scalar>
void Quaternion<Scalar>::toAngleAxis(Scalar& angle, Vector3& axis) const
{
  // FIXME should we split this function to an "angle" and an "axis" functions ?
  // the drawbacks is that this approach would require to compute twice the norm of (x,y,z)...
  // or we returns a Vector4, or a small AngleAxis object... ???
  Scalar n2 = this->template start<3>().norm2();
  if (ei_isMuchSmallerThan(n2,Scalar(1)))
  {
    angle = 0;
    axis << 1, 0, 0;
  }
  else
  {
    angle = 2*std::acos(this->w());
    axis = this->template start<3>() / ei_sqrt(n2);
  }
}

/** updates \c *this from the rotation defined by the Euler angles \a eulerAngles,
  * and returns a reference to \c *this
  */
template <typename Scalar>
Quaternion<Scalar>& Quaternion<Scalar>::fromEulerAngles(Vector3 eulerAngles)
{
  // FIXME should the arguments be 3 scalars or a single Vector3 ?
  eulerAngles *= 0.5;

  Vector3 cosines = eulerAngles.cwiseCos();
  Vector3 sines   = eulerAngles.cwiseSin();

  Scalar cYcZ = cosines.y() * cosines.z();
  Scalar sYsZ = sines.y() * sines.z();
  Scalar sYcZ = sines.y() * cosines.z();
  Scalar cYsZ = cosines.y() * sines.z();

  this->w() = cosines.x() * cYcZ + sines.x()   * sYsZ;
  this->x() = sines.x()   * cYcZ - cosines.x() * sYsZ;
  this->y() = cosines.x() * sYcZ + sines.x()   * cYsZ;
  this->z() = cosines.x() * cYsZ - sines.x()   * sYcZ;

  return *this;
}

/** Computes and returns the Euler angles corresponding to the quaternion \c *this.
  */
template <typename Scalar>
typename Quaternion<Scalar>::Vector3 Quaternion<Scalar>::toEulerAngles(void) const
{
  Scalar y2 = this->y() * this->y();
  return Vector3(
    std::atan2(2*(this->w()*this->x() + this->y()*this->z()), (1 - 2*(this->x()*this->x() + y2))),
    std::asin( 2*(this->w()*this->y() - this->z()*this->x())),
    std::atan2(2*(this->w()*this->z() + this->x()*this->y()), (1 - 2*(y2 + this->z()*this->z()))));
}

/** Makes a quaternion representing the rotation between two vectors \a a and \a b.
  * \returns a reference to the actual quaternion
  * Note that the two input vectors have \b not to be normalized.
  */
template<typename Scalar>
template<typename Derived1, typename Derived2>
inline Quaternion<Scalar>& Quaternion<Scalar>::fromTwoVectors(const MatrixBase<Derived1>& a, const MatrixBase<Derived2>& b)
{
  Vector3 v0 = a.normalized();
  Vector3 v1 = b.normalized();
  Vector3 axis = v0.cross(v1);
  Scalar c = v0.dot(v1);

  // if dot == 1, vectors are the same
  if (ei_isApprox(c,Scalar(1)))
  {
    // set to identity
    this->w() = 1; this->template start<3>().setZero();
  }
  Scalar s = ei_sqrt((1+c)*2);
  Scalar invs = 1./s;
  this->template start<3>() = axis * invs;
  this->w() = s * 0.5;

  return *this;
}

/** \returns the multiplicative inverse of \c *this
  * Note that in most cases, i.e., if you simply want the opposite
  * rotation, it is enough to use the conjugate.
  *
  * \sa Quaternion::conjugate()
  */
template <typename Scalar>
inline Quaternion<Scalar> Quaternion<Scalar>::inverse() const
{
  // FIXME should this funtion be called multiplicativeInverse and conjugate() be called inverse() or opposite()  ??
  Scalar n2 = this->norm2();
  if (n2 > 0)
    return conjugate() / n2;
  else
  {
    // return an invalid result to flag the error
    return this->zero();
  }
}

/** \returns the conjugate of the \c *this which is equal to the multiplicative inverse
  * if the quaternion is normalized.
  * The conjugate of a quaternion represents the opposite rotation.
  *
  * \sa Quaternion::inverse()
  */
template <typename Scalar>
inline Quaternion<Scalar> Quaternion<Scalar>::conjugate() const
{
  return Quaternion(this->w(),-this->x(),-this->y(),-this->z());
}

/** \returns the spherical linear interpolation between the two quaternions
  * \c *this and \a other at the parameter \a t
  */
template <typename Scalar>
Quaternion<Scalar> Quaternion<Scalar>::slerp(Scalar t, const Quaternion& other) const
{
  // FIXME options for this function would be:
  // 1 - Quaternion& fromSlerp(Scalar t, const Quaternion& q0, const Quaternion& q1);
  //     which set *this from the s-lerp and returns *this
  // 2 - Quaternion slerp(Scalar t, const Quaternion& other) const
  //     which returns the s-lerp between this and other
  // ??
  if (*this == other)
    return *this;

  Scalar d = this->dot(other);

  // theta is the angle between the 2 quaternions
  Scalar theta = std::acos(ei_abs(d));
  Scalar sinTheta = ei_sin(theta);

  Scalar scale0 = ei_sin( ( 1 - t ) * theta) / sinTheta;
  Scalar scale1 = ei_sin( ( t * theta) ) / sinTheta;
  if (d<0)
    scale1 = -scale1;

  return scale0 * (*this) + scale1 * other;
}

#endif // EIGEN_QUATERNION_H