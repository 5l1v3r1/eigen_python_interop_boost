// This file is part of Eigen, a lightweight C++ template library
// for linear algebra.
//
// Copyright (C) 2010 Manuel Yguel <manuel.yguel@gmail.com>
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

#include "main.h"
#include <unsupported/Eigen/Polynomials>
#include <iostream>
#include <algorithm>

#ifdef HAS_GSL
#include "gsl_helper.h"
#endif

using namespace std;

template<int Size>
struct ei_increment_if_fixed_size
{
  enum {
    ret = (Size == Dynamic) ? Dynamic : Size+1
  };
};




template<int Deg, typename POLYNOMIAL, typename SOLVER>
bool aux_evalSolver( const POLYNOMIAL& pols, SOLVER& psolve )
{
  typedef typename POLYNOMIAL::Scalar Scalar;

  typedef typename SOLVER::RootsType    RootsType;
  typedef Matrix<Scalar,Deg,1>          EvalRootsType;

  const int deg = pols.size()-1;

  psolve.compute( pols );
  const RootsType& roots( psolve.roots() );
  EvalRootsType evr( deg );
  for( int i=0; i<roots.size(); ++i ){
    evr[i] = std::abs( poly_eval( pols, roots[i] ) ); }

  bool evalToZero = evr.isZero( test_precision<Scalar>() );
  if( !evalToZero )
  {
    cerr << "WRONG root: " << endl;
    cerr << "Polynomial: " << pols.transpose() << endl;
    cerr << "Roots found: " << roots.transpose() << endl;
    cerr << "Abs value of the polynomial at the roots: " << evr.transpose() << endl;
    cerr << endl;
  }

  #ifdef HAS_GSL
  if (ei_is_same_type< Scalar, double>::ret)
  {
    typedef GslTraits<Scalar> Gsl;
    RootsType gslRoots(deg);
    Gsl::eigen_poly_solve( pols, gslRoots );
    EvalRootsType gslEvr( deg );
    for( int i=0; i<gslRoots.size(); ++i )
    {
      gslEvr[i] = std::abs( poly_eval( pols, gslRoots[i] ) );
    }
    bool gslEvalToZero = gslEvr.isZero( test_precision<Scalar>() );
    if( !evalToZero )
    {
      if( !gslEvalToZero ){
        cerr << "GSL also failed" << endl; }
      else{
        cerr << "GSL did NOT failed" << endl; }
      cerr << "GSL roots found: " << gslRoots.transpose() << endl;
      cerr << "Abs value of the polynomial at the GSL roots: " << gslEvr.transpose() << endl;
      cerr << endl;
    }
  }
  #endif //< HAS_GSL


  std::vector<Scalar> rootModuli( roots.size() );
  Map< EvalRootsType > aux( &rootModuli[0], roots.size() );
  aux = roots.array().abs();
  std::sort( rootModuli.begin(), rootModuli.end() );
  bool distinctModuli=true;
  for( size_t i=1; i<rootModuli.size() && distinctModuli; ++i )
  {
    if( ei_isApprox( rootModuli[i], rootModuli[i-1] ) ){
      distinctModuli = false; }
  }
  VERIFY( evalToZero || !distinctModuli );

  return distinctModuli;
}







template<int Deg, typename POLYNOMIAL>
void evalSolver( const POLYNOMIAL& pols )
{
  typedef typename POLYNOMIAL::Scalar Scalar;

  typedef PolynomialSolver<Scalar, Deg >              PolynomialSolverType;

  PolynomialSolverType psolve;
  aux_evalSolver<Deg, POLYNOMIAL, PolynomialSolverType>( pols, psolve );
}




