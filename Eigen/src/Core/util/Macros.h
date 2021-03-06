// This file is part of Eigen, a lightweight C++ template library
// for linear algebra.
//
// Copyright (C) 2008-2010 Gael Guennebaud <g.gael@free.fr>
// Copyright (C) 2006-2008 Benoit Jacob <jacob.benoit.1@gmail.com>
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

#ifndef EIGEN_MACROS_H
#define EIGEN_MACROS_H

#define EIGEN_WORLD_VERSION 2
#define EIGEN_MAJOR_VERSION 91
#define EIGEN_MINOR_VERSION 0

#define EIGEN_VERSION_AT_LEAST(x,y,z) (EIGEN_WORLD_VERSION>x || (EIGEN_WORLD_VERSION>=x && \
                                      (EIGEN_MAJOR_VERSION>y || (EIGEN_MAJOR_VERSION>=y && \
                                                                 EIGEN_MINOR_VERSION>=z))))
#ifdef __GNUC__
  #define EIGEN_GNUC_AT_LEAST(x,y) ((__GNUC__>=x && __GNUC_MINOR__>=y) || __GNUC__>x)
#else
  #define EIGEN_GNUC_AT_LEAST(x,y) 0
#endif

#if defined(__GNUC__) && (__GNUC__ <= 3)
#define EIGEN_GCC3_OR_OLDER 1
#else
#define EIGEN_GCC3_OR_OLDER 0
#endif

// 16 byte alignment is only useful for vectorization. Since it affects the ABI, we need to enable
// 16 byte alignment on all platforms where vectorization might be enabled. In theory we could always
// enable alignment, but it can be a cause of problems on some platforms, so we just disable it in
// certain common platform (compiler+architecture combinations) to avoid these problems.
// Only static alignment is really problematic (relies on nonstandard compiler extensions that don't
// work everywhere, for example don't work on GCC/ARM), try to keep heap alignment even
// when we have to disable static alignment.
#if defined(__GNUC__) && !(defined(__i386__) || defined(__x86_64__) || defined(__powerpc__) || defined(__ppc__) || defined(__ia64__))
#define EIGEN_GCC_AND_ARCH_DOESNT_WANT_STACK_ALIGNMENT 1
#else
#define EIGEN_GCC_AND_ARCH_DOESNT_WANT_STACK_ALIGNMENT 0
#endif

// static alignment is completely disabled with GCC 3, Sun Studio, and QCC/QNX
#if !EIGEN_GCC_AND_ARCH_DOESNT_WANT_STACK_ALIGNMENT \
 && !EIGEN_GCC3_OR_OLDER \
 && !defined(__SUNPRO_CC) \
 && !defined(__QNXNTO__)
  #define EIGEN_ARCH_WANTS_STACK_ALIGNMENT 1
#else
  #define EIGEN_ARCH_WANTS_STACK_ALIGNMENT 0
#endif

#ifdef EIGEN_DONT_ALIGN
  #ifndef EIGEN_DONT_ALIGN_STATICALLY
    #define EIGEN_DONT_ALIGN_STATICALLY
  #endif
  #define EIGEN_ALIGN 0
#else
  #define EIGEN_ALIGN 1
#endif

// EIGEN_ALIGN_STATICALLY is the true test whether we want to align arrays on the stack or not. It takes into account both the user choice to explicitly disable
// alignment (EIGEN_DONT_ALIGN_STATICALLY) and the architecture config (EIGEN_ARCH_WANTS_STACK_ALIGNMENT). Henceforth, only EIGEN_ALIGN_STATICALLY should be used.
#if EIGEN_ARCH_WANTS_STACK_ALIGNMENT && !defined(EIGEN_DONT_ALIGN_STATICALLY)
  #define EIGEN_ALIGN_STATICALLY 1
#else
  #define EIGEN_ALIGN_STATICALLY 0
  #ifndef EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
    #define EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
  #endif
#endif

