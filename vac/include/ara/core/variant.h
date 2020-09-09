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
/*!        \file  ara/core/variant.h
 *        \brief  Contains the implementation of variant (tagged union) type SWS core type ara::core::Variant.
 *
 *      \details  A type-safe union that behaves like std::variant from C++17.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_ARA_CORE_VARIANT_H_
#define LIB_VAC_INCLUDE_ARA_CORE_VARIANT_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <functional>
#include <limits>
#include <tuple>
#include <utility>

#include "ara/core/utility.h"
#include "vac/language/compile_time.h"
#include "vac/language/cpp17_backport.h"
#include "vac/language/cpp20_backport.h"
#include "vac/language/throw_or_terminate.h"

namespace ara {
namespace core {

// Forward declaration for Variant.
template <typename... Xs>
class Variant;

/*!
 * \brief Provides compile-time index access to the Variant types.
 * \vpublic
 */
template <std::size_t I, typename T>
struct variant_alternative;

/*!
 * \brief Provides compile-time index access to the Variant types.
 * \vpublic
 */
template <std::size_t I, typename T>
struct variant_alternative<I, T const> : std::add_const<typename variant_alternative<I, T>::type> {};

/* VECTOR Next Construct VectorC++-V7-1-10: MD_VAC_V7-1-10_volatile */
/*!
 * \brief Provides compile-time index access to the Variant types.
 * \vpublic
 */
template <std::size_t I, typename T>
struct variant_alternative<I, T volatile> : std::add_volatile<typename variant_alternative<I, T>::type> {};

/* VECTOR Next Construct VectorC++-V7-1-10: MD_VAC_V7-1-10_volatile */
/*!
 * \brief Provides compile-time index access to the Variant types.
 * \vpublic
 */
template <std::size_t I, typename T>
struct variant_alternative<I, T const volatile> : std::add_cv<typename variant_alternative<I, T>::type> {};

/*!
 * \brief Provides compile-time index access to the Variant types.
 * \vpublic
 */
template <std::size_t I, typename... Types>
struct variant_alternative<I, Variant<Types...>>
    : vac::language::type_identity<vac::language::compile_time::get_from_index<I, Types...>> {};

/*!
 * \brief Provides compile-time index access to the Variant types.
 * \vpublic
 */
template <std::size_t I, typename T>
using variant_alternative_t = typename variant_alternative<I, T>::type;

/*!
 * \brief Unit type intended for use as a well-behaved empty alternative in Variant.
 * \vpublic
 */
struct monostate {};

/*! \brief Operator overload for monostate. */
constexpr bool operator<(monostate, monostate) noexcept { return false; }

/*! \brief Operator overload for monostate. */
constexpr bool operator>(monostate, monostate) noexcept { return false; }

/*! \brief Operator overload for monostate. */
constexpr bool operator<=(monostate, monostate) noexcept { return true; }

/*! \brief Operator overload for monostate. */
constexpr bool operator>=(monostate, monostate) noexcept { return true; }

/*! \brief Operator overload for monostate. */
constexpr bool operator==(monostate, monostate) noexcept { return true; }

/*! \brief Operator overload for monostate. */
constexpr bool operator!=(monostate, monostate) noexcept { return false; }

/* VECTOR Next Construct AutosarC++17_10-A12.4.1: MD_VAC_A12.4.1_destructorOfABaseClassShallBePublicVirtual */
/*!
 * \brief Provides access to the number of alternatives in a possibly cv-qualified Variant as a compile-time constant
 *        expression.
 * \vpublic
 */
template <typename T>
struct variant_size;

/*!
 * \brief Provides access to the number of alternatives in a possibly cv-qualified Variant as a compile-time constant
 *        expression.
 * \vpublic
 */
template <typename... Types>
struct variant_size<Variant<Types...>> : std::integral_constant<std::size_t, sizeof...(Types)> {};
/*!
 * \brief Provides access to the number of alternatives in a possibly cv-qualified Variant as a compile-time constant
 *        expression.
 * \vpublic
 */
template <typename T>
struct variant_size<const T> : variant_size<T> {};
/* VECTOR Next Construct VectorC++-V7-1-10: MD_VAC_V7-1-10_volatile */
/*!
 * \brief Provides access to the number of alternatives in a possibly cv-qualified Variant as a compile-time constant
 *        expression.
 * \vpublic
 */
template <typename T>
struct variant_size<volatile T> : variant_size<T> {};
/* VECTOR Next Construct VectorC++-V7-1-10: MD_VAC_V7-1-10_volatile */
/*!
 * \brief Provides access to the number of alternatives in a possibly cv-qualified Variant as a compile-time constant
 *        expression.
 * \vpublic
 */
template <typename T>
struct variant_size<const volatile T> : variant_size<T> {};

/* VECTOR Next Construct AutosarC++17_10-M0.1.4: MD_VAC_M0.1.4_staticFieldInitializedButNotReferenced */
/* VECTOR Next Construct AutosarC++17_10-M3.4.1: MD_VAC_M3.4.1_globalTemplateVariable */
/* VECTOR Next Construct AutosarC++17_10-M17.0.2: MD_VAC_M17.0.2_standardLibraryObjectNameOverride */
/*!
 * \brief Provides access to the number of alternatives in a possibly cv-qualified Variant as a compile-time constant
 *        expression.
 * \vpublic
 */
template <typename T>
constexpr size_t variant_size_v = variant_size<T>::value;

// Forward declaration for holds alternative
template <typename T, typename... Types>
bool holds_alternative(const Variant<Types...>& v) noexcept;

