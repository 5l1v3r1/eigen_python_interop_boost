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

#ifndef EIGEN_TRIDIAGONALIZATION_H
#define EIGEN_TRIDIAGONALIZATION_H

/** \class Tridiagonalization
  *
  * \brief Trigiagonal decomposition of a selfadjoint matrix
  *
  * \param MatrixType the type of the matrix of which we are computing the eigen decomposition
  *
  * This class performs a tridiagonal decomposition of a selfadjoint matrix \f$ A \f$ such that:
  * \f$ A = Q T Q^* \f$ where \f$ Q \f$ is unitatry and \f$ T \f$ a real symmetric tridiagonal matrix
  *
  * \sa MatrixBase::tridiagonalize()
  */
template<typename _MatrixType> class Tridiagonalization
{
  public:

    typedef _MatrixType MatrixType;
    typedef typename MatrixType::Scalar Scalar;
    typedef typename NumTraits<Scalar>::Real RealScalar;

    enum {SizeMinusOne = MatrixType::RowsAtCompileTime==Dynamic
                        ? Dynamic
                        : MatrixType::RowsAtCompileTime-1};

    typedef Matrix<Scalar, SizeMinusOne, 1> CoeffVectorType;

    typedef typename NestByValue<DiagonalCoeffs<MatrixType> >::RealReturnType DiagonalType;

    typedef typename NestByValue<DiagonalCoeffs<
        NestByValue<Block<
          MatrixType,SizeMinusOne,SizeMinusOne> > > >::RealReturnType SubDiagonalType;

    Tridiagonalization()
    {}

    Tridiagonalization(int rows, int cols)
      : m_matrix(rows,cols), m_hCoeffs(rows-1)
    {}

    Tridiagonalization(const MatrixType& matrix)
      : m_matrix(matrix),
        m_hCoeffs(matrix.cols()-1)
    {
      _compute(m_matrix, m_hCoeffs);
    }

    /** Computes or re-compute the tridiagonalization for the matrix \a matrix.
      *
      * This method allows to re-use the allocated data.
      */
    void compute(const MatrixType& matrix)
    {
      m_matrix = matrix;
      m_hCoeffs.resize(matrix.rows()-1);
      _compute(m_matrix, m_hCoeffs);
    }

    /** \returns the householder coefficients allowing to
      * reconstruct the matrix Q from the packed data.
      *
      * \sa packedMatrix()
      */
    CoeffVectorType householderCoefficients(void) const { return m_hCoeffs; }

    /** \returns the internal result of the decomposition.
      *
      * The returned matrix contains the following information:
      *  - the strict upper part is equal to the input matrix A
      *  - the diagonal and lower sub-diagonal represent the tridiagonal symmetric matrix (real).
      *  - the rest of the lower part contains the Householder vectors that, combined with
      *    Householder coefficients returned by householderCoefficients(),
      *    allows to reconstruct the matrix Q as follow:
      *       Q = H_{N-1} ... H_1 H_0
      *    where the matrices H are the Householder transformation:
      *       H_i = (I - h_i * v_i * v_i')
      *    where h_i == householderCoefficients()[i] and v_i is a Householder vector:
      *       v_i = [ 0, ..., 0, 1, M(i+2,i), ..., M(N-1,i) ]
      *
      * See LAPACK for further details on this packed storage.
      */
    const MatrixType& packedMatrix(void) const { return m_matrix; }

    MatrixType matrixQ(void) const;
    const DiagonalType diagonal(void) const;
    const SubDiagonalType subDiagonal(void) const;

  private:

    static void _compute(MatrixType& matA, CoeffVectorType& hCoeffs);

  protected:
    MatrixType m_matrix;
    CoeffVectorType m_hCoeffs;
};


/** \internal
  * Performs a tridiagonal decomposition of \a matA in place.
  *
  * \param matA the input selfadjoint matrix
  * \param hCoeffs returned Householder coefficients
  *
  * The result is written in the lower triangular part of \a matA.
  *
  * Implemented from Golub's "Matrix Computations", algorithm 8.3.1.
  *
  * \sa packedMatrix()
  */
