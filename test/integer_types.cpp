// This file is part of Eigen, a lightweight C++ template library
// for linear algebra.
//
// Copyright (C) 2010 Benoit Jacob <jacob.benoit.1@gmail.com>
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

#define EIGEN_NO_STATIC_ASSERT

#include "main.h"

#undef VERIFY_IS_APPROX
#define VERIFY_IS_APPROX(a, b) VERIFY((a)==(b));
#undef VERIFY_IS_NOT_APPROX
#define VERIFY_IS_NOT_APPROX(a, b) VERIFY((a)!=(b));

template<typename MatrixType> void integer_types(const MatrixType& m)
{
  typedef typename MatrixType::Scalar Scalar;

  VERIFY(NumTraits<Scalar>::IsInteger);
  enum { is_signed = (Scalar(-1) > Scalar(0)) ? 0 : 1 };
  VERIFY(int(NumTraits<Scalar>::IsSigned) == is_signed);
  
  typedef Matrix<Scalar, MatrixType::RowsAtCompileTime, 1> VectorType;

  int rows = m.rows();
  int cols = m.cols();

  // this test relies a lot on Random.h, and there's not much more that we can do
  // to test it, hence I consider that we will have tested Random.h
  MatrixType m1(rows, cols),
             m2 = MatrixType::Random(rows, cols),
             m3(rows, cols),
             mzero = MatrixType::Zero(rows, cols);

  typedef Matrix<Scalar, MatrixType::RowsAtCompileTime, MatrixType::RowsAtCompileTime> SquareMatrixType;
  SquareMatrixType identity = SquareMatrixType::Identity(rows, rows),
                   square = SquareMatrixType::Random(rows, rows);
  VectorType v1(rows),
             v2 = VectorType::Random(rows),
             vzero = VectorType::Zero(rows);

  do {
    m1 = MatrixType::Random(rows, cols);
  } while(m1 == mzero || m1 == m2);

  do {
    v1 = VectorType::Random(rows);
  } while(v1 == vzero || v1 == v2);

  VERIFY_IS_APPROX(               v1,    v1);
  VERIFY_IS_NOT_APPROX(           v1,    2*v1);
  VERIFY_IS_APPROX(               vzero, v1-v1);
  VERIFY_IS_APPROX(               m1,    m1);
  VERIFY_IS_NOT_APPROX(           m1,    2*m1);
  VERIFY_IS_APPROX(               mzero, m1-m1);

  VERIFY_IS_APPROX(m3 = m1,m1);
  MatrixType m4;
  VERIFY_IS_APPROX(m4 = m1,m1);

  m3.real() = m1.real();
  VERIFY_IS_APPROX(static_cast<const MatrixType&>(m3).real(), static_cast<const MatrixType&>(m1).real());
  VERIFY_IS_APPROX(static_cast<const MatrixType&>(m3).real(), m1.real());

  // check == / != operators
  VERIFY(m1==m1);
  VERIFY(m1!=m2);
  VERIFY(!(m1==m2));
  VERIFY(!(m1!=m1));
  m1 = m2;
  VERIFY(m1==m2);
  VERIFY(!(m1!=m2));

  // check linear structure
  
  Scalar s1;
  do {
    s1 = ei_random<Scalar>();
  } while(s1 == 0);

  VERIFY_IS_EQUAL(-(-m1),                  m1);
  VERIFY_IS_EQUAL(m1+m1,                   2*m1);
  VERIFY_IS_EQUAL(m1+m2-m1,                m2);
  VERIFY_IS_EQUAL(-m2+m1+m2,               m1);
  VERIFY_IS_EQUAL(m1*s1,                   s1*m1);
  VERIFY_IS_EQUAL((m1+m2)*s1,              s1*m1+s1*m2);
  VERIFY_IS_EQUAL((-m1+m2)*s1,             -s1*m1+s1*m2);
  m3 = m2; m3 += m1;
  VERIFY_IS_EQUAL(m3,                      m1+m2);
  m3 = m2; m3 -= m1;
  VERIFY_IS_EQUAL(m3,                      m2-m1);
  m3 = m2; m3 *= s1;
  VERIFY_IS_EQUAL(m3,                      s1*m2);

  // check matrix product.

  VERIFY_IS_APPROX(identity * m1, m1);
  VERIFY_IS_APPROX(square * (m1 + m2), square * m1 + square * m2);
  VERIFY_IS_APPROX((m1 + m2).transpose() * square, m1.transpose() * square + m2.transpose() * square);
  VERIFY_IS_APPROX((m1 * m2.transpose()) * m1, m1 * (m2.transpose() * m1));
}

void test_integer_types()
{
  for(int i = 0; i < g_repeat; i++) {
    CALL_SUBTEST_1( integer_types(Matrix<unsigned int, 1, 1>()) );
    CALL_SUBTEST_1( integer_types(Matrix<unsigned long, 3, 4>()) );
    CALL_SUBTEST_2( integer_types(Matrix<long, 2, 2>()) );

    CALL_SUBTEST_3( integer_types(Matrix<char, 2, Dynamic>(2, 10)) );
    CALL_SUBTEST_4( integer_types(Matrix<unsigned char, 3, 3>()) );
    CALL_SUBTEST_4( integer_types(Matrix<unsigned char, Dynamic, Dynamic>(20, 20)) );

    CALL_SUBTEST_5( integer_types(Matrix<short, Dynamic, 4>(7, 4)) );
    CALL_SUBTEST_6( integer_types(Matrix<unsigned short, 4, 4>()) );

    CALL_SUBTEST_7( integer_types(Matrix<long long, 11, 13>()) );
    CALL_SUBTEST_8( integer_types(Matrix<unsigned long long, Dynamic, 5>(1, 5)) );
  }
}