// Forward declaration for get.
template <std::size_t N, typename... Types>
variant_alternative_t<N, Variant<Types...>>& get(Variant<Types...>& v) noexcept(false);
template <std::size_t N, typename... Types>
variant_alternative_t<N, Variant<Types...>>&& get(Variant<Types...>&& v) noexcept(false);
template <std::size_t N, typename... Types>
variant_alternative_t<N, Variant<Types...>> const& get(Variant<Types...> const& v) noexcept(false);
template <std::size_t N, typename... Types>
variant_alternative_t<N, Variant<Types...>> const&& get(Variant<Types...> const&& v) noexcept(false);

template <std::size_t N, typename... Types>
variant_alternative_t<N, Variant<Types...>>* get_if(Variant<Types...>* v) noexcept(false);
template <std::size_t N, typename... Types>
variant_alternative_t<N, Variant<Types...>> const* get_if(Variant<Types...> const* v) noexcept(false);
template <typename T, typename... Types>
auto get_if(Variant<Types...>* v) noexcept(false) -> std::add_pointer_t<T>;
template <typename T, typename... Types>
auto get_if(Variant<Types...> const* v) noexcept(false) -> std::add_pointer_t<T const>;

/*!
 * \brief   Exception type that will be thrown if any invalid access will be made.
 * \details See C++17 specification.
 * \vpublic
 */
class bad_variant_access final : public std::exception {
 public:
  /*!
   * \brief Default constructor.
   */
  bad_variant_access() noexcept = default;
  /* VECTOR Next Construct AutosarC++17_10-A3.9.1: MD_VAC_A3.9.1_useOfBasetypeOutsideTypedefs */
  /*!
   * \brief  Needed method to display error message.
   * \return A message for debugging purposes.
   */
  char const* what() const noexcept final { return message; }

 private:
  /* VECTOR Next Construct AutosarC++17_10-A3.9.1: MD_VAC_A3.9.1_useOfBasetypeOutsideTypedefs */
  constexpr static char const* message{"Bad visit / get access"};
};

/* VECTOR Next Construct AutosarC++17_10-M17.0.2: MD_VAC_M17.0.2_standardLibraryObjectNameOverride */
/*!
 * \brief Marker for an invalid element in Variant.
 * \vpublic
 */
constexpr std::size_t variant_npos{std::numeric_limits<std::size_t>::max()};

namespace detail {

/*!
 * \brief Implementation of comparisons for variants.
 * \vprivate
 */
template <typename... Types>
class VariantCompare {
  /*!
   * \brief Type of the variants that are compared.
   */
  using variant_t = Variant<Types...> const&;

 public:
  /*!
   * \brief The constructor.
   * \param v1 The first Variant to compare.
   * \param v2 The second Variant to compare.
   */
  VariantCompare(variant_t v1, variant_t v2) : v1_{v1}, v2_{v2} {}

  /*!
   * \brief  Function that checks for equality of 2 variants.
   * \return True if both variants are the same.
   */
  bool equal() && {
    bool ret_value;
    if (v1_.index() != v2_.index()) {
      ret_value = false;
    } else if (v1_.valueless_by_exception()) {
      ret_value = true;
    } else {
      ret_value = compare_internal<std::equal_to, Types...>();
    }
    return ret_value;
  }

  /*!
   * \brief  Function that checks if one Variant is smaller than the other.
   * \return True if v1 is smaller than v2.
   */
  bool is_smaller() && {
    bool ret_value;
    if (v2_.valueless_by_exception()) {
      ret_value = false;
    } else if (v1_.valueless_by_exception()) {
      ret_value = true;
    } else if (v1_.index() < v2_.index()) {
      ret_value = true;
    } else if (v1_.index() > v2_.index()) {
      ret_value = false;
    } else {
      ret_value = compare_internal<std::less, Types...>();
    }
    return ret_value;
  }

 private:
  /*!
   * \brief        Generic compare function that takes an operator like std::less.
   * \tparam       Comp The function that compares both items.
   * \return       True if the comparison function returns true.
   * \precondition Both variants are valid and have values of the same type.
   */
  template <template <typename> class Comp, typename Cand>
  bool compare_internal() const noexcept(false) {
    return Comp<Cand>{}(*v1_.template get_if<Cand>(), *v2_.template get_if<Cand>());
  }

  /*!
   * \brief        Generic compare function that takes an operator like std::less.
   * \tparam       Comp The function that compares both items.
   * \return       True if the comparison function returns true for the correct alternative.
   * \precondition Both variants are valid and have values of the same type.
   */
  template <template <typename> class Comp, typename Cand, typename Cand2, typename... Cands>
  bool compare_internal() const noexcept(false) {
    bool ret_value;
    if (ara::core::holds_alternative<Cand>(v1_)) {
      ret_value = compare_internal<Comp, Cand>();
    } else {
      ret_value = compare_internal<Comp, Cand2, Cands...>();
    }
    return ret_value;
  }