template<typename MatrixType>
void Tridiagonalization<MatrixType>::_compute(MatrixType& matA, CoeffVectorType& hCoeffs)
{
  assert(matA.rows()==matA.cols());
  int n = matA.rows();
  for (int i = 0; i<n-2; ++i)
  {
    // let's consider the vector v = i-th column starting at position i+1

    // start of the householder transformation
    // squared norm of the vector v skipping the first element
    RealScalar v1norm2 = matA.col(i).end(n-(i+2)).norm2();

    if (ei_isMuchSmallerThan(v1norm2,static_cast<Scalar>(1)))
    {
      hCoeffs.coeffRef(i) = 0.;
    }
    else
    {
      Scalar v0 = matA.col(i).coeff(i+1);
      RealScalar beta = ei_sqrt(ei_abs2(v0)+v1norm2);
      if (ei_real(v0)>=0.)
        beta = -beta;
      matA.col(i).end(n-(i+2)) *= (1./(v0-beta));
      matA.col(i).coeffRef(i+1) = beta;
      Scalar h = (beta - v0) / beta;
      // end of the householder transformation

      // Apply similarity transformation to remaining columns,
      // i.e., A = H' A H where H = I - h v v' and v = matA.col(i).end(n-i-1)

      matA.col(i).coeffRef(i+1) = 1;
      // let's use the end of hCoeffs to store temporary values
      hCoeffs.end(n-i-1) = h * (matA.corner(BottomRight,n-i-1,n-i-1).template extract<Lower|SelfAdjoint>()
                                * matA.col(i).end(n-i-1));


      hCoeffs.end(n-i-1) += (h * (-0.5) * matA.col(i).end(n-i-1).dot(hCoeffs.end(n-i-1)))
                            * matA.col(i).end(n-i-1);

      matA.corner(BottomRight,n-i-1,n-i-1).template part<Lower>() =
        matA.corner(BottomRight,n-i-1,n-i-1) - (
            (matA.col(i).end(n-i-1) * hCoeffs.end(n-i-1).adjoint()).lazy()
          + (hCoeffs.end(n-i-1) * matA.col(i).end(n-i-1).adjoint()).lazy() );
      // FIXME check that the above expression does follow the lazy path (no temporary and
      // only lower products are evaluated)
      // FIXME can we avoid to evaluate twice the diagonal products ?
      // (in a simple way otherwise it's overkill)

      matA.col(i).coeffRef(i+1) = beta;

      hCoeffs.coeffRef(i) = h;
    }
  }
  if (NumTraits<Scalar>::IsComplex)
  {
    // householder transformation on the remaining single scalar
    int i = n-2;
    Scalar v0 = matA.col(i).coeff(i+1);
    RealScalar beta = ei_abs(v0);
    if (ei_real(v0)>=0.)
      beta = -beta;
    matA.col(i).coeffRef(i+1) = beta;
    hCoeffs.coeffRef(i) = (beta - v0) / beta;
  }
}

/** reconstructs and returns the matrix Q */
template<typename MatrixType>
typename Tridiagonalization<MatrixType>::MatrixType
Tridiagonalization<MatrixType>::matrixQ(void) const
{
  int n = m_matrix.rows();
  MatrixType matQ = MatrixType::identity(n,n);
  for (int i = n-2; i>=0; i--)
  {
    Scalar tmp = m_matrix.coeff(i+1,i);
    m_matrix.const_cast_derived().coeffRef(i+1,i) = 1;

    matQ.corner(BottomRight,n-i-1,n-i-1) -=
      ((m_hCoeffs[i] * m_matrix.col(i).end(n-i-1)) *
      (m_matrix.col(i).end(n-i-1).adjoint() * matQ.corner(BottomRight,n-i-1,n-i-1)).lazy()).lazy();

    m_matrix.const_cast_derived().coeffRef(i+1,i) = tmp;
  }
  return matQ;
}

/** \returns an expression of the diagonal vector */
template<typename MatrixType>
const typename Tridiagonalization<MatrixType>::DiagonalType
Tridiagonalization<MatrixType>::diagonal(void) const
{
  return m_matrix.diagonal().nestByValue().real();
}

/** \returns an expression of the sub-diagonal vector */
template<typename MatrixType>
const typename Tridiagonalization<MatrixType>::SubDiagonalType
Tridiagonalization<MatrixType>::subDiagonal(void) const
{
  int n = m_matrix.rows();
  return Block<MatrixType,SizeMinusOne,SizeMinusOne>(m_matrix, 1, 0, n-1,n-1)
    .nestByValue().diagonal().nestByValue().real();
}

#endif // EIGEN_TRIDIAGONALIZATION_H