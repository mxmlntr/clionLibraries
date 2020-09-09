/**********************************************************************************************************************
 *  COPYRIGHT
 *  -------------------------------------------------------------------------------------------------------------------
 *  \verbatim
 *  Copyright (c) 2019 by Vector Informatik GmbH. All rights reserved.
 *
 *                This software is copyright protected and proprietary to Vector Informatik GmbH.
 *                Vector Informatik GmbH grants to you only those rights as set out in the license conditions.
 *                All other rights remain with Vector Informatik GmbH.
 *  \endverbatim
 *  -------------------------------------------------------------------------------------------------------------------
 *  FILE DESCRIPTION
 *  -----------------------------------------------------------------------------------------------------------------*/
/*!        \file  try.h
 *        \brief  Provides utility functions for converting a thrown exception into an error.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_LANGUAGE_TRY_H_
#define LIB_VAC_INCLUDE_VAC_LANGUAGE_TRY_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/

#include <type_traits>
#include <utility>

#include "ara/core/result.h"
#include "vac/language/cpp14_backport.h"
#include "vac/language/error_code.h"

namespace vac {
namespace language {

/*!
 * \brief  Wrapper template to create a mapping from an Exception type to an error.
 * \tparam ExceptionT Exception type which shall be mapped to an error.
 * \tparam ErrorT Type of the error. Defaulted to ErrorCode.
 */
template <typename ExceptionT, typename ErrorT = ErrorCode>
class Catch final {
 public:
  /*! \brief Exception type. */
  using ExceptionType = ExceptionT;
  /*! \brief Error type. */
  using ErrorType = ErrorT;

  /*!
   * \brief  Creates an error instance as a member with the given arguments.
   * \tparam Args Arguments for error construction.
   * \param  args Parameters for error construction.
   */
  template <typename... Args>
  constexpr explicit Catch(Args&&... args) noexcept(std::is_nothrow_constructible<ErrorT, Args&&...>::value)
      : error_{std::forward<Args>(args)...} {}

  /*!
   * \brief Default copy constructor.
   */
  Catch(Catch const&) = default;

  /*!
   * \brief Default move constructor.
   */
  Catch(Catch&&) = default;

  /*!
   * \brief Default copy assignment.
   */
  Catch& operator=(Catch const&) & = default;

  /*!
   * \brief Default move assignment.
   */
  Catch& operator=(Catch&&) & = default;

  /*!
   * \brief Default desturctor.
   */
  ~Catch() = default;

  /*!
   * \brief Getter for the error.
   */
  constexpr auto Error() const noexcept -> ErrorT const& { return error_; }

 private:
  /*!
   * \brief Error code, an instance of ExceptionType shall be mapped to.
   */
  ErrorT error_;
};

/*!
 * \brief   Executes a given Callable while converting a raised exception into an error.
 * \details The function call has three possible outcomes: (1) It returns the return value of \a func if no exception
 *          was thrown during the function call; (2) if the call of \a func returned with an exception as specified by
 *          \a map then the corresponding error is returned; (3) if the call of \a func returned with an exception not
 *          specified by \a map then the exception is forwarded.
 * \tparam  F Callable. Must return a Result. Must accept Args&&... as parameters.
 * \tparam  Ex Exception type to be catched.
 * \tparam  Er Error type.
 * \tparam  Args Function argument types.
 * \param   map Mapping for a specific exception type.
 * \param   func Callable which is executed during the function call.
 * \param   args Arguments of the callable.
 * \return  The return value of \a func, or the mapped error if an exception was raised as specified by \a map.
 * \trace   CREQ-181290
 */
template <typename F, typename Ex, typename Er, typename... Args,
          typename = enable_if_t<ara::core::is_result<result_of_t<F(Args&&...)>>::value>>
auto Try(Catch<Ex, Er> const& map, F&& func, Args&&... args) -> result_of_t<F(Args&&...)> {
  /*!
   * \brief Alias for result_of_t<F(Args && ...).
   */
  using R = result_of_t<F(Args && ...)>;

  try {
    /* VECTOR Next Line AutosarC++17_10-M6.6.5: MD_VAC_M6.6.5_multipleExit */
    return std::forward<F>(func)(std::forward<Args>(args)...);
  } catch (typename Catch<Ex, Er>::ExceptionType const&) {
    /* VECTOR Next Line AutosarC++17_10-M6.6.5: MD_VAC_M6.6.5_multipleExit */
    return R{R::kInPlaceError, map.Error()};
  }
}

/* VECTOR Next Construct AutosarC++17_10-A13.3.1: MD_VAC_A13.3.1_forwardingFunctionsShallNotBeOverloaded */
/*!
 * \brief   Wraps the return value of the given callable \a func into a Result while converting a raised exception into
 *          an error.
 * \details The function call has three possible outcomes: (1) It returns the return value of \a func if no
 *          exception was thrown during the function call; (2) if the call of \a func returned with an exception as
 *          specified by \a map then the corresponding error is returned; (3) if the call of \a func returned with an
 *          exception not specified by \a map then the exception is forwarded.
 * \tparam  F Callable. Must not return a Result. Must accept Args&&... as parameters.
 * \tparam  Ex Exception type to be catched. \tparam  Er Error type.
 * \tparam  Args Function argument types.
 * \param   map Mapping for a specific exception type.
 * \param   func Callable which is executed during the function call.
 * \param   args Arguments of the callable.
 * \return  The return value of \a func, or the mapped error if an exception was raised as specified by \a map.
 * \trace   CREQ-181290
 */