  /*!
   * \brief The first Variant to compare.
   */
  variant_t v1_;
  /*!
   * \brief The second Variant to compare.
   */
  variant_t v2_;
};

/*!
 * \brief   Internal helper for visiting Variant items.
 * \details Specialization for invalid instructions.
 * \return
 * \vprivate
 */
template <typename Ret, typename Visitor, typename Variant>
[[noreturn]] auto TryVisitVariant(Visitor&&, Variant &&) -> Ret {
  vac::language::ThrowOrTerminate<bad_variant_access>();
}

/* VECTOR Next Construct AutosarC++17_10-A13.3.1: MD_VAC_A13.3.1_forwardingFunctionsShallNotBeOverloaded */
/*!
 * \brief   Internal helper for visiting Variant items.
 * \details Specialization that will dispatch visiting the correct non-pointer type.
 * \param   visitor The visitor accepting the stored value.
 * \param   v The visited Variant.
 * \return  Result of the functor call.
 * \vprivate
 */
template <typename Ret, typename Visitor, typename Variant, typename T, typename... Types>
auto TryVisitVariant(Visitor&& visitor, Variant&& v) noexcept(false) -> Ret {
  std::add_pointer_t<std::conditional_t<std::is_const<std::remove_reference_t<Variant>>::value, T const, T>>
      maybe_pointer{ara::core::get_if<T>(&v)};
  return (maybe_pointer != nullptr) ? std::forward<Visitor>(visitor).operator()(*maybe_pointer)
                                    : TryVisitVariant<Ret, Visitor, Variant, Types...>(std::forward<Visitor>(visitor),
                                                                                       std::forward<Variant>(v));
}

/*!
 * \brief  Helper for visiting Variant items.
 * \param  visitor The visitor accepting the stored value.
 * \param  v The visited Variant.
 * \return Result of the functor call.
 * \vprivate
 */
template <typename Visitor, typename Variant, typename... Types>
auto VisitVariant(Visitor&& visitor, Variant&& v) noexcept(false)
    -> decltype(visitor.operator()(*ara::core::get_if<0>(&v))) {
  /*! \brief Local reference type alias. */
  using RetType = decltype(visitor.operator()(*ara::core::get_if<0>(&v)));
  return TryVisitVariant<RetType, Visitor, Variant, Types...>(std::forward<Visitor>(visitor), std::forward<Variant>(v));
}

/*!
 * \brief Visitor for swapping variants holding same type.
 * \vprivate
 */
template <typename Variant>
class SwapWithVisitor {
 public:
  /*!
   * \brief Constructor.
   */
  explicit SwapWithVisitor(Variant& other) : other_{other} {}

  /*!
   * \brief Swaps value with other.
   */
  template <typename T>
  void operator()(T& v) noexcept(false) {
    using std::swap;
    swap(v, get<T>(other_));
  }

 private:
  /*!
   * \brief Other Variant with common type.
   */
  Variant& other_;
};

/* VECTOR Next Construct AutosarC++17_10-A12.4.1: MD_VAC_A12.4.1_destructorOfABaseClassShallBePublicVirtual */
/*!
 * \brief Overload set for types Xs used for correct overload resolution in constructor template.
 * \vprivate
 */
template <typename... Ts>
struct OverloadSet;

/* VECTOR Next Construct AutosarC++17_10-A12.4.1: MD_VAC_A12.4.1_destructorOfABaseClassShallBePublicVirtual */
/*!
 * \brief Recursion tail template without any type arguments.
 * \vprivate
 */
template <>
class OverloadSet<> {
 public:
  /* VECTOR Next Construct AutosarC++17_10-M3.2.4: MD_VAC_M3.2.4_functionWithoutDefinition */
  /*!
   * \brief Base overload to allow "using" in subclasses.
   */
  static void overload_value();
};

/*!
 * \brief Adds an overload for each type in the template list.
 * \vprivate
 */
template <typename T, typename... Ts>
class OverloadSet<T, Ts...> : OverloadSet<Ts...> {
 public:
  using OverloadSet<Ts...>::overload_value;
  /*!
   * \brief Overload for type T, calculate the index of T and Ts in reverse way so that the index is in ascending order.
   */
  static std::integral_constant<size_t, sizeof...(Ts)> overload_value(T);
};

/* VECTOR Next Construct AutosarC++17_10-M0.1.5: MD_VAC_M0.1.5_UnusedTypeDeclaration */
/*!
 * \brief Helper struct to check if the AcceptedIndex is actually acceptable.
 * \vprivate
 */
template <typename T, typename Variant, typename = void>
struct IsAcceptable : std::false_type {};

/* VECTOR Next Construct AutosarC++17_10-M0.1.5: MD_VAC_M0.1.5_UnusedTypeDeclaration */
/*!
 * \brief Helper struct to check if the type T is implicitly convertible to any of the Variant types Ts...
 * \vprivate
 */
template <typename T, typename... Ts>
struct IsAcceptable<
    T, Variant<Ts...>,
    std::enable_if_t<!std::is_void<decltype(OverloadSet<Ts...>::overload_value(std::declval<T>()))>::value>>
    : std::true_type {};

/*!
 * \brief Accepted index for type X and Variant type in constructor template.
 * \vprivate
 */
template <typename T, typename Variant, bool = IsAcceptable<T, Variant>::value>
struct AcceptedIndex : std::integral_constant<std::size_t, variant_npos> {};

/*!
 * \brief Adds an index for each type in the template list.
 * \vprivate
 */
template <typename T, typename... Ts>
struct AcceptedIndex<T, Variant<Ts...>, true> {
  /*!
   * \brief Calculate index from overload_value(T).
   */
  static constexpr std::size_t value{sizeof...(Ts) - 1 -
                                     decltype(OverloadSet<Ts...>::overload_value(std::declval<T>()))::value};
};

}  // namespace detail

/*!
 * \brief   A type-safe union as specified in C++17.
 * \details See 'tagged-union' or 'discriminated union'.
 * \trace   CREQ-158604
 * \vpublic
 */
template <typename... Xs>
class Variant final {
  /*!
   * \brief Max size of containable types.
   */
  constexpr static std::size_t storage_size{vac::language::compile_time::get_max_sizeof<Xs...>()};
  /*!
   * \brief Max alignment of containable types.
   */
  constexpr static std::size_t storage_alignment{vac::language::compile_time::get_max_alignof<Xs...>()};