template< int Deg, typename POLYNOMIAL, typename ROOTS, typename REAL_ROOTS >
void evalSolverSugarFunction( const POLYNOMIAL& pols, const ROOTS& roots, const REAL_ROOTS& real_roots )
{
  typedef typename POLYNOMIAL::Scalar Scalar;

  typedef PolynomialSolver<Scalar, Deg >              PolynomialSolverType;

  PolynomialSolverType psolve;
  if( aux_evalSolver<Deg, POLYNOMIAL, PolynomialSolverType>( pols, psolve ) )
  {
    //It is supposed that
    // 1) the roots found are correct
    // 2) the roots have distinct moduli

    typedef typename POLYNOMIAL::Scalar                 Scalar;
    typedef typename REAL_ROOTS::Scalar                 Real;

    typedef PolynomialSolver<Scalar, Deg >              PolynomialSolverType;
    typedef typename PolynomialSolverType::RootsType    RootsType;
    typedef Matrix<Scalar,Deg,1>                        EvalRootsType;

    //Test realRoots
    std::vector< Real > calc_realRoots;
    psolve.realRoots( calc_realRoots );
    VERIFY( calc_realRoots.size() == (size_t)real_roots.size() );

    const Scalar psPrec = ei_sqrt( test_precision<Scalar>() );

    for( size_t i=0; i<calc_realRoots.size(); ++i )
    {
      bool found = false;
      for( size_t j=0; j<calc_realRoots.size()&& !found; ++j )
      {
        if( ei_isApprox( calc_realRoots[i], real_roots[j] ), psPrec ){
          found = true; }
      }
      VERIFY( found );
    }

    //Test greatestRoot
    VERIFY( ei_isApprox( roots.array().abs().maxCoeff(),
          ei_abs( psolve.greatestRoot() ), psPrec ) );

    //Test smallestRoot
    VERIFY( ei_isApprox( roots.array().abs().minCoeff(),
          ei_abs( psolve.smallestRoot() ), psPrec ) );

    bool hasRealRoot;
    //Test absGreatestRealRoot
    Real r = psolve.absGreatestRealRoot( hasRealRoot );
    VERIFY( hasRealRoot == (real_roots.size() > 0 ) );
    if( hasRealRoot ){
      VERIFY( ei_isApprox( real_roots.array().abs().maxCoeff(), ei_abs(r), psPrec ) );  }

    //Test absSmallestRealRoot
    r = psolve.absSmallestRealRoot( hasRealRoot );
    VERIFY( hasRealRoot == (real_roots.size() > 0 ) );
    if( hasRealRoot ){
      VERIFY( ei_isApprox( real_roots.array().abs().minCoeff(), ei_abs( r ), psPrec ) ); }

    //Test greatestRealRoot
    r = psolve.greatestRealRoot( hasRealRoot );
    VERIFY( hasRealRoot == (real_roots.size() > 0 ) );
    if( hasRealRoot ){
      VERIFY( ei_isApprox( real_roots.array().maxCoeff(), r, psPrec ) ); }

    //Test smallestRealRoot
    r = psolve.smallestRealRoot( hasRealRoot );
    VERIFY( hasRealRoot == (real_roots.size() > 0 ) );
    if( hasRealRoot ){
    VERIFY( ei_isApprox( real_roots.array().minCoeff(), r, psPrec ) ); }
  }
}


template<typename _Scalar, int _Deg>
void polynomialsolver(int deg)
{
  typedef ei_increment_if_fixed_size<_Deg>            Dim;
  typedef Matrix<_Scalar,Dim::ret,1>                  PolynomialType;
  typedef Matrix<_Scalar,_Deg,1>                      EvalRootsType;

  cout << "Standard cases" << endl;
  PolynomialType pols = PolynomialType::Random(deg+1);
  evalSolver<_Deg,PolynomialType>( pols );

  cout << "Hard cases" << endl;
  _Scalar multipleRoot = ei_random<_Scalar>();
  EvalRootsType allRoots = EvalRootsType::Constant(deg,multipleRoot);
  roots_to_monicPolynomial( allRoots, pols );
  evalSolver<_Deg,PolynomialType>( pols );

  cout << "Test sugar" << endl;
  EvalRootsType realRoots = EvalRootsType::Random(deg);
  roots_to_monicPolynomial( realRoots, pols );
  evalSolverSugarFunction<_Deg>(
      pols,
      realRoots.template cast <
                    std::complex<
                         typename NumTraits<_Scalar>::Real
                         >
                    >(),
      realRoots );
}


template<typename _Scalar> void polynomialsolver_scalar()
{
  CALL_SUBTEST_1( (polynomialsolver<_Scalar,1>(1)) );
  CALL_SUBTEST_2( (polynomialsolver<_Scalar,2>(2)) );
  CALL_SUBTEST_3( (polynomialsolver<_Scalar,3>(3)) );
  CALL_SUBTEST_4( (polynomialsolver<_Scalar,4>(4)) );
  CALL_SUBTEST_5( (polynomialsolver<_Scalar,5>(5)) );
  CALL_SUBTEST_6( (polynomialsolver<_Scalar,6>(6)) );
  CALL_SUBTEST_7( (polynomialsolver<_Scalar,7>(7)) );
  CALL_SUBTEST_8( (polynomialsolver<_Scalar,8>(8)) );

  CALL_SUBTEST_9( (polynomialsolver<_Scalar,Dynamic>(
          ei_random<int>(9,45)
          )) );
}

void test_polynomialsolver()
{
  for(int i = 0; i < g_repeat; i++)
  {
    polynomialsolver_scalar<double>();
    polynomialsolver_scalar<float>();
  }
}
