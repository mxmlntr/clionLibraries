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
/*!        \file  either_trait.h
 *        \brief  Trait for tagged union type with two options (Either).
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_LANGUAGE_DETAIL_EITHER_TRAIT_H_
#define LIB_VAC_INCLUDE_VAC_LANGUAGE_DETAIL_EITHER_TRAIT_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/

#include <type_traits>

namespace vac {
namespace language {
namespace detail {

/*!
 * \brief  Trait for tagged union type with two options (Either).
 * \tparam L Left type of the Either.
 * \tparam R Right type of the Either.
 */
template <typename L, typename R>
struct EitherTrait final {
 private:
  struct AllTypes {
    L l_;
    R r_;
  };

 public:
  /*! \brief True if L and R are move constructible, else false. */
  static constexpr bool is_move_constructible = std::is_move_constructible<AllTypes>::value;

  /*! \brief True if L and R are nothrow move constructible, else false. */
  static constexpr bool is_nothrow_move_constructible = std::is_nothrow_move_constructible<AllTypes>::value;

  /*! \brief True if L and R are trivially move constructible, else false. */
  static constexpr bool is_trivially_move_constructible = std::is_trivially_move_constructible<AllTypes>::value;

  /*! \brief True if L and R are copy constructible, else false. */
  static constexpr bool is_copy_constructible = std::is_copy_constructible<AllTypes>::value;

  /*! \brief True if L and R are trivially copy constructible, else false. */
  static constexpr bool is_trivially_copy_constructible = std::is_trivially_copy_constructible<AllTypes>::value;

  /*! \brief True if L and R are trivially destructible, else false. */
  static constexpr bool is_trivially_destructible = std::is_trivially_destructible<AllTypes>::value;

  /*! \brief True if L and R are move constructible and move assignable, else false. */
  static constexpr bool is_move_assignable = is_move_constructible && std::is_move_assignable<AllTypes>::value;

  /*!
   * \brief True if L and R are nothrow destructible, nothrow move constructible, and nothrow move assignable,
   *        else false.
   */
  static constexpr bool is_nothrow_move_assignable = std::is_nothrow_destructible<AllTypes>::value &&
                                                     is_nothrow_move_constructible &&
                                                     std::is_nothrow_move_assignable<AllTypes>::value;

  /*!
   * \brief True if L and R are trivially destructible, trivially move constructible, and trivially move assignable,
   *        else false.
   */
  static constexpr bool is_trivially_move_assignable = is_trivially_destructible && is_trivially_move_constructible &&
                                                       std::is_trivially_move_assignable<AllTypes>::value;

  /*! \brief True if L and R are copy constructible and copy assignable, else false. */
  static constexpr bool is_copy_assignable = is_copy_constructible && std::is_copy_assignable<AllTypes>::value;

  /*!
   * \brief True if L and R are trivially destructible, trivially copy constructible, and trivially copy assignable,
   *        else false.
   */
  static constexpr bool is_trivially_copy_assignable = is_trivially_destructible && is_trivially_copy_constructible &&
                                                       std::is_trivially_copy_assignable<AllTypes>::value;
};

}  // namespace detail
}  // namespace language
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_LANGUAGE_DETAIL_EITHER_TRAIT_H_