  /*!
   * \brief Unified storage type.
   */
  using Storage = typename std::aligned_storage<storage_size, storage_alignment>::type;

  // Assertions
  static_assert(vac::language::compile_time::not_any<std::is_void<Xs>::value...>::value,
                "Variant cannot store void type (use monotype)");
  static_assert(vac::language::compile_time::not_any<std::is_array<Xs>::value...>::value,
                "Variant cannot store C Arrays");
  static_assert(vac::language::compile_time::not_any<std::is_reference<Xs>::value...>::value,
                "Variant can only contain values (use std::ref)");
  static_assert(!vac::language::compile_time::has_duplicate<Xs...>::value, "There may not be any duplicate types!");

  /*!
   * \brief Collection of traits useful for Variant.
   * \vprivate
   */
  template <typename X>
  class VariantTrait {
   public:
    /*!
     * \brief Value type that can be returned.
     */
    using decayed = typename std::decay<X>::type;

    /*!
     * \brief The intuitive index for type X given types Xs of the Variant.
     */
    static constexpr std::size_t const accepted_index{detail::AcceptedIndex<X&&, Variant>::value};

    /*!
     * \brief The index of the underlying type.
     */
    static constexpr std::size_t const index{vac::language::compile_time::get_index<decayed, Xs...>()};

    /*!
     * \brief Checks if variants default constructor may throw.
     */
    static constexpr bool is_nothrow_default_constructible{std::is_nothrow_default_constructible<X>::value};
    /* VECTOR Next Construct AutosarC++17_10-M0.1.4: MD_VAC_M0.1.4_staticFieldInitializedButNotReferenced */
    /*!
     * \brief Checks if Variant may have a default constructor.
     */
    static constexpr bool is_default_constructible{std::is_default_constructible<X>::value};
    /* VECTOR Next Construct AutosarC++17_10-M0.1.4: MD_VAC_M0.1.4_staticFieldInitializedButNotReferenced */
    /*!
     * \brief Checks if type X is the same as the type of Variant<Xs...>.
     */
    static constexpr bool is_same_variant{std::is_same<std::decay_t<X>, Variant>::value};
  };

 public:
  /* VECTOR Disable AutosarC++17_10-A12.1.5: MD_VAC_A12.1.5_useDelegatingConstructor */
  /*!
   * \brief Default constructor that is only usable if the first specified type is default constructible.
   * \vpublic
   */
  template <typename Default = variant_alternative_t<0, Variant>, typename Trait = VariantTrait<Default>,
            typename = typename std::enable_if<Trait::is_default_constructible>::type>
  Variant() noexcept(Trait::is_nothrow_default_constructible) : storage_{}, variant_index_{Trait::index} {
    new (&storage_) Default();
  }

  /*!
   * \brief  Constructor from universal reference type.
   * \param  value The value that is stored inside the Variant.
   * \tparam X Type to be stored inside the Variant.
   * \vpublic
   */
  template <typename X, typename Trait = VariantTrait<X>,
            typename = typename std::enable_if<!Trait::is_same_variant>::type>
  explicit Variant(X&& value) noexcept(false)
      : Variant(in_place_type_t<variant_alternative_t<Trait::accepted_index, Variant>>(), std::forward<X>(value)) {}

  /* VECTOR Next Construct AutosarC++17_10-M4.5.1: MD_VAC_M4.5.1_boolOperandInNew */
  /*!
   * \brief  Emplace constructor.
   * \param  args Arguments of construction.
   * \tparam type That shall be emplace into the Variant.
   * \vpublic
   */
  template <typename X, typename Trait = VariantTrait<X>, typename... Args>
  explicit Variant(in_place_type_t<X>, Args&&... args) noexcept(false)
      : storage_{}, variant_index_{Trait::accepted_index} {
    /* VECTOR Next Line AutosarC++17_10-M5.2.12: MD_VAC_M5.2.12_cStyleArrayNotSupported */
    new (&storage_) typename Trait::decayed(std::forward<Args>(args)...);
  }

  /*!
   * \brief Copy constructor.
   * \param other The Variant from which to construct.
   * \vpublic
   */
  Variant(Variant const& other) : storage_{}, variant_index_{other.variant_index_} {
    copy_storage<Xs...>(other.variant_index_, other.storage_);
  }

  /* VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate */
  /* VECTOR Next Construct AutosarC++17_10-A12.8.4: MD_VAC_A12.8.4_moveConstructorShallNotUseCopySemantics */
  /*!
   * \brief Move constructor.
   * \param other The Variant from which to construct.
   * \vpublic
   */
  Variant(Variant&& other) : storage_{}, variant_index_{other.variant_index_} {
    if (!other.valueless_by_exception()) {
      move_storage<Xs...>(other.variant_index_, std::move(other.storage_));
    }
  }

  /* VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate */
  /* VECTOR Next Construct AutosarC++17_10-M0.1.9: MD_VAC_M0.1.9_destructorStatementWithoutSideEffect */
  /*!
   * \brief   Destructor.
   * \details Will call destructor of contained value.
   * \vpublic
   */
  ~Variant() {
    if (is_valid()) {
      destroy_storage<Xs...>(variant_index_);
      variant_index_ = variant_npos;
    }
  }

  /* VECTOR Next Construct AutosarC++17_10-A13.3.1: MD_VAC_A13.3.1_forwardingFunctionsShallNotBeOverloaded */
  /*!
   * \brief  Copy assignment.
   * \param  other The value that is to be copied.
   * \return A reference on the copied-to Variant.
   * \vpublic
   */
  Variant& operator=(Variant const& other) & {
    if (this != &other) {
      prepare_assignment(other.variant_index_);
      copy_storage<Xs...>(other.variant_index_, other.storage_);
    }
    return *this;
  }

