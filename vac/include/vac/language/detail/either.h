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
/**        \file  either.h
 *        \brief  Internal union-like class that represents either the Left type or Right type.
 *
 *********************************************************************************************************************/
#ifndef LIB_VAC_INCLUDE_VAC_LANGUAGE_DETAIL_EITHER_H_
#define LIB_VAC_INCLUDE_VAC_LANGUAGE_DETAIL_EITHER_H_
/*********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/

#include <type_traits>
#include <utility>

#include "ara/core/utility.h"
#include "vac/language/cpp14_backport.h"
#include "vac/language/cpp17_backport.h"
#include "vac/language/detail/either_storage.h"
#include "vac/language/detail/either_trait.h"

namespace vac {
namespace language {
namespace detail {

/*!
 * \brief  An alias for ara::core::in_place_type_t.
 */
template <typename T>
using in_place_type_t = ara::core::in_place_type_t<T>;

/*!
 * \brief  Internal union-like class that represents either the Left type or Right type.
 * \tparam L The left type for that union.
 * \tparam R The right type for that union.
 * \trace  CREQ-171867
 */
template <typename L, typename R>
class Either final {
 public:
  /*! \brief The trait used for analyzing the eithers traits. */
  using Trait = EitherTrait<L, R>;

  /*!
   * \brief Constructor from left rvalue.
   * \param l The value from which to construct the Either.
   */
  constexpr explicit Either(L&& l) : Either(in_place_type_t<L>{}, std::move(l)) {}
  /*!
   * \brief Constructor from right rvalue.
   * \param r The value from which to construct the Either.
   */
  constexpr explicit Either(R&& r) : Either(in_place_type_t<R>{}, std::move(r)) {}
  /*!
   * \brief Constructor from left value.
   * \param l The value from which to construct the Either.
   */
  constexpr explicit Either(L const& l) : Either(in_place_type_t<L>{}, l) {}
  /*!
   * \brief Constructor from right value.
   * \param r The value from which to construct the Either.
   */
  constexpr explicit Either(R const& r) : Either(in_place_type_t<R>{}, r) {}

  /*!
   * \brief  Emplace constructor for the left value.
   * \tparam Args The type of arguments for the called constructor.
   * \param  args Arguments of construction.
   */
  template <typename... Args, typename = vac::language::enable_if_t<std::is_constructible<L, Args...>::value>>
  constexpr explicit Either(in_place_type_t<L>, Args&&... args)
      : storage_{in_place_type_t<L>{}, std::forward<Args>(args)...} {}

  /*!
   * \brief  Emplace constructor for the right value.
   * \tparam Args The type of arguments for the called constructor.
   * \param  args Arguments of construction.
   */
  template <typename... Args, typename = vac::language::enable_if_t<std::is_constructible<R, Args...>::value>>
  constexpr explicit Either(in_place_type_t<R>, Args&&... args)
      : storage_{in_place_type_t<R>{}, std::forward<Args>(args)...} {}

  /*! \brief Copy constructor. */
  constexpr Either(Either const& other) = default;
  /*! \brief Move constructor. */
  constexpr Either(Either&& other) = default;

  /*!
   * \brief  Copy assignment.
   * \param  other The either whose values are copied into this.
   * \return A reference to this.
   */
  auto operator=(Either const& other) & -> Either& = default;

  /*!
   * \brief  Move assignment.
   * \param  other The either whose values are moved into this.
   * \return A reference to this.
   */
  auto operator=(Either&& other) & -> Either& = default;

  /*! \brief Default destructor. */
  ~Either() = default;

  /*!
   * \brief  Returns true if it is a left value.
   * \return True if the left value is contained.
   * \trace  CREQ-171869
   */
  constexpr auto IsLeft() const & noexcept -> bool { return storage_.IsLeft(); }

  /*!
   * \brief  Returns true if it is a right value.
   * \return True if the right value is contained.
   * \trace  CREQ-171869
   */
  constexpr auto IsRight() const & noexcept -> bool { return !this->IsLeft(); }

  /*!
   * \brief  Gets the Left value.
   * \return The left value.
   * \trace  CREQ-171870
   */
  constexpr auto LeftUnsafe() const & noexcept -> L const& { return storage_.Left(); }

  /*!
   * \brief  Gets the Right value.
   * \return The right value.
   * \trace  CREQ-171870
   */
  constexpr auto RightUnsafe() const & noexcept -> R const& { return storage_.Right(); }

  /*!
   * \brief  Gets the Left value.
   * \return The left value.
   * \trace  CREQ-171870
   */
  auto LeftUnsafe() & noexcept -> L& { return const_cast<L&>(static_cast<Either const*>(this)->LeftUnsafe()); }

  /*!
   * \brief  Gets the Right value.
   * \return The right value.
   * \trace  CREQ-171870
   */
  auto RightUnsafe() & noexcept -> R& { return const_cast<R&>(static_cast<Either const*>(this)->RightUnsafe()); }

  /*!
   * \brief  Gets the Left value.
   * \return The left value.
   * \trace  CREQ-171870
   */
  auto LeftUnsafe() && noexcept -> L&& { return std::move(const_cast<L&>(this->LeftUnsafe())); }

  /*!
   * \brief  Gets the Right value.
   * \return The right value.
   * \trace  CREQ-171870
   */
  auto RightUnsafe() && noexcept -> R&& { return std::move(const_cast<R&>(this->RightUnsafe())); }

  /*!
   * \brief  Assign value of type L in place.
   * \tparam Args The types of the values.
   * \param  args The values that will construct the type to be assigned.
   * \return A reference to the constructed value.
   * \trace  CREQ-171868
   */
  template <typename... Args, typename = vac::language::enable_if_t<std::is_constructible<L, Args...>::value>>
  auto EmplaceLeft(Args&&... args) -> L& {
    storage_.EmplaceLeft(std::forward<Args>(args)...);
    return this->LeftUnsafe();
  }

  /*!
   * \brief  Assign value of type R in place.
   * \tparam Args The types of the values.
   * \param  args The values that will construct the type to be assigned.
   * \return A reference to the constructed value.
   * \trace  CREQ-171868
   */
  template <typename... Args, typename = vac::language::enable_if_t<std::is_constructible<R, Args...>::value>>
  auto EmplaceRight(Args&&... args) -> R& {
    storage_.EmplaceRight(std::forward<Args>(args)...);
    return this->RightUnsafe();
  }

 private:
  /*! \brief Container that stores the left or right value. */
  EitherStorage<L, R> storage_;
};

/*!
 * \brief  Checks for equality for eithers.
 * \param  a The first either to compare.
 * \param  b The second either to compare.
 * \return True if both eithers are the same, else false.
 */
template <typename L, typename R>
constexpr auto operator==(Either<L, R> const& a, Either<L, R> const& b) -> bool {
  /* VECTOR Disable AutosarC++17_10-A5.0.1: MD_VAC_A5.0.1_UnsequencedFunctionCalls */
  return a.IsLeft() ? (b.IsLeft() ? a.LeftUnsafe() == b.LeftUnsafe() : false)
                    : (b.IsRight() ? a.RightUnsafe() == b.RightUnsafe() : false);
  /* VECTOR Enable AutosarC++17_10-A5.0.1 */
}
}  // namespace detail
}  // namespace language
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_LANGUAGE_DETAIL_EITHER_H_