template <typename F, typename Ex, typename Er, typename... Args,
          typename = enable_if_t<!ara::core::is_result<result_of_t<F(Args&&...)>>::value>>
auto Try(Catch<Ex, Er> const& map, F&& func, Args&&... args) -> ara::core::Result<result_of_t<F(Args&&...)>, Er> {
  /*!
   * \brief Alias for result_of_t<F(Args && ...).
   */
  using FuncResult = result_of_t<F(Args && ...)>;

  /*!
   * \brief Alias for ara::core::Result<FuncResult, Er>.
   */
  using R = ara::core::Result<FuncResult, Er>;

  auto wrap_callable_to_result = [&func](Args&&... arguments) -> R {
    return R{R::kInPlaceValue, std::forward<F>(func)(std::forward<Args>(arguments)...)};
  };
  return Try(map, wrap_callable_to_result, std::forward<Args>(args)...);
}

/* VECTOR Next Construct AutosarC++17_10-A13.3.1: MD_VAC_A13.3.1_forwardingFunctionsShallNotBeOverloaded */
/*!
 * \brief   Executes a given Callable while converting a raised exception into an error.
 * \details The function call has three possible outcomes: (1) It returns the return value of \a func if no exception
 *          was thrown during the function call; (2) if the call of \a func returned with an exception as specified by
 *          \a map1 or \a map2 then the corresponding error is returned; (3) if the call of \a func returned with an
 *          exception not specified by \a map1 or \a map2 then the exception is forwarded.
 * \tparam  F Callable. Must return a Result. Must accept Args&&... as parameters.
 * \tparam  Ex1 Exception type 1 to be catched.
 * \tparam  Ex2 Exception type 2 to be catched.
 * \tparam  Er Error type.
 * \tparam  Args Function argument types.
 * \param   map1 Mapping for a specific exception type.
 * \param   map2 Mapping for a another specific exception type.
 * \param   func Callable which is executed during the function call.
 * \param   args Arguments of the callable.
 * \return  The return value of \a func, or the mapped error if an exception was raised as
 *          specified by \a map1 or \a map2.
 * \trace   CREQ-181290
 */
template <typename F, typename Ex1, typename Ex2, typename Er, typename... Args,
          typename = enable_if_t<ara::core::is_result<result_of_t<F(Args&&...)>>::value>>
auto Try(Catch<Ex1, Er> const& map1, Catch<Ex2, Er> const& map2, F&& func, Args&&... args)
    -> result_of_t<F(Args&&...)> {
  static_assert(!std::is_same<Ex1, Ex2>::value, "Exception types should not be the same.");
  static_assert(!std::is_base_of<Ex1, Ex2>::value,
                "Ex1 should not be a base class of Ex2. This would shadow the catch-clause for Ex2.");
  /*!
   * \brief Alias for result_of_t<F(Args && ...)>.
   */
  using R = result_of_t<F(Args && ...)>;
  R ret_value{R::FromValue()};

  try {
    ret_value = std::forward<F>(func)(std::forward<Args>(args)...);
  } catch (typename Catch<Ex1, Er>::ExceptionType const&) {
    ret_value = R{R::kInPlaceError, map1.Error()};
  } catch (typename Catch<Ex2, Er>::ExceptionType const&) {
    ret_value = R{R::kInPlaceError, map2.Error()};
  }
  return ret_value;
}

/* VECTOR Next Construct AutosarC++17_10-A13.3.1: MD_VAC_A13.3.1_forwardingFunctionsShallNotBeOverloaded */

/*!
 * \brief   Wraps the return value of the given callable \a func into a Result while converting a raised exception into
 *          an error.
 * \details The function call has three possible outcomes: (1) It returns the return value of \a func if no exception
 *          was thrown during the function call; (2) if the call of \a func returned with an exception as specified by
 *          \a map1 or \a map2 then the corresponding error is returned; (3) if the call of \a func returned with an
 *          exception not specified by \a map1 or \a map2 then the exception is forwarded.
 * \tparam  F Callable. Must not return a Result. Must accept Args&&... as parameters.
 * \tparam  Ex1 Exception type 1 to be catched.
 * \tparam  Ex2 Exception type 2 to be catched.
 * \tparam  Er Error type.
 * \tparam  Args Function argument types.
 * \param   map1 Mapping for a specific exception type.
 * \param   map2 Mapping for a another specific exception type.
 * \param   func Callable which is executed during the function call.
 * \param   args Arguments of the callable.
 * \return  The return value of \a func, or the mapped error if an exception was raised as
 *          specified by \a map1 or \a map2.
 * \trace   CREQ-181290
 */
template <typename F, typename Ex1, typename Ex2, typename Er, typename... Args,
          typename = enable_if_t<!ara::core::is_result<result_of_t<F(Args&&...)>>::value>>
auto Try(Catch<Ex1, Er> const& map1, Catch<Ex2, Er> const& map2, F&& func, Args&&... args)
    -> ara::core::Result<result_of_t<F(Args&&...)>, Er> {
  /*!
   * \brief Alias for result_of_t<F(Args && ...)>.
   */
  using FuncResult = result_of_t<F(Args && ...)>;

  /*!
   * \brief Alias for ara::core::Result<FuncResult, Er>.
   */
  using R = ara::core::Result<FuncResult, Er>;

  auto wrap_callable_result = [&func](Args&&... arguments) -> R {
    return R{R::kInPlaceValue, std::forward<F>(func)(std::forward<Args>(arguments)...)};
  };
  return Try(map1, map2, wrap_callable_result, std::forward<Args>(args)...);
}

}  // namespace language
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_LANGUAGE_TRY_H_