  /* VECTOR Next Construct AutosarC++17_10-A13.3.1: MD_VAC_A13.3.1_forwardingFunctionsShallNotBeOverloaded */
  /*!
   * \brief  Move assignment.
   * \param  other The value that is to be moved.
   * \return A reference on the moved-to Variant.
   * \vpublic
   */
  Variant& operator=(Variant&& other) & {
    if (this != &other) {
      prepare_assignment(other.variant_index_);
      move_storage<Xs...>(other.variant_index_, std::move(other.storage_));
    }
    return *this;
  }

  /*!
   * \brief  Variant alternative assignment.
   * \param  value The value that is to be assigned.
   * \return A reference on the assigned-to Variant.
   * \vpublic
   */
  template <typename X, typename Trait = VariantTrait<X>,
            typename = typename std::enable_if<!Trait::is_same_variant>::type,
            typename AcceptedType = variant_alternative_t<VariantTrait<X>::accepted_index, Variant>,
            typename = typename std::enable_if<std::is_constructible<AcceptedType, X&&>::value>::type,
            typename = typename std::enable_if<std::is_assignable<AcceptedType&, X&&>::value>::type>
      Variant& operator=(X&& value) & noexcept(false) {
    if (Trait::accepted_index == index()) {
      get<Trait::accepted_index>(*this) = std::forward<X>(value);
    } else {
      emplace<AcceptedType>(std::forward<X>(value));
    }
    return *this;
  }

  /* VECTOR Next Construct AutosarC++17_10-M4.5.1: MD_VAC_M4.5.1_boolOperandInNew */
  /*!
   * \brief  Assign value in place.
   * \param  args The value that is to be moved.
   * \return A reference on the moved-to Variant.
   * \vpublic
   */
  template <typename T, typename... Args, typename = typename std::is_constructible<T, Args...>::type>
  T& emplace(Args&&... args) noexcept(false) {
    prepare_assignment(variant_npos);
    new (&storage_) typename VariantTrait<T>::decayed(std::forward<Args>(args)...);
    variant_index_ = VariantTrait<T>::index;
    return *get_unsafe<T>(storage_);
  }

  /* VECTOR Next Construct AutosarC++17_10-A13.3.1: MD_VAC_A13.3.1_forwardingFunctionsShallNotBeOverloaded */
  /*!
   * \brief  Assign value in place.
   * \param  args The value that is to be moved.
   * \return A reference on the moved-to Variant.
   * \vpublic
   */
  template <size_t I, typename... Args,
            typename = typename std::is_constructible<variant_alternative_t<I, Variant>>::type>
  variant_alternative_t<I, Variant>& emplace(Args&&... args) noexcept(false) {
    return emplace<variant_alternative_t<I, Variant>>(std::forward<Args>(args)...);
  }

  /*!
   * \brief   Get index of the contained alternative.
   * \details If the Variant is not valid, returns the invalid index.
   * \return  The zero-based index of the alternative that is currently held by the Variant.
   * \vpublic
   */
  std::size_t index() const noexcept { return variant_index_; }

  /*!
   * \brief  Checks if the Variant is invalid.
   * \return False if and only if the Variant holds a value.
   * \vpublic
   */
  bool valueless_by_exception() const noexcept { return !is_valid(); }

  /*!
   * \brief  Gets the stored value cast to the specified type without any checks.
   * \return A pointer to the stored value.
   * \vprivate
   */
  template <typename T>
  auto get_if() noexcept -> std::add_pointer_t<T> {
    std::add_pointer_t<T> ret_value{nullptr};
    if (is_valid() && ara::core::holds_alternative<T>(*this)) {
      ret_value = get_unsafe<T>(storage_);
    }
    return ret_value;
  }

  /*!
   * \brief  Gets the stored value cast to the specified type without any checks.
   * \return A pointer to the stored value.
   * \vprivate
   */
  template <typename T>
  auto get_if() const noexcept -> std::add_pointer_t<T const> {
    std::add_pointer_t<T const> ret_value{nullptr};
    if (is_valid() && ara::core::holds_alternative<T>(*this)) {
      ret_value = get_unsafe<T>(storage_);
    }
    return ret_value;
  }

  /* VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate */
  /*!
   * \brief Swaps Variant with other.
   * \param other Variant to swap with.
   * \vpublic
   */
  void swap(Variant& other) noexcept((vac::language::conjunction<vac::language::is_nothrow_swappable<Xs>...>::value) &&
                                     (std::is_nothrow_move_constructible<Variant>::value)) {
    if (this->index() == other.index()) {
      if (this->is_valid()) {
        detail::SwapWithVisitor<Variant> swapper{other};
        visit(swapper, *this);
      }
    } else {
      Variant middle_man{std::move(other)};
      other = std::move(*this);
      *this = std::move(middle_man);
    }
  }

 private:
  /* VECTOR Next Construct AutosarC++17_10-A5.2.4: MD_VAC_A5.2.4_reinterpretCast */
  /*!
   * \brief  Gets the stored value cast to the specified type without any checks.
   * \param  storage The storage from which to get the value from.
   * \return A pointer to the stored value.
   */
  template <typename T>
  static auto get_unsafe(Storage& store) noexcept -> std::add_pointer_t<T> {
    static_assert(!std::is_same<T, void>::value, "T should not be void");
    static_assert(vac::language::compile_time::any_in<T, Xs...>() ||
                      vac::language::compile_time::any_in<std::decay_t<T>, Xs...>(),
                  "T is not contained in this Variant!");
    return reinterpret_cast<std::add_pointer_t<T>>(&store);
  }

