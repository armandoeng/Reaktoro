
#ifdef EIGEN_PARSED_BY_DOXYGEN

/// \returns an expression of \c *this with reshaped sizes.
///
/// \param nRows the number of rows in the reshaped expression, specified at either run-time or compile-time, or AutoSize
/// \param nCols the number of columns in the reshaped expression, specified at either run-time or compile-time, or AutoSize
/// \tparam Order specifies whether the coefficients should be processed in column-major-order (ColMajor), in row-major-order (RowMajor),
///               or follows the \em natural order of the nested expression (AutoOrder). The default is ColMajor.
/// \tparam NRowsType the type of the value handling the number of rows, typically Index.
/// \tparam NColsType the type of the value handling the number of columns, typically Index.
///
/// Dynamic size example: \include MatrixBase_reshaped_int_int.cpp
/// Output: \verbinclude MatrixBase_reshaped_int_int.out
///
/// The number of rows \a nRows and columns \a nCols can also be specified at compile-time by passing Eigen::fix<N>,
/// or Eigen::fix<N>(n) as arguments. In the later case, \c n plays the role of a runtime fallback value in case \c N equals Eigen::Dynamic.
/// Here is an example with a fixed number of rows and columns:
/// \include MatrixBase_reshaped_fixed.cpp
/// Output: \verbinclude MatrixBase_reshaped_fixed.out
///
/// Finally, one of the sizes parameter can be automatically deduced from the other one by passing AutoSize as in the following example:
/// \include MatrixBase_reshaped_auto.cpp
/// Output: \verbinclude MatrixBase_reshaped_auto.out
/// AutoSize does preserve compile-time sizes when possible, i.e., when the sizes of the input are known at compile time \b and
/// that the other size is passed at compile-time using Eigen::fix<N> as above.
///
/// \sa operator()(all), class Reshaped, fix, fix<N>(int)
///
template<int Order = ColMajor, typename NRowsType, typename NColsType>
EIGEN_DEVICE_FUNC
inline Reshaped<Derived,...>
reshaped(NRowsType nRows, NColsType nCols);

/** This is the const version of reshaped(NRowsType,NColsType). */
template<int Order = ColMajor, typename NRowsType, typename NColsType>
EIGEN_DEVICE_FUNC
inline const Reshaped<const Derived,...>
reshaped(NRowsType nRows, NColsType nCols) const;

/// \returns an expression of \c *this with columns (or rows) stacked to a linear column (or row) vector
///
/// \tparam Order specifies whether to glue columns or rows, and returns a column or row vector.
///               Possible values are ColMajor, RowMajor or AutoOrder. The default is ColMajor.
///
/// If Order==ColMajor (the default), then it returns a column vector from the stacked columns of \c *this.
/// This is equivalent to \code A(all) \endcode and \code A.reshaped<RowMajor>(fix<1>,AutoSize) \endcode.
///
/// If Order==RowMajor, then it returns a row vector from the glued rows of \c *this.
/// This is equivalent to \code A.reshaped<RowMajor>(fix<1>,AutoSize) \endcode.
///
/// If Order=AutoOrder, the it returns the same expression as \code A.reshaped<storage_order_of_A>() \endcode
///
/// If you want more control, you can still fall back to reshaped(NRowsType,NColsType).
/// For instance, to return a column vector with element stacked following the storage order,
/// you can do: \code A.reshaped<AutoOrder>(AutoSize,fix<1>) \endcode
///
/// \sa operator()(all), reshaped(NRowsType,NColsType), class Reshaped
///
template<int Order = ColMajor>
EIGEN_DEVICE_FUNC
inline Reshaped<Derived,...>
reshaped();

/** This is the const version of reshaped(). */
template<int Order = ColMajor>
EIGEN_DEVICE_FUNC
inline const Reshaped<const Derived,...>
reshaped() const;

/// \returns as expression of \c *this with columns stacked to a linear column vector
///
/// This overload is essentially a shortcut for
/// \code this->reshape(AutoSize,fix<1>) \endcode
///
/// Example:
/// \include MatrixBase_reshaped_all.cpp
/// Output: \verbinclude MatrixBase_reshaped_all.out
///
/// \sa reshaped()
EIGEN_DEVICE_FUNC
inline Reshaped<Derived,SizeAtCompileTime,1>
operator()(all);

#else

// This file is automatically included twice to generate const and non-const versions