#ifdef EIGEN_DEFAULT_TO_ROW_MAJOR
#define EIGEN_DEFAULT_MATRIX_STORAGE_ORDER_OPTION RowMajor
#else
#define EIGEN_DEFAULT_MATRIX_STORAGE_ORDER_OPTION ColMajor
#endif

/** Allows to disable some optimizations which might affect the accuracy of the result.
  * Such optimization are enabled by default, and set EIGEN_FAST_MATH to 0 to disable them.
  * They currently include:
  *   - single precision Cwise::sin() and Cwise::cos() when SSE vectorization is enabled.
  */
#ifndef EIGEN_FAST_MATH
#define EIGEN_FAST_MATH 1
#endif

#define EIGEN_DEBUG_VAR(x) std::cerr << #x << " = " << x << std::endl;

#ifdef NDEBUG
# ifndef EIGEN_NO_DEBUG
#  define EIGEN_NO_DEBUG
# endif
#endif

#ifndef ei_assert
#ifdef EIGEN_NO_DEBUG
#define ei_assert(x)
#else
#define ei_assert(x) assert(x)
#endif
#endif

#ifdef EIGEN_INTERNAL_DEBUGGING
#define ei_internal_assert(x) ei_assert(x)
#else
#define ei_internal_assert(x)
#endif

#ifdef EIGEN_NO_DEBUG
#define EIGEN_ONLY_USED_FOR_DEBUG(x) (void)x
#else
#define EIGEN_ONLY_USED_FOR_DEBUG(x)
#endif

// EIGEN_ALWAYS_INLINE_ATTRIB should be use in the declaration of function
// which should be inlined even in debug mode.
// FIXME with the always_inline attribute,
// gcc 3.4.x reports the following compilation error:
//   Eval.h:91: sorry, unimplemented: inlining failed in call to 'const Eigen::Eval<Derived> Eigen::MatrixBase<Scalar, Derived>::eval() const'
//    : function body not available
#if EIGEN_GNUC_AT_LEAST(4,0)
#define EIGEN_ALWAYS_INLINE_ATTRIB __attribute__((always_inline))
#else
#define EIGEN_ALWAYS_INLINE_ATTRIB
#endif

// EIGEN_FORCE_INLINE means "inline as much as possible"
#if (defined _MSC_VER) || (defined __intel_compiler)
#define EIGEN_STRONG_INLINE __forceinline
#else
#define EIGEN_STRONG_INLINE inline
#endif

#if (defined __GNUC__)
#define EIGEN_DONT_INLINE __attribute__((noinline))
#elif (defined _MSC_VER)
#define EIGEN_DONT_INLINE __declspec(noinline)
#else
#define EIGEN_DONT_INLINE
#endif

#if (defined __GNUC__)
#define EIGEN_DEPRECATED __attribute__((deprecated))
#elif (defined _MSC_VER)
#define EIGEN_DEPRECATED __declspec(deprecated)
#else
#define EIGEN_DEPRECATED
#endif

#if (defined __GNUC__)
#define EIGEN_UNUSED __attribute__((unused))
#else
#define EIGEN_UNUSED
#endif

#if (defined __GNUC__)
#define EIGEN_ASM_COMMENT(X)  asm("#"X)
#else
#define EIGEN_ASM_COMMENT(X)
#endif

/* EIGEN_ALIGN_TO_BOUNDARY(n) forces data to be n-byte aligned. This is used to satisfy SIMD requirements.
 * However, we do that EVEN if vectorization (EIGEN_VECTORIZE) is disabled,
 * so that vectorization doesn't affect binary compatibility.
 *
 * If we made alignment depend on whether or not EIGEN_VECTORIZE is defined, it would be impossible to link
 * vectorized and non-vectorized code.
 */
#if !EIGEN_ALIGN_STATICALLY
  #define EIGEN_ALIGN_TO_BOUNDARY(n)
#elif (defined __GNUC__) || (defined __PGI)
  #define EIGEN_ALIGN_TO_BOUNDARY(n) __attribute__((aligned(n)))
#elif (defined _MSC_VER)
  #define EIGEN_ALIGN_TO_BOUNDARY(n) __declspec(align(n))
#elif (defined __SUNPRO_CC)
  // FIXME not sure about this one:
  #define EIGEN_ALIGN_TO_BOUNDARY(n) __attribute__((aligned(n)))
#else
  #error Please tell me what is the equivalent of __attribute__((aligned(n))) for your compiler
#endif

#define EIGEN_ALIGN16 EIGEN_ALIGN_TO_BOUNDARY(16)

#ifdef EIGEN_DONT_USE_RESTRICT_KEYWORD
  #define EIGEN_RESTRICT
#endif
#ifndef EIGEN_RESTRICT
  #define EIGEN_RESTRICT __restrict
#endif

#ifndef EIGEN_STACK_ALLOCATION_LIMIT
#define EIGEN_STACK_ALLOCATION_LIMIT 20000
#endif

#ifndef EIGEN_DEFAULT_IO_FORMAT
#define EIGEN_DEFAULT_IO_FORMAT Eigen::IOFormat()
#endif

// just an empty macro !
#define EIGEN_EMPTY

// concatenate two tokens
#define EIGEN_CAT2(a,b) a ## b
#define EIGEN_CAT(a,b) EIGEN_CAT2(a,b)

// convert a token to a string
#define EIGEN_MAKESTRING2(a) #a
#define EIGEN_MAKESTRING(a) EIGEN_MAKESTRING2(a)

// format used in Eigen's documentation
// needed to define it here as escaping characters in CMake add_definition's argument seems very problematic.
#define EIGEN_DOCS_IO_FORMAT IOFormat(3, 0, " ", "\n", "", "")

// C++0x features
#if defined(__GXX_EXPERIMENTAL_CXX0X__) || (defined(_MSC_VER) && (_MSC_VER >= 1600))
  #define EIGEN_REF_TO_TEMPORARY const &
#else
  #define EIGEN_REF_TO_TEMPORARY const &
#endif

#if defined(_MSC_VER) && (!defined(__INTEL_COMPILER))
#define EIGEN_INHERIT_ASSIGNMENT_EQUAL_OPERATOR(Derived) \
  using Base::operator =;
#else
#define EIGEN_INHERIT_ASSIGNMENT_EQUAL_OPERATOR(Derived) \
  using Base::operator =; \
  EIGEN_STRONG_INLINE Derived& operator=(const Derived& other) \
  { \
    Base::operator=(other); \
    return *this; \
  }
#endif

#define EIGEN_INHERIT_ASSIGNMENT_OPERATORS(Derived) \
  EIGEN_INHERIT_ASSIGNMENT_EQUAL_OPERATOR(Derived)

/**
* Just a side note. Commenting within defines works only by documenting
* behind the object (via '!<'). Comments cannot be multi-line and thus
* we have these extra long lines. What is confusing doxygen over here is
* that we use '\' and basically have a bunch of typedefs with their
* documentation in a single line.
**/

#define EIGEN_GENERIC_PUBLIC_INTERFACE_NEW(Derived) \
  typedef typename Eigen::ei_traits<Derived>::Scalar Scalar; /*!< \brief Numeric type, e.g. float, double, int or std::complex<float>. */ \
  typedef typename Eigen::NumTraits<Scalar>::Real RealScalar; /*!< \brief The underlying numeric type for composed scalar types. \details In cases where Scalar is e.g. std::complex<T>, T were corresponding to RealScalar. */ \
  typedef typename Base::CoeffReturnType CoeffReturnType; /*!< \brief The return type for coefficient access. \details Depending on whether the object allows direct coefficient access (e.g. for a MatrixXd), this type is either 'const Scalar&' or simply 'Scalar' for objects that do not allow direct coefficient access. */ \
  typedef typename Eigen::ei_nested<Derived>::type Nested; \
  enum { RowsAtCompileTime = Eigen::ei_traits<Derived>::RowsAtCompileTime, \
        ColsAtCompileTime = Eigen::ei_traits<Derived>::ColsAtCompileTime, \
        Flags = Eigen::ei_traits<Derived>::Flags, \
        CoeffReadCost = Eigen::ei_traits<Derived>::CoeffReadCost, \
        SizeAtCompileTime = Base::SizeAtCompileTime, \
        MaxSizeAtCompileTime = Base::MaxSizeAtCompileTime, \
        IsVectorAtCompileTime = Base::IsVectorAtCompileTime };