  /* VECTOR Next Construct AutosarC++17_10-A5.2.4: MD_VAC_A5.2.4_reinterpretCast */
  /*!
   * \brief  Gets the stored value cast to the specified type without any checks.
   * \param  storage The storage from which to get the value from.
   * \return A pointer to the stored value.
   */
  template <typename T>
  static auto get_unsafe(Storage const& store) noexcept -> std::add_pointer_t<T const> {
    static_assert(!std::is_same<T, void>::value, "T should not be void");
    static_assert(vac::language::compile_time::any_in<T, Xs...>() ||
                      vac::language::compile_time::any_in<std::decay_t<T>, Xs...>(),
                  "T is not contained in this Variant!");
    return reinterpret_cast<std::add_pointer_t<T const>>(&store);
  }

  /*!
   * \brief  A Variant that is valueless by exception is treated as being in an invalid state.
   * \return True if and only if the Variant holds a value.
   */
  bool is_valid() const noexcept { return variant_index_ != variant_npos; }

  /* VECTOR Next Construct AutosarC++17_10-M0.1.9: MD_VAC_M0.1.9_destructorStatementWithoutSideEffect */
  /*!
   * \brief Prepares for copy/move assignment.
   * \param new_index The index of the new stored type.
   */
  void prepare_assignment(std::size_t new_index) {
    if (is_valid()) {
      destroy_storage<Xs...>(variant_index_);
    }
    variant_index_ = new_index;
  }

  /* VECTOR Next Construct AutosarC++17_10-M4.5.1: MD_VAC_M4.5.1_boolOperandInNew */
  /* VECTOR Next Construct AutosarC++17_10-M0.1.8: MD_VAC_M0.1.8_voidFunctionHasNoExternalSideEffect */
  /*!
   * \brief Move the stored value from that to this.
   * \param storage The data storage from which to move.
   */
  template <typename Hd>
  void move_storage(std::size_t, Storage&& store) noexcept(std::is_nothrow_move_constructible<Hd>::value) {
    new (&storage_) Hd(std::move(*get_unsafe<Hd>(store)));
  }

  /*!
   * \brief Move the stored value from that to this.
   * \param offset The type offset.
   * \param storage The data storage from which to move.
   */
  template <typename Hd, typename Tl1, typename... Tl>
  void move_storage(std::size_t offset, Storage&& store) noexcept(false) {
    if (offset == 0) {
      move_storage<Hd>(0, std::move(store));
    } else {
      move_storage<Tl1, Tl...>(--offset, std::move(store));
    }
  }

  /* VECTOR Next Construct AutosarC++17_10-M0.1.8: MD_VAC_M0.1.8_voidFunctionHasNoExternalSideEffect */
  /*!
   * \brief Copy the stored value from that to this.
   * \param storage The data storage from which to copy.
   */
  template <typename Hd>
  void copy_storage(std::size_t, Storage const& storage) noexcept(false) {
    new (&storage_) Hd(*get_unsafe<Hd>(storage));
  }

  /*!
   * \brief Copy the stored value from that to this.
   * \param offset The type offset.
   * \param store The data storage from which to copy.
   */
  template <typename Hd, typename Tl1, typename... Tl>
  void copy_storage(std::size_t offset, Storage const& store) noexcept(false) {
    if (offset == 0) {
      copy_storage<Hd>(0, store);
    } else {
      copy_storage<Tl1, Tl...>(--offset, store);
    }
  }

  /* VECTOR Next Construct AutosarC++17_10-M0.1.8: MD_VAC_M0.1.8_destructorHasNoExternalSideEffect */
  /*!
   * \brief Destroys the stored value.
   */
  template <typename Hd>
  void destroy_storage(std::size_t) noexcept(false) {
    get_unsafe<Hd>(storage_)->~Hd();
  }

  /* VECTOR Next Construct AutosarC++17_10-M0.1.8: MD_VAC_M0.1.8_destructorHasNoExternalSideEffect */
  /* VECTOR Next Construct AutosarC++17_10-M0.1.9: MD_VAC_M0.1.9_destructorStatementWithoutSideEffect */
  /*!
   * \brief Destroys the stored value.
   * \param offset The type offset.
   */
  template <typename Hd, typename Tl1, typename... Tl>
  void destroy_storage(std::size_t offset) noexcept(false) {
    if (offset == 0) {
      destroy_storage<Hd>(0);
    } else {
      destroy_storage<Tl1, Tl...>(--offset);
    }
  }

  /*!
   * \brief The actual storage.
   */
  Storage storage_;
  /*!
   * \brief Type-index in which the item is contained.
   */
  std::size_t variant_index_;
};

/* VECTOR Next Construct AutosarC++17_10-A2.11.5: MD_VAC_A2.11.5_nameOfObjectOrFunctionReused */
/*!
 * \brief  Checks if the contained value is of the passed type.
 * \param  v The Variant that shall be tested.
 * \return If the Variant contains a value of the passed type.
 * \vpublic
 */
template <typename T, typename... Types>
bool holds_alternative(Variant<Types...> const& v) noexcept {
  return vac::language::compile_time::get_index<T, Types...>() == v.index();
}

/* VECTOR Next Construct AutosarC++17_10-A2.11.5: MD_VAC_A2.11.5_nameOfObjectOrFunctionReused */
/*!
 * \brief  Get Variant value from index as pointer.
 * \param  v The Variant which value shall be returned.
 * \return The value that is contained or nullptr.
 * \vpublic
 */
