/**********************************************************************************************************************
 *  COPYRIGHT
 *  -------------------------------------------------------------------------------------------------------------------
 *  \verbatim
 *  Copyright (c) 2020 by Vector Informatik GmbH. All rights reserved.
 *
 *                This software is copyright protected and proprietary to Vector Informatik GmbH.
 *                Vector Informatik GmbH grants to you only those rights as set out in the license conditions.
 *                All other rights remain with Vector Informatik GmbH.
 *  \endverbatim
 *  -------------------------------------------------------------------------------------------------------------------
 *  FILE DESCRIPTION
 *  -----------------------------------------------------------------------------------------------------------------*/
/*!        \file    vac/language/switch.h
 *         \brief   Implements a switch that works for non primitive types.
 *         \details Switch that works on complex data and may be used as expression.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_LANGUAGE_SWITCH_H_
#define LIB_VAC_INCLUDE_VAC_LANGUAGE_SWITCH_H_

/*********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <functional>
#include <type_traits>
#include <utility>

#include "ara/core/optional.h"

namespace vac {
namespace language {

/*!
 * \brief   A switch 'expression' for complex data.
 * \details May be used on any type that has an operator== implementation.
 * \tparam  Out The output type for the switch expression.
 * \tparam  In The type on which the switch operates.
 */
template <typename Out, typename In>
class SwitchType {
 public:
  /*! \brief Alias for In const */
  using InConst = std::add_const_t<In>;

  /*!
   * \brief Constructs a switch 'expression' from the value to match on.
   * \param in The value to match on.
   */
  explicit SwitchType(InConst& in) noexcept : out_{}, in_{in} {}

  /*!
   * \brief  A case 'expression' for the current switch.
   * \tparam Supplier The function type for the value producer.
   * \param  matcher The case value.
   * \param  supplier Returns the data if the case matches.
   * \return The Switch itself to enable chaining.
   * \trace  CREQ-LibVac-SwitchLambda
   */
  template <typename Supplier, typename SupplierResult = std::result_of_t<Supplier()>,
            std::enable_if_t<std::is_constructible<Out, SupplierResult>::value, int> = 0>
  auto Case(InConst& matcher, Supplier&& supplier) && -> SwitchType&& {
    if (!out_ && in_.get() == matcher) {
      out_.emplace(std::forward<SupplierResult>(supplier()));
    }
    return std::move(*this);
  }

  /*!
   * \brief  A case 'expression' for the current switch.
   * \param  matcher The case value.
   * \param  value Returns the data if the case matches.
   * \return The Switch itself to enable chaining.
   * \trace  CREQ-LibVac-SwitchLiteral
   */
  template <typename Out1, std::enable_if_t<std::is_constructible<Out, Out1>::value, int> = 0>
  auto Case(InConst& matcher, Out1&& value) && -> SwitchType&& {
    if (!out_ && in_.get() == matcher) {
      out_.emplace(std::forward<Out1>(value));
    }
    return std::move(*this);
  }

  /*!
   * \brief  A default case 'expression' for the current switch.
   * \tparam Supplier The function type for the value producer.
   * \param  supplier Returns the data if no other case matched.
   * \return Either the supplied value or a value produced before.
   * \trace  CREQ-LibVac-SwitchLambda
   */
  template <typename Supplier, typename SupplierResult = std::result_of_t<Supplier()>,
            std::enable_if_t<std::is_constructible<Out, SupplierResult>::value, int> = 0>
  auto Default(Supplier&& supplier) && -> Out {
    return out_ ? *(std::move(this)->out_) : static_cast<Out>(std::forward<SupplierResult>(supplier()));
  }

  /*!
   * \brief  A default case 'expression' for the current switch.
   * \param  value Returns the data if no other case matched.
   * \return Either the supplied value or a value produced before.
   * \trace  CREQ-LibVac-SwitchLiteral
   */
  template <typename Out1, std::enable_if_t<std::is_constructible<Out, Out1>::value, int> = 0>
  auto Default(Out1&& value) && -> Out {
    return out_ ? *(std::move(this)->out_) : static_cast<Out>(std::forward<Out1>(value));
  }

  /*!
   * \brief  Gets the unexhaustive result.
   * \return Either the produced value or an empty optional.
   * \trace  CREQ-LibVac-SwitchOptional
   */
  auto Get() && -> ara::core::Optional<Out> { return std::move(this)->out_; }

 private:
  ara::core::Optional<Out> out_{};     /*!< \brief The container for the return value. */
  std::reference_wrapper<InConst> in_; /*!< \brief The reference for the switch value. */
};

/*!
 * \brief   A switch 'statement' for complex data.
 * \details May be used on any type that has an operator== implementation.
 * \tparam  In The type on which the switch operates.
 */
template <typename In>
class SwitchType<void, In> {
 public:
  /*! \brief Alias for In const */
  using InConst = std::add_const_t<In>;

  /*!
   * \brief Constructs a switch 'statement' from the value to match on.
   * \param in The value to match on.
   */
  explicit SwitchType(InConst& in) noexcept : in_{in}, need_match_{true} {}

  /*!
   * \brief  A case 'statement' for the current switch.
   * \tparam Effect The function type that produces the side effect.
   * \param  matcher The case value.
   * \param  effect Performs the side effect if the case matches.
   * \return The Switch itself to enable chaining.
   * \trace  CREQ-LibVac-SwitchSideEffect
   */
  template <typename Effect>
  auto Case(InConst& matcher, Effect&& effect) && -> SwitchType&& {
    if (need_match_ && in_.get() == matcher) {
      need_match_ = false;
      effect();
    }
    return std::move(*this);
  }

  /*!
   * \brief  A default case 'statement' for the current switch.
   * \tparam Effect The function type that produces the side effect.
   * \param  effect Performs the side effect if no other case matched.
   * \return Either the supplied value or a value produced before.
   * \trace  CREQ-LibVac-SwitchSideEffect
   */
  template <typename Effect>
  auto Default(Effect&& effect) && -> void {
    if (need_match_) {
      need_match_ = false;
      effect();
    }
  }

 private:
  std::reference_wrapper<InConst> in_; /*!< \brief The reference for the switch value. */
  bool need_match_;                    /*!< \brief Marker if further case statements need to be considered. */
};

/*!
 * \brief   Constructs a switch 'expression'/'statement' from the value to match on.
 * \details May be used on any type that has an operator== implementation.
 * \tparam  Out The output type for the switch expression.
 * \tparam  In The type on which the switch operates.
 * \param   in The value to match on.
 * \return  A switch 'expression'/'statement'.
 * \trace   CREQ-LibVac-SwitchComplexTypes
 */
template <typename Out = void, typename In>
auto Switch(In const& in) -> SwitchType<Out, In> {
  return SwitchType<Out, In>{in};
}

}  // namespace language
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_LANGUAGE_SWITCH_H_