#define EIGEN_DENSE_PUBLIC_INTERFACE(Derived) \
  typedef typename Eigen::ei_traits<Derived>::Scalar Scalar; /*!< \brief Numeric type, e.g. float, double, int or std::complex<float>. */ \
  typedef typename Eigen::NumTraits<Scalar>::Real RealScalar; /*!< \brief The underlying numeric type for composed scalar types. \details In cases where Scalar is e.g. std::complex<T>, T were corresponding to RealScalar. */ \
  typedef typename Base::PacketScalar PacketScalar; \
  typedef typename Base::CoeffReturnType CoeffReturnType; /*!< \brief The return type for coefficient access. \details Depending on whether the object allows direct coefficient access (e.g. for a MatrixXd), this type is either 'const Scalar&' or simply 'Scalar' for objects that do not allow direct coefficient access. */ \
  typedef typename Eigen::ei_nested<Derived>::type Nested; \
  enum { RowsAtCompileTime = Eigen::ei_traits<Derived>::RowsAtCompileTime, \
        ColsAtCompileTime = Eigen::ei_traits<Derived>::ColsAtCompileTime, \
        MaxRowsAtCompileTime = Eigen::ei_traits<Derived>::MaxRowsAtCompileTime, \
        MaxColsAtCompileTime = Eigen::ei_traits<Derived>::MaxColsAtCompileTime, \
        Flags = Eigen::ei_traits<Derived>::Flags, \
        CoeffReadCost = Eigen::ei_traits<Derived>::CoeffReadCost, \
        SizeAtCompileTime = Base::SizeAtCompileTime, \
        MaxSizeAtCompileTime = Base::MaxSizeAtCompileTime, \
        IsVectorAtCompileTime = Base::IsVectorAtCompileTime }; \
  using Base::derived; \
  using Base::const_cast_derived;


#define EIGEN_ENUM_MIN(a,b) (((int)a <= (int)b) ? (int)a : (int)b)
#define EIGEN_SIZE_MIN(a,b) (((int)a == 1 || (int)b == 1) ? 1 \
                           : ((int)a == Dynamic || (int)b == Dynamic) ? Dynamic \
                           : ((int)a <= (int)b) ? (int)a : (int)b)
#define EIGEN_ENUM_MAX(a,b) (((int)a >= (int)b) ? (int)a : (int)b)
#define EIGEN_LOGICAL_XOR(a,b) (((a) || (b)) && !((a) && (b)))

#define EIGEN_IMPLIES(a,b) (!(a) || (b))

#define EIGEN_MAKE_CWISE_BINARY_OP(METHOD,FUNCTOR) \
  template<typename OtherDerived> \
  inline const CwiseBinaryOp<FUNCTOR<Scalar>, Derived, OtherDerived> \
  METHOD(const EIGEN_CURRENT_STORAGE_BASE_CLASS<OtherDerived> &other) const \
  { \
    return CwiseBinaryOp<FUNCTOR<Scalar>, Derived, OtherDerived>(derived(), other.derived()); \
  }

// the expression type of a cwise product
#define EIGEN_CWISE_PRODUCT_RETURN_TYPE(LHS,RHS) \
    CwiseBinaryOp< \
      ei_scalar_product_op< \
        typename ei_scalar_product_traits< \
          typename ei_traits<LHS>::Scalar, \
          typename ei_traits<RHS>::Scalar \
        >::ReturnType \
      >, \
      LHS, \
      RHS \
    >

#endif // EIGEN_MACROS_H
