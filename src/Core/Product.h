// This file is part of Eigen, a lightweight C++ template library
// for linear algebra. Eigen itself is part of the KDE project.
//
// Copyright (C) 2006-2007 Benoit Jacob <jacob@math.jussieu.fr>
//
// Eigen is free software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation; either version 2 or (at your option) any later version.
//
// Eigen is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along
// with Eigen; if not, write to the Free Software Foundation, Inc., 51
// Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
//
// As a special exception, if other files instantiate templates or use macros
// or functions from this file, or you compile this file and link it
// with other works to produce a work based on this file, this file does not
// by itself cause the resulting work to be covered by the GNU General Public
// License. This exception does not invalidate any other reasons why a work
// based on this file might be covered by the GNU General Public License.

#ifndef EIGEN_PRODUCT_H
#define EIGEN_PRODUCT_H

template<int Index, int Size, typename Lhs, typename Rhs>
struct ProductUnroller
{
  static void run(int row, int col, const Lhs& lhs, const Rhs& rhs,
                  typename Lhs::Scalar &res)
  {
    ProductUnroller<Index-1, Size, Lhs, Rhs>::run(row, col, lhs, rhs, res);
    res += lhs.read(row, Index) * rhs.read(Index, col);
  }
};

template<int Size, typename Lhs, typename Rhs>
struct ProductUnroller<0, Size, Lhs, Rhs>
{
  static void run(int row, int col, const Lhs& lhs, const Rhs& rhs,
                  typename Lhs::Scalar &res)
  {
    res = lhs.read(row, 0) * rhs.read(0, col);
  }
};

template<int Index, typename Lhs, typename Rhs>
struct ProductUnroller<Index, Dynamic, Lhs, Rhs>
{
  static void run(int row, int col, const Lhs& lhs, const Rhs& rhs,
                  typename Lhs::Scalar &res)
  {
    EIGEN_UNUSED(row);
    EIGEN_UNUSED(col);
    EIGEN_UNUSED(lhs);
    EIGEN_UNUSED(rhs);
    EIGEN_UNUSED(res);
  }
};

template<typename Lhs, typename Rhs> class Product
  : public MatrixBase<typename Lhs::Scalar, Product<Lhs, Rhs> >
{
  public:
    typedef typename Lhs::Scalar Scalar;
    typedef typename Lhs::Ref LhsRef;
    typedef typename Rhs::Ref RhsRef;
    friend class MatrixBase<Scalar, Product>;
    
    static const int RowsAtCompileTime = Lhs::RowsAtCompileTime,
                     ColsAtCompileTime = Rhs::ColsAtCompileTime;

    Product(const LhsRef& lhs, const RhsRef& rhs)
      : m_lhs(lhs), m_rhs(rhs) 
    {
      assert(lhs.cols() == rhs.rows());
    }
    
    Product(const Product& other)
      : m_lhs(other.m_lhs), m_rhs(other.m_rhs) {}
    
    // assignments are illegal but we still want to intercept them and get clean compile errors
    EIGEN_INHERIT_ASSIGNMENT_OPERATORS(Product)
    
  private:
    const Product& _ref() const { return *this; }
    int _rows() const { return m_lhs.rows(); }
    int _cols() const { return m_rhs.cols(); }
    
    Scalar _read(int row, int col) const
    {
      Scalar res;
      if(Lhs::ColsAtCompileTime != Dynamic && Lhs::ColsAtCompileTime <= 16)
        ProductUnroller<Lhs::ColsAtCompileTime-1, Lhs::ColsAtCompileTime, LhsRef, RhsRef>
          ::run(row, col, m_lhs, m_rhs, res);
      else
      {
        res = m_lhs(row, 0) * m_rhs(0, col);
        for(int i = 1; i < m_lhs.cols(); i++)
          res += m_lhs(row, i) * m_rhs(i, col);
      }
      return res;
    }
    
  protected:
    const LhsRef m_lhs;
    const RhsRef m_rhs;
};

template<typename Scalar, typename Derived>
template<typename OtherDerived>
Product<Derived, OtherDerived>
MatrixBase<Scalar, Derived>::lazyProduct(const MatrixBase<Scalar, OtherDerived> &other) const
{
  return Product<Derived, OtherDerived>(ref(), other.ref());
}

template<typename Scalar, typename Derived1, typename Derived2>
Eval<Product<Derived1, Derived2> >
operator*(const MatrixBase<Scalar, Derived1> &mat1, const MatrixBase<Scalar, Derived2> &mat2)
{
  return mat1.lazyProduct(mat2).eval();
}

template<typename Scalar, typename Derived>
template<typename OtherDerived>
Derived &
MatrixBase<Scalar, Derived>::operator*=(const MatrixBase<Scalar, OtherDerived> &other)
{
  return *this = *this * other;
}

#endif // EIGEN_PRODUCT_H