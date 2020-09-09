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
/*!        \file  token_init.h
 *        \brief  Utility for creating an instance with token initialization.
 *
 *      \details  Creating an instance of type T with token initialization involves two consecutive steps: the call of
 *                a pre-construction function and the call of the actual constructor of T. The pre-construction
 *                function takes all parameters as arguments which are required for the construction of the instance of
 *                T, and returns a Result<Token,E> with construction token type 'Token' and the error type E. The
 *                pre-construction performs all operations which may result in an error. It returns either such an error
 *                or the successfully constructed token. The actual constructor is only called if the pre-construction
 *                function returned a token. In the constructor, the token is stored directly or the required members
 *                are extracted from the token.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_LANGUAGE_TOKEN_INIT_H_
#define LIB_VAC_INCLUDE_VAC_LANGUAGE_TOKEN_INIT_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/

#include <utility>

#include "ara/core/result.h"
#include "vac/language/cpp14_backport.h"

namespace vac {
namespace language {

/*!
 * \brief   Creates an instance of type T using a custom pre-construction function.
 * \details This function requires a pre-construction function with the following signature:
 *          <code>
 *          Result<Token,E> T::Preconstruct(Args&&...)
 *          </code>
 *          and a constructor of T taking the token as argument:
 *          <code>
 *          T(Token&&...)
 *          </code>.
 * \tparam  T Type of the instance which shall be created.
 * \tparam  PreConstr Pre-construction function.
 * \tparam  Args Construction parameters.
 * \param   pre Pre-construction function.
 * \param   args Arguments for the pre-construction function.
 * \return  The constructed instance of T, or the error returned by the pre-construction function.
 * \trace   CREQ-181061
 */
template <typename T, typename PreConstr, typename... Args>
auto TokenInitCustom(PreConstr&& pre, Args&&... args)
    -> ara::core::Result<T, typename result_of_t<PreConstr(Args&&...)>::error_type> {
  /*!
   * \brief An alias for result_of_t<PreConstr(Args && ...)>.
   */
  using PreConstrResult = result_of_t<PreConstr(Args && ...)>;
  static_assert(ara::core::is_result<PreConstrResult>::value, "Return type of PreConstr must be Result type.");

  /*!
   * \brief An alias for PreConstrResult::value_type.
   */
  using Token = typename PreConstrResult::value_type;

  /*!
   * \brief An alias for PreConstrResult::error_type.
   */
  using Error = typename PreConstrResult::error_type;

  static_assert(std::is_constructible<T, Token&&>::value, "T must be constructible from token type.");

  /*!
   * \brief An alias for ara::core::Result<T, Error>.
   */
  using R = ara::core::Result<T, Error>;

  return std::forward<PreConstr>(pre)(std::forward<Args>(args)...).AndThen([](Token&& token) -> R {
    return R{R::kInPlaceValue, std::move(token)};
  });
}

/*!
 * \brief   Creates an instance of type T.
 * \details This function requires a static member function with the signature as pre-construction function:
 *          <code>
 *          Result<Token,E> T::Preconstruct(Args&&...)
 *          </code>
 *          and a constructor taking the token as argument:
 *          <code>
 *          T(Token&&...)
 *          </code>.
 * \tparam  T Type of the instance which shall be created.
 * \tparam  Args Construction parameters for p.
 * \param   args Arguments for the pre-construction function.
 * \return  The constructed instance of T, or the error returned by the pre-construction function.
 * \trace   CREQ-181061
 */
template <typename T, typename... Args>
auto TokenInit(Args&&... args)
    -> ara::core::Result<T, typename result_of_t<decltype (&T::Preconstruct)(Args&&...)>::error_type> {
  return TokenInitCustom<T>(&T::Preconstruct, std::forward<Args>(args)...);
}

}  // namespace language
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_LANGUAGE_TOKEN_INIT_H_