template <std::size_t N, typename... Types>
variant_alternative_t<N, Variant<Types...>>* get_if(Variant<Types...>* v) noexcept(false) {
  static_assert(N < sizeof...(Types), "Out of bounds access!");
  return ara::core::get_if<variant_alternative_t<N, Variant<Types...>>, Types...>(v);
}

/*!
 * \brief  Get Variant value from index as pointer.
 * \param  v The Variant which value shall be returned.
 * \return The value that is contained or nullptr.
 * \vpublic
 */
template <std::size_t N, typename... Types>
variant_alternative_t<N, Variant<Types...>> const* get_if(Variant<Types...> const* v) noexcept(false) {
  static_assert(N < sizeof...(Types), "Out of bounds access!");
  return ara::core::get_if<variant_alternative_t<N, Variant<Types...>>, Types...>(v);
}

/*!
 * \brief  Get Variant value from type as pointer.
 * \param  v The Variant which value shall be returned.
 * \return The value that is contained or nullptr.
 * \vpublic
 */
template <typename T, typename... Types>
auto get_if(Variant<Types...>* v) noexcept(false) -> std::add_pointer_t<T> {
  std::add_pointer_t<T> ret_value{nullptr};
  if (v != nullptr) {
    ret_value = v->template get_if<T>();
  }
  return ret_value;
}

/*!
 * \brief  Get Variant value from type as pointer.
 * \param  v The Variant which value shall be returned.
 * \return The value that is contained or nullptr.
 * \vpublic
 */
template <typename T, typename... Types>
auto get_if(Variant<Types...> const* v) noexcept(false) -> std::add_pointer_t<T const> {
  std::add_pointer_t<T const> ret_value{nullptr};
  if (v != nullptr) {
    ret_value = v->template get_if<T>();
  }
  return ret_value;
}

/* VECTOR Next Construct AutosarC++17_10-A2.11.5: MD_VAC_A2.11.5_nameOfObjectOrFunctionReused */
/*!
 * \brief  Get Variant value from index.
 * \param  v The Variant which value shall be returned.
 * \return The value that is contained.
 * \vpublic
 */
template <std::size_t N, typename... Types>
variant_alternative_t<N, Variant<Types...>>& get(Variant<Types...>& v) noexcept(false) {
  static_assert(N < sizeof...(Types), "Out of bounds access!");
  return get<variant_alternative_t<N, Variant<Types...>>, Types...>(v);
}

/*!
 * \brief  Get Variant value from index.
 * \param  v The Variant which value shall be returned.
 * \return The value that is contained.
 * \vpublic
 */
template <std::size_t N, typename... Types>
variant_alternative_t<N, Variant<Types...>>&& get(Variant<Types...>&& v) noexcept(false) {
  static_assert(N < sizeof...(Types), "Out of bounds access!");
  return get<variant_alternative_t<N, Variant<Types...>>, Types...>(std::move(v));
}

/*!
 * \brief  Get Variant value from index.
 * \param  v The Variant which value shall be returned.
 * \return The value that is contained.
 * \vpublic
 */
template <std::size_t N, typename... Types>
variant_alternative_t<N, Variant<Types...>> const& get(Variant<Types...> const& v) noexcept(false) {
  static_assert(N < sizeof...(Types), "Out of bounds access!");
  return get<variant_alternative_t<N, Variant<Types...>>, Types...>(v);
}

/* VECTOR Next Construct AutosarC++17_10-A18.9.3: MD_VAC_A18.9.3_moveWithConstArgument */
/*!
 * \brief  Get Variant value from index.
 * \param  v The Variant which value shall be returned.
 * \return The value that is contained.
 * \vpublic
 */
template <std::size_t N, typename... Types>
variant_alternative_t<N, Variant<Types...>> const&& get(Variant<Types...> const&& v) noexcept(false) {
  static_assert(N < sizeof...(Types), "Out of bounds access!");
  return get<variant_alternative_t<N, Variant<Types...>>, Types...>(std::move(v));
}

/*!
 * \brief  Get Variant value from type.
 * \param  v The Variant which value shall be returned.
 * \return The value that is contained.
 * \vpublic
 */
template <typename T, typename... Types>
T& get(Variant<Types...>& v) noexcept(false) {
  std::add_pointer_t<T> result{ara::core::get_if<T>(&v)};

  if (result == nullptr) {
    vac::language::ThrowOrTerminate<bad_variant_access>();
  }
  return *result;
}

/*!
 * \brief  Get Variant value from type.
 * \param  v The Variant which value shall be returned.
 * \return The value that is contained.
 * \vpublic
 */
template <typename T, typename... Types>
T&& get(Variant<Types...>&& v) noexcept(false) {
  std::add_pointer_t<T> result{ara::core::get_if<T>(&v)};

  if (result == nullptr) {
    vac::language::ThrowOrTerminate<bad_variant_access>();
  }
  return std::move(*result);
}

/*!
 * \brief  Get Variant value from type.
 * \param  v The Variant which value shall be returned.
 * \return The value that is contained.
 * \vpublic
 */
template <typename T, typename... Types>
T const& get(Variant<Types...> const& v) noexcept(false) {
  std::add_pointer_t<T const> result{ara::core::get_if<T>(&v)};

  if (result == nullptr) {
    vac::language::ThrowOrTerminate<bad_variant_access>();
  }
  return *result;
}

/* VECTOR Next Construct AutosarC++17_10-A18.9.3: MD_VAC_A18.9.3_moveWithConstArgument */
/*!
 * \brief  Get Variant value from type.
 * \param  v The Variant which value shall be returned.
 * \return The value that is contained.
 * \vpublic
 */