#ifndef EIGEN_RESHAPED_METHOD_2ND_PASS
#define EIGEN_RESHAPED_METHOD_CONST const
#else
#define EIGEN_RESHAPED_METHOD_CONST
#endif

#ifndef EIGEN_RESHAPED_METHOD_2ND_PASS

// This part is included once

#endif

template<typename NRowsType, typename NColsType>
EIGEN_DEVICE_FUNC
inline Reshaped<EIGEN_RESHAPED_METHOD_CONST Derived,
                internal::get_compiletime_reshape_size<NRowsType,NColsType,SizeAtCompileTime>::value,
                internal::get_compiletime_reshape_size<NColsType,NRowsType,SizeAtCompileTime>::value>
reshaped(NRowsType nRows, NColsType nCols) EIGEN_RESHAPED_METHOD_CONST
{
  return Reshaped<EIGEN_RESHAPED_METHOD_CONST Derived,
                  internal::get_compiletime_reshape_size<NRowsType,NColsType,SizeAtCompileTime>::value,
                  internal::get_compiletime_reshape_size<NColsType,NRowsType,SizeAtCompileTime>::value>
                (derived(),
                 internal::get_runtime_reshape_size(nRows,internal::get_runtime_value(nCols),size()),
                 internal::get_runtime_reshape_size(nCols,internal::get_runtime_value(nRows),size()));
}

template<int Order, typename NRowsType, typename NColsType>
EIGEN_DEVICE_FUNC
inline Reshaped<EIGEN_RESHAPED_METHOD_CONST Derived,
                internal::get_compiletime_reshape_size<NRowsType,NColsType,SizeAtCompileTime>::value,
                internal::get_compiletime_reshape_size<NColsType,NRowsType,SizeAtCompileTime>::value,
                (Order==AutoOrder?Flags&RowMajorBit:Order)>
reshaped(NRowsType nRows, NColsType nCols) EIGEN_RESHAPED_METHOD_CONST
{
  return Reshaped<EIGEN_RESHAPED_METHOD_CONST Derived,
                  internal::get_compiletime_reshape_size<NRowsType,NColsType,SizeAtCompileTime>::value,
                  internal::get_compiletime_reshape_size<NColsType,NRowsType,SizeAtCompileTime>::value,
                  Order==AutoOrder?Flags&RowMajorBit:Order>
                (derived(),
                 internal::get_runtime_reshape_size(nRows,internal::get_runtime_value(nCols),size()),
                 internal::get_runtime_reshape_size(nCols,internal::get_runtime_value(nRows),size()));
}

// Views as linear vectors

EIGEN_DEVICE_FUNC
inline Reshaped<EIGEN_RESHAPED_METHOD_CONST Derived,SizeAtCompileTime,1>
reshaped() EIGEN_RESHAPED_METHOD_CONST
{
  return Reshaped<EIGEN_RESHAPED_METHOD_CONST Derived,SizeAtCompileTime,1>(derived(),size(),1);
}

template<int Order>
EIGEN_DEVICE_FUNC
inline Reshaped<EIGEN_RESHAPED_METHOD_CONST Derived,
                (Order==RowMajor ? 1 : SizeAtCompileTime),
                (Order==RowMajor ? SizeAtCompileTime : 1),
                (Order==AutoOrder?Flags&RowMajorBit:Order)>
reshaped() EIGEN_RESHAPED_METHOD_CONST
{
  EIGEN_STATIC_ASSERT(Order==RowMajor || Order==ColMajor, INVALID_TEMPLATE_PARAMETER);
  return Reshaped<EIGEN_RESHAPED_METHOD_CONST Derived,
                  (Order==RowMajor ? 1 : SizeAtCompileTime),
                  (Order==RowMajor ? SizeAtCompileTime : 1),
                  (Order==AutoOrder?Flags&RowMajorBit:Order)>
                (derived(),
                 (Order==RowMajor ? 1 : size()),
                 (Order==RowMajor ? size() : 1));
}

#undef EIGEN_RESHAPED_METHOD_CONST

#ifndef EIGEN_RESHAPED_METHOD_2ND_PASS
#define EIGEN_RESHAPED_METHOD_2ND_PASS
#include "ReshapedMethods.h"
#undef EIGEN_RESHAPED_METHOD_2ND_PASS
#endif

#endif // EIGEN_PARSED_BY_DOXYGEN