template <typename T, typename... Types>
T const&& get(Variant<Types...> const&& v) noexcept(false) {
  std::add_pointer_t<T const> result{ara::core::get_if<T>(&v)};

  if (result == nullptr) {
    vac::language::ThrowOrTerminate<bad_variant_access>();
  }
  return std::move(*result);
}

/*!
 * \brief  The equality operator.
 * \param  v1 The first Variant for comparison.
 * \param  v2 The second Variant for comparison.
 * \return True if both variants are the same.
 * \vpublic
 */
template <typename... Types>
bool operator==(Variant<Types...> const& v1, Variant<Types...> const& v2) noexcept(false) {
  return detail::VariantCompare<Types...>{v1, v2}.equal();
}

/*!
 * \brief  The inequality operator.
 * \param  v1 The first Variant for comparison.
 * \param  v2 The second Variant for comparison.
 * \return True if both variants are not the same.
 * \vpublic
 */
template <typename... Types>
bool operator!=(Variant<Types...> const& v1, Variant<Types...> const& v2) noexcept(false) {
  return !(v1 == v2);
}

/*!
 * \brief  The less than operator.
 * \param  v1 The first Variant for comparison.
 * \param  v2 The second Variant for comparison.
 * \return True if the first Variant is smaller.
 * \vpublic
 */
template <typename... Types>
bool operator<(Variant<Types...> const& v1, Variant<Types...> const& v2) noexcept(false) {
  return detail::VariantCompare<Types...>{v1, v2}.is_smaller();
}

/*!
 * \brief  The greater than operator.
 * \param  v1 The first Variant for comparison.
 * \param  v2 The second Variant for comparison.
 * \return True if the first Variant is greater.
 * \vpublic
 */
template <typename... Types>
bool operator>(Variant<Types...> const& v1, Variant<Types...> const& v2) noexcept(false) {
  return v2 < v1;
}

/*!
 * \brief  The less or equal than operator.
 * \param  v1 The first Variant for comparison.
 * \param  v2 The second Variant for comparison.
 * \return True if the first Variant is smaller or equal.
 * \vpublic
 */
template <typename... Types>
bool operator<=(Variant<Types...> const& v1, Variant<Types...> const& v2) noexcept(false) {
  return !(v1 > v2);
}

/*!
 * \brief  The greater or equal than operator.
 * \param  v1 The first Variant for comparison.
 * \param  v2 The second Variant for comparison.
 * \return True if the first Variant is greater or equal.
 * \vpublic
 */
template <typename... Types>
bool operator>=(Variant<Types...> const& v1, Variant<Types...> const& v2) noexcept(false) {
  return v2 <= v1;
}

/*!
 * \brief  Visit Variant with visitor accepting all Variant items.
 * \param  visitor The visitor, a functor accepting all variants.
 * \param  v The Variant to visit.
 * \return The result returned by the functor.
 * \vpublic
 */
template <typename Visitor, typename... Types>
auto visit(Visitor&& visitor, Variant<Types...>& v) noexcept(false)
    -> decltype(detail::VisitVariant<Visitor, Variant<Types...>&, Types...>(std::forward<Visitor>(visitor), v)) {
  return detail::VisitVariant<Visitor, Variant<Types...>&, Types...>(std::forward<Visitor>(visitor), v);
}

/* VECTOR Next Construct AutosarC++17_10-A13.3.1: MD_VAC_A13.3.1_forwardingFunctionsShallNotBeOverloaded */
/*!
 * \brief  Visit Variant with visitor accepting all Variant items.
 * \param  visitor The visitor, a functor accepting all variants.
 * \param  v The Variant to visit.
 * \return The result returned by the functor.
 * \vpublic
 */
template <typename Visitor, typename... Types>
auto visit(Visitor&& visitor, Variant<Types...> const& v) noexcept(false)
    -> decltype(detail::VisitVariant<Visitor, Variant<Types...> const&, Types...>(std::forward<Visitor>(visitor), v)) {
  return detail::VisitVariant<Visitor, Variant<Types...> const&, Types...>(std::forward<Visitor>(visitor), v);
}

/* VECTOR Next Construct AutosarC++17_10-A13.3.1: MD_VAC_A13.3.1_forwardingFunctionsShallNotBeOverloaded */
/*!
 * \brief  Visit Variant with visitor accepting all Variant items.
 * \param  visitor The visitor, a functor accepting all variants.
 * \param  v The Variant to visit.
 * \return The result returned by the functor.
 * \vpublic
 */
template <typename Visitor, typename... Types>
auto visit(Visitor&& visitor, Variant<Types...>&& v) noexcept(false)
    -> decltype(detail::VisitVariant<Visitor, Variant<Types...>&&, Types...>(std::forward<Visitor>(visitor),
                                                                             std::move(v))) {
  return detail::VisitVariant<Visitor, Variant<Types...>&&, Types...>(std::forward<Visitor>(visitor), std::move(v));
}

/* VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate */
/*!
 * \brief Swap overload for Variant.
 * \param lhs Variant object to be swapped
 * \param rhs Variant object to be swapped
 * \return
 * \vpublic
 */
template <typename... Types>
inline auto swap(Variant<Types...>& lhs, Variant<Types...>& rhs) noexcept(noexcept(lhs.swap(rhs)))
    -> std::enable_if_t<(vac::language::conjunction<std::is_move_constructible<Types>...>::value) &&
                        (vac::language::conjunction<vac::language::is_swappable<Types>...>::value)> {
  lhs.swap(rhs);
}

}  // namespace core
}  // namespace ara

#endif  // LIB_VAC_INCLUDE_ARA_CORE_VARIANT_H_
