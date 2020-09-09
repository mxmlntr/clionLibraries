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
/*!        \file  ara/core/result.h
 *        \brief  SWS core type ara::core::Result.
 *
 *      \details  A Result contains either a Value representing the actual result of a function or an Error
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_ARA_CORE_RESULT_H_
#define LIB_VAC_INCLUDE_ARA_CORE_RESULT_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <type_traits>
#include <utility>

#include "ara/core/error_code.h"
#include "ara/core/utility.h"
#include "vac/language/compile_exceptions.h"
#include "vac/language/cpp17_backport.h"
#include "vac/language/detail/either.h"
#include "vac/language/throw_or_terminate.h"
#include "vac/testing/test_adapter.h"

namespace ara {
namespace core {
namespace detail {

/*!
 * \brief  Extracts the first type of a parameter pack.
 * \tparam Args... the types of arguments given to this function.
 * \vprivate
 */
template <typename... Args>
class FirstType;

/*!
 * \brief Template specialization for an empty parameter pack.
 * \vprivate
 */
template <>
class FirstType<> : public std::false_type {};

/*!
 * \brief  The function with at least one argument of the Result.
 * \tparam First The first type of arguments given to this function.
 * \tparam Args... The types of arguments given to this function.
 * \vprivate
 */
template <typename First, typename... Args>
class FirstType<First, Args...> {
 public:
  /*!
   * \brief The type of the first argument.
   */
  using type = First;
};

/*!
 * \brief  First type of the Result.
 * \tparam Args... the types of arguments given to this function.
 * \vprivate
 */
template <typename... Args>
using FirstTypeT = typename FirstType<Args...>::type;

}  // namespace detail

// Forward declaration for Result.
template <typename T, typename E = ErrorCode>
class Result;

/*!
 * \brief   Trait that detects whether a type is a Result<...>.
 * \details SFINAE pattern: General template with typedef type false.
 * \tparam  U Arbitrary placeholder for SFINAE pattern.
 * \trace   CREQ-180979
 * \vprivate
 */
template <typename U>
struct is_result : std::false_type {};

/*!
 * \brief   Trait that detects whether a type is a Result<...>.
 * \details SFINAE pattern: Specialization for Result<U, G>.
 * \tparam  U Value of Result.
 * \tparam  G Error of Result.
 * \vprivate
 */
template <typename U, typename G>
struct is_result<Result<U, G>> : std::true_type {};

/*!
 * \brief  This class is a type that contains either a value or an error.
 * \tparam T The type of value. Must be object/integral type, and must be move-constructible.
 * \tparam E The type of error. Must be object/integral type, must be move-constructible, must be copy-constructible,
 *         and must not be (implicitly) convertible to \a T.
 * \trace  CREQ-166420
 * \vpublic
 */
template <typename T, typename E>
class Result final {
  // Assertions for T
  static_assert(std::is_object<T>::value, "T must must be object/integral type.");
  static_assert(std::is_move_constructible<T>::value,
                "T must be move-constructible. Otherwise it cannot be used as return type!");

  // Assertions for E
  static_assert(std::is_object<E>::value, "E must must be object/integral type.");
  static_assert(std::is_move_constructible<E>::value,
                "E must be move-constructible. Otherwise it cannot be used as return type!");
  static_assert(std::is_copy_constructible<E>::value,
                "E must be copy-constructible. Move-only Error types may require additional r-value overloads of "
                "member functions for Result.");
  static_assert(!std::is_convertible<E, T>::value,
                "E must not be (implicitly) convertible to T! Otherwise, undesired effects may occur, because "
                "Result(T const&) and Result(T&&) are not explicit.");

  /*!
   * \brief  SFINAE for callable returning Result<...>.
   * \tparam F Callable.
   * \vprivate
   */
  template <typename F>
  using CallableReturnsResult = std::enable_if_t<is_result<std::result_of_t<F(T const&)>>::value>;

  /*!
   * \brief  SFINAE for callable returning void.
   * \tparam F Callable.
   * \vprivate
   */
  template <typename F>
  using CallableReturnsVoid = std::enable_if_t<std::is_void<std::result_of_t<F(T const&)>>::value>;

  /*!
   * \brief  SFINAE for callable not returning Result<...>.
   * \tparam F Callable.
   * \vprivate
   */
  template <typename F>
  using CallableReturnsNoResult = std::enable_if_t<(!is_result<std::result_of_t<F(T const&)>>::value) &&
                                                   (!std::is_void<std::result_of_t<F(T const&)>>::value)>;

  /*!
   * \brief Helper trait to determine if Result is no-throw swappable.
   * \vprivate
   */
  constexpr static bool is_nothrow_swappable{
      (std::is_nothrow_destructible<T>::value) && (std::is_nothrow_destructible<E>::value) &&
      (std::is_nothrow_move_constructible<T>::value) && (std::is_nothrow_move_constructible<E>::value)};

 public:
  /*! \brief Value type of the Result. */
  using value_type = T;
  /*! \brief Error type of the Result. */
  using error_type = E;
  /*! \brief In-place construct constant for value_type */
  constexpr static in_place_type_t<value_type> kInPlaceValue{};
  /*! \brief In-place construct constant for error_type */
  constexpr static in_place_type_t<error_type> kInPlaceError{};

  /*!
   * \brief  Build a new Result from a value that is constructed in-place from the given arguments.
   *         This function shall not participate in overload resolution unless:
   *         std::is_constructible<T, Args&&...>::value is true, and the first type of the expanded parameter pack is
   *         not T, and the first type of the expanded parameter pack is not a specialization of Result.
   * \tparam Args... The types of arguments given to this function.
   * \param  args the arguments used for constructing the value.
   * \return A Result that contains a value.
   * \vpublic
   */
  template <typename... Args, typename = std::enable_if_t<(std::is_constructible<T, Args&&...>::value) &&
                                                          (!std::is_same<T, detail::FirstTypeT<Args...>>::value) &&
                                                          (!is_result<detail::FirstTypeT<Args...>>::value)>>
  constexpr static auto FromValue(Args&&... args) noexcept(false) -> Result {
    return Result{Result::kInPlaceValue, std::forward<Args>(args)...};
  }

  /*!
   * \brief  Build a new Result from the specified value (given as lvalue).
   * \param  t The value to put into the Result.
   * \return A Result that contains the value t.
   * \trace  CREQ-216785
   * \vpublic
   */
  constexpr static auto FromValue(T const& t) -> Result { return Result{Result::kInPlaceValue, t}; }

  /*!
   * \brief  Build a new Result from the specified value (given as rvalue).
   * \param  t The value to put into the Result.
   * \return A Result that contains the value t.
   * \trace  CREQ-216786
   * \vpublic
   */
  constexpr static auto FromValue(T&& t) -> Result { return Result{Result::kInPlaceValue, std::move(t)}; }

  /*!
   * \brief  Build a new Result from an error that is constructed in-place from the given arguments.
   *         This function shall not participate in overload resolution unless:
   *         std::is_constructible<E, Args&&...>::value is true, and the first type of the expanded parameter pack is
   *         not E, and the first type of the expanded parameter pack is not a specialization of Result.
   * \tparam Args... The types of arguments given to this function.
   * \param  args The arguments used for constructing the error.
   * \return A Result that contains an error.
   * \vpublic
   */
  template <typename... Args, typename = std::enable_if_t<(std::is_constructible<E, Args&&...>::value) &&
                                                          (!std::is_same<E, detail::FirstTypeT<Args...>>::value) &&
                                                          (!is_result<detail::FirstTypeT<Args...>>::value)>>
  constexpr static auto FromError(Args&&... args) noexcept(false) -> Result {
    return Result{Result::kInPlaceError, std::forward<Args>(args)...};
  }

  /*!
   * \brief  Build a new Result from the specified error (given as lvalue).
   * \param  e The error to put into the Result.
   * \return A Result that contains the error e.
   * \trace  CREQ-216950
   * \vpublic
   */
  static auto FromError(E const& e) -> Result { return Result{Result::kInPlaceError, e}; }

  /*!
   * \brief  Build a new Result from the specified error (given as rvalue).
   * \param  e The error to put into the Result.
   * \return A Result that contains the error e.
   * \trace  CREQ-216951
   * \vpublic
   */
  static auto FromError(E&& e) -> Result { return Result{Result::kInPlaceError, std::move(e)}; }

  /* VECTOR Next Construct VectorC++-V12-1-4: MD_VAC_V12-1-4_constructorShallBeDeclaredExplicit */
  /*!
   * \brief Construct a new Result from the specified value (given as lvalue).
   * \param t The value to put into the Result.
   * \vpublic
   */
  constexpr Result(T const& t) : Result{Result::kInPlaceValue, t} {}  // NOLINT(runtime/explicit)

  /* VECTOR Next Construct VectorC++-V12-1-4: MD_VAC_V12-1-4_constructorShallBeDeclaredExplicit */
  /*!
   * \brief Construct a new Result from the specified value (given as rvalue).
   * \param t The value to put into the Result.
   * \vpublic
   */
  constexpr Result(T&& t) : Result{Result::kInPlaceValue, std::move(t)} {}  // NOLINT(runtime/explicit)

  /*!
   * \brief Construct a new Result from the specified error (given as lvalue).
   * \param e The error to put into the Result.
   * \vpublic
   */
  constexpr explicit Result(E const& e) : Result{Result::kInPlaceError, e} {}

  /*!
   * \brief Construct a new Result from the specified error (given as rvalue).
   * \param e The error to put into the Result.
   * \vpublic
   */
  constexpr explicit Result(E&& e) : Result{Result::kInPlaceError, std::move(e)} {}

  /* VECTOR Next Construct VectorC++-V12-1-4: MD_VAC_V12-1-4_constructorShallBeDeclaredExplicit */
  /* VECTOR Next Construct AutosarC++17_10-A12.1.5: MD_VAC_A12.1.5_useDelegatingConstructor */
  /*!
   * \brief In-place tag constructor for a Value.
   * \param args Construction parameter.
   * \vprivate
   */
  template <typename... Args>
  constexpr explicit Result(in_place_type_t<T>, Args&&... args)
      : data_{Result::kInPlaceValue, std::forward<Args>(args)...} {}

  /*!
   * \brief In-place tag constructor for an Error.
   * \param args Construction parameter.
   * \vprivate
   */
  template <typename... Args>
  constexpr explicit Result(in_place_type_t<E>, Args&&... args)
      : data_{Result::kInPlaceError, std::forward<Args>(args)...} {}

  /*!
   * \brief Copy-construct a new Result from another instance.
   * \param other The other instance.
   * \vpublic
   */
  constexpr Result(Result const& other) = default;

  /* VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate */
  /*!
   * \brief Move-construct a new Result from another instance.
   * \param other The other instance.
   * \vpublic
   */
  constexpr Result(Result&& other) = default;

  /* VECTOR Next Construct AutosarC++17_10-M0.1.8: MD_VAC_M0.1.8_destructorHasNoExternalSideEffect */
  /* VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate */
  /*!
   * \brief Destructor.
   * \vpublic
   */
  ~Result() = default;

  /*!
   * \brief  Copy-assign another Result to this instance.
   * \param  other The other instance.
   * \return *this, containing the contents of \a other.
   * \vpublic
   */
  auto operator=(Result const& other) & -> Result& = default;

  /* VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate */
  /*!
   * \brief  Move-assign another Result to this instance.
   * \param  other The other instance.
   * \return *this, containing the contents of \a other.
   * \vpublic
   */
  auto operator=(Result&& other) & -> Result& = default;

  /*!
   * \brief  Put a new value into this instance, constructed in-place from the given arguments.
   * \tparam Args... The types of arguments given to this function.
   * \param  args  The arguments used for constructing the value.
   * \return
   * \vpublic
   */
  template <typename... Args>
  auto EmplaceValue(Args&&... args) noexcept(false) -> void {
    this->data_.EmplaceRight(std::forward<Args>(args)...);
  }

  /*!
   * \brief  Put a new error into this instance, constructed in-place from the given arguments.
   * \tparam Args... The types of arguments given to this function.
   * \param  args The arguments used for constructing the error.
   * \return
   * \vpublic
   */
  template <typename... Args>
  auto EmplaceError(Args&&... args) noexcept(false) -> void {
    this->data_.EmplaceLeft(std::forward<Args>(args)...);
  }

  /* VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate */
  /*!
   * \brief         Exchange the contents of this instance with those of \a other.
   * \param[in,out] other The other instance.
   * \return
   * \vpublic
   */
  auto Swap(Result& other) noexcept(is_nothrow_swappable) -> void {
    using std::swap;
    swap(data_, other.data_);
  }

  /*!
   * \brief  Check whether *this contains a value.
   * \return True if *this contains a value, false otherwise.
   * \trace  CREQ-166422
   * \vpublic
   */
  constexpr explicit operator bool() const noexcept { return HasValue(); }

  /*!
   * \brief  Check whether *this contains a value.
   * \return True if *this contains a value, false otherwise.
   * \trace  CREQ-166422
   * \vpublic
   */
  constexpr auto HasValue() const noexcept -> bool { return this->data_.IsRight(); }

  /*!
   * \brief   Access the contained value.
   * \details This function's behavior is undefined if *this does not contain a value.
   * \return  A reference to the contained value.
   * \trace   CREQ-166423
   * \return  a reference to the contained value.
   * \vpublic
   */
  constexpr auto operator*() const -> T const& { return Value(); }

  /*!
   * \brief   Access the contained value.
   * \details This function's behavior is undefined if *this does not contain a value.
   * \return  A pointer to the contained value.
   * \trace   CREQ-166423
   * \vpublic
   */
  constexpr auto operator-> () const -> T const* { return std::addressof(Value()); }

  /*!
   * \brief   Access the contained value.
   * \details The behavior of this function is undefined if *this does not contain a value.
   * \return  A reference to the contained value.
   * \trace   CREQ-166423
   * \vpublic
   */
  constexpr auto Value() const& -> T const& {
    assert(HasValue());
    return this->data_.RightUnsafe();
  }

  /*!
   * \brief  Non-const access the contained value.
   *         The behavior of this function is undefined if *this does not contain a value.
   * \return A non-const reference to the contained value.
   * \trace  CREQ-166423
   * \vprivate
   */
  auto Value() & -> T& {
    assert(HasValue());
    return this->data_.RightUnsafe();
  }

  /*!
   * \brief   Access the contained value.
   * \details The behavior of this function is undefined if *this does not contain a value.
   * \return  A reference to the contained value.
   * \trace   CREQ-166423
   * \vpublic
   */
  auto Value() && -> T&& { return std::move(this->Value()); }

  /*!
   * \brief   Access the contained error.
   * \details The behavior of this function is undefined if *this does not contain an error.
   * \return  A reference to the contained error.
   * \trace   CREQ-166423
   * \vpublic
   */
  constexpr auto Error() const& -> E const& {
    assert(!HasValue());
    return this->data_.LeftUnsafe();
  }

  /*!
   * \brief   Access the contained error.
   * \details The behavior of this function is undefined if *this does not contain an error.
   * \return  A rvalue reference to the contained error.
   * \trace   CREQ-166423
   * \vpublic
   */
  auto Error() && -> E&& {
    assert(!HasValue());
    return std::move(this->data_.LeftUnsafe());
  }

  /*!
   * \brief   Return the contained value or the given default value.
   * \details If *this contains a value, it is returned. Otherwise, the specified default value is returned,
   *          converted to T.
   * \tparam  U the type of \a defaultValue. U&& must be (implicitly) convertible to T.
   * \param   defaultValue  The value to use if *this does not contain a value.
   * \return  The value.
   * \trace   CREQ-180563
   * \vpublic
   */
  template <typename U>
  constexpr auto ValueOr(U&& defaultValue) const& noexcept(false) -> T {
    static_assert(std::is_convertible<U&&, T>::value, "U&& must be convertible to T.");
    return HasValue() ? Value() : static_cast<T>(std::forward<U>(defaultValue));
  }

  /* VECTOR Next Construct AutosarC++17_10-A13.3.1: MD_VAC_A13.3.1_forwardingFunctionsShallNotBeOverloaded */
  /*!
   * \copydoc Result::ValueOr(U&&)const&
   * \vpublic
   */
  template <typename U>
      auto ValueOr(U&& defaultValue) && noexcept(false) -> T {
    static_assert(std::is_convertible<U&&, T>::value, "U&& must be convertible to T.");
    return HasValue() ? std::move(*this).Value() : static_cast<T>(std::forward<U>(defaultValue));
  }

  /*!
   * \brief   Return the contained error or the given default error.
   * \details If *this contains an error, it is returned. Otherwise, the specified default error is returned,
   *          converted to E.
   * \tparam  G The type of \a defaultError.
   * \param   defaultError The error to use if *this does not contain an error.
   * \return  The error.
   * \trace   CREQ-180761
   * \vpublic
   */
  template <typename G>
  constexpr auto ErrorOr(G&& defaultError) const noexcept(false) -> E {
    static_assert(std::is_convertible<G&&, E>::value, "G&& must be convertible to E.");
    return HasValue() ? static_cast<E>(std::forward<G>(defaultError)) : Error();
  }

  /*!
   * \brief  Return whether this instance contains the given error.
   * \tparam G The type of the error \a e.
   * \param  error The error to check.
   * \return True if *this contains the given error, false otherwise.
   * \vpublic
   */
  template <typename G>
  constexpr auto CheckError(G&& error) const noexcept(false) -> bool {
    static_assert(std::is_convertible<G&&, E>::value, "G&& must be convertible to E.");
    return HasValue() ? false : (Error() == static_cast<E>(std::forward<G>(error)));
  }

  /*!
   * \brief   Return the contained value or throw an exception.
   * \details This function does not participate in overload resolution when the compiler toolchain
   *          does not support C++ exceptions.
   * \return  The value.
   * \throws  <TYPE> The exception type associated with the contained error, exceptionsafety{Strong}.
   * \trace   CREQ-166424
   * \vpublic
   */
  auto ValueOrThrow() const noexcept(false) -> T const& {
    if (!HasValue()) {
      E e{this->data_.LeftUnsafe()};
      e.ThrowAsException();
    }
    return Value();
  }

  /*!
   * \brief   Returns the contained value or return the result of a function call.
   * \details If *this contains a value, it is returned. Otherwise, the specified callable is invoked and its return
   *          value which is to be compatible to type T is returned from this function.
   *          The Callable is expected to be compatible to this interface:
   *          <code>
   *          T f(E const&);
   *          </code>
   * \tparam  F The type of the Callable \a f.
   * \param   f The Callable.
   * \return  The contained value or return the result of a function call.
   * \trace   CREQ-180562
   * \vpublic
   */
  template <typename F>
  constexpr auto Resolve(F&& f) const& noexcept(false) -> T {
    static_assert(std::is_convertible<std::result_of_t<F(error_type const&)>, T>::value,
                  "Return type of f must be (implicitly) convertible to T.");
    return HasValue() ? Value() : static_cast<T>(std::forward<F>(f)(Error()));
  }

  /* VECTOR Next Construct AutosarC++17_10-A13.3.1: MD_VAC_A13.3.1_forwardingFunctionsShallNotBeOverloaded */
  /*!
   * \copydoc Result::Resolve(F&&)const&
   * \vpublic
   */
  template <typename F>
      auto Resolve(F&& f) && noexcept(false) -> T {
    return HasValue() ? std::move(*this).Value() : std::forward<F>(f)(std::move(*this).Error());
  }

  /*!
   * \brief   Apply the given Callable to the value of this instance, and return a new Result with the
   *          result of the call.
   * \details The Callable is expected to be compatible to one of these two interfaces:
   *          <code>
   *          Result<XXX, E> f(T const&);
   *          XXX f(T const&);
   *          </code>
   *          meaning that the Callable either returns a Result<XXX> or a XXX directly,
   *          where XXX can be any type that is suitable for use by class Result.
   *          The return type of this function is always Result<XXX, E>.
   *          If this instance does not contain a value, a new Result<XXX, E> is still
   *          created and returned, with the original error contents of this instance
   *          being copied into the new instance.
   * \tparam  F The type of the Callable \a f.
   * \tparam  CallableReturnsResult<F> Enable template when callable \a F returns Result<...> type.
   * \param   f The Callable.
   * \return  A new Result instance of the possibly transformed type.
   * \trace   CREQ-180561
   * \vpublic
   */
  template <typename F, typename = CallableReturnsResult<F>>
  constexpr auto Bind(F&& f) const noexcept(false) -> std::result_of_t<F(value_type const&)> {
    return AndThen(std::forward<F>(f));
  }

  /* VECTOR Next Construct AutosarC++17_10-A13.3.1: MD_VAC_A13.3.1_forwardingFunctionsShallNotBeOverloaded */
  /*!
   * \brief  Apply the given Callable to the value of this instance, and return a new Result with the
   *         result of the call.
   * \tparam F The type of the Callable \a f.
   * \tparam CallableReturnsNoResult<F> Enable template when callable \a f does not return Result<...> type.
   * \param  f The Callable.
   * \return A new Result instance of the possibly transformed type.
   * \trace  CREQ-180561
   * \vpublic
   */
  template <typename F, typename = CallableReturnsNoResult<F>>
  constexpr auto Bind(F&& f) const noexcept(false) -> Result<std::result_of_t<F(value_type const&)>, E> {
    return Map(std::forward<F>(f));
  }

  /* VECTOR Next Construct AutosarC++17_10-M0.1.9: MD_VAC_M0.1.9_redundantCode */
  /* VECTOR Next Construct AutosarC++17_10-A13.3.1: MD_VAC_A13.3.1_forwardingFunctionsShallNotBeOverloaded */
  /*!
   * \brief  Apply the given Callable to the value of this instance, and return a new Result with the
   *         result of the call.
   * \tparam F The type of the Callable \a f.
   * \tparam CallableReturnsNoResult<F> Enable template when callable \a f does not return Result<...> type.
   * \param  f The Callable.
   * \return A new Result instance of the possibly transformed type.
   * \trace  CREQ-180561
   * \vpublic
   */
  template <typename F, typename = CallableReturnsVoid<F>>
  constexpr auto Bind(F&& f) const& noexcept(false) -> Result<void, E> {
    // VECTOR Next Construct AutosarC++17_10-A2.11.2: MD_VAC_A2.11.2_usingNameReusedInLocalScope
    /*! \brief Local result type alias. */
    using R = Result<void, E>;
    return AndThen([&f](value_type const& value) -> R {
      std::forward<F>(f)(value);
      return R{R::kInPlaceValue};
    });
  }

  /*!
   * \brief  Executes the given Callable with the value of this instance, or returns the contained error.
   * \tparam F The type of the Callable \a f. Must be compatible with the signature XX f(T const&).
   *         Return type must not be void.
   * \tparam U The return value of the Function F.
   * \param  f The Callable.
   * \return A new Result instance of the possibly transformed type or the contained error.
   * \remark This function can be used to transform the contained value.
   * \trace  CREQ-180560
   * \vprivate
   */
  template <typename F, typename U = std::result_of_t<F(value_type const&)>>
  constexpr auto Map(F&& f) const& noexcept(false) -> Result<U, E> {
    static_assert(!std::is_void<U>::value, "Return Type of f must not be void. Use Inspect()/Drop() instead.");
    // VECTOR Next Construct AutosarC++17_10-A2.11.2: MD_VAC_A2.11.2_usingNameReusedInLocalScope
    /*! \brief Local result type alias. */
    using R = Result<U, E>;
    return HasValue() ? R{R::kInPlaceValue, std::forward<F>(f)(Value())} : R{Result::kInPlaceError, Error()};
  }

  /* VECTOR Next Construct AutosarC++17_10-A13.3.1: MD_VAC_A13.3.1_forwardingFunctionsShallNotBeOverloaded */
  /*!
   * \brief  Executes the given Callable with the value of this instance, or returns the contained error.
   * \tparam F The type of the Callable \a f. Must be compatible with the signature XX f(T&&).
   *         Return type must not be void.
   * \tparam U The return value of the Function F.
   * \param  f The Callable.
   * \return A new Result instance of the possibly transformed type or the contained error.
   * \remark This function can be used to transform the contained value.
   * \trace  CREQ-180560
   * \vprivate
   */
  template <typename F, typename U = std::result_of_t<F(value_type&&)>>
      auto Map(F&& f) && noexcept(false) -> Result<U, E> {
    static_assert(!std::is_void<U>::value, "Return Type of f must not be void. Use Inspect()/Drop() instead.");
    /*! \brief Local result type alias. */
    using R = Result<U, E>;
    /* If value, the value is moved to the provided function and the returned value of the function is forwarded to the
     * Result constructor, otherwise the error is moved to the Result constructor. */
    return HasValue() ? R{R::kInPlaceValue, std::forward<F>(f)(std::move(*this).Value())}
                      : R{Result::kInPlaceError, std::move(*this).Error()};
  }

  /*!
   * \brief  Executes the given Callable with the error of this instance, or returns the untouched value.
   * \tparam F the type of the Callable \a f. Must be compatible with the signature XX f(E const&).
   *         Return Type must be an object/integral type.
   * \tparam E1 The return value of the Function F.
   * \param  f The Callable.
   * \return A new Result instance of the possibly transformed error type or the unchanged value.
   * \remark This function can be used to transform the contained error, or perform error handling in general.
   * \trace  CREQ-180559
   * \vprivate
   */
  template <typename F, typename E1 = std::result_of_t<F(error_type const&)>>
  constexpr auto MapError(F&& f) const& noexcept(false) -> Result<T, E1> {
    /*! \brief Local result type alias. */
    using R = Result<T, E1>;
    return HasValue() ? R{Result::kInPlaceValue, Value()} : R{R::kInPlaceError, std::forward<F>(f)(Error())};
  }

  /* VECTOR Next Construct AutosarC++17_10-A13.3.1: MD_VAC_A13.3.1_forwardingFunctionsShallNotBeOverloaded */
  /*!
   * \brief  Executes the given Callable with the error of this instance, or returns the untouched value.
   * \tparam F The type of the Callable \a f. Must be compatible with the signature XX f(E&&).
   *         Return Type must be an object/integral type.
   * \tparam E1 The return value of the Function F
   * \param  f The Callable.
   * \return A new Result instance of the possibly transformed error type or the unchanged value.
   * \remark This function can be used to transform the contained error, or perform error handling in general.
   * \trace  CREQ-180559
   * \vprivate
   */
  template <typename F, typename E1 = std::result_of_t<F(error_type&&)>>
      auto MapError(F&& f) && noexcept(false) -> Result<T, E1> {
    /*! \brief Local result type alias. */
    using R = Result<T, E1>;
    return HasValue() ? R{Result::kInPlaceValue, std::move(*this).Value()}
                      : R{R::kInPlaceError, std::forward<F>(f)(std::move(*this).Error())};
  }

  /*!
   * \brief   Create a new Result with the given new value if this instance contains a value, otherwise it
   *          return the contained error.
   * \details The new value is inplace-constructed from the given parameters \a args.
   * \tparam  U Rhe type of the given value, defaulted to \a T.
   * \tparam  Args Construction arguments for \a U.
   * \tparam  Res The return type of the Replace function.
   * \param   args Arguments for the in place construction of the replacement value.
   * \return  A new Result instance with the constructed replacement value or the contained error.
   * \remark  This function can be used to replace the contained value, which type may differ from \a T.
   * \trace   CREQ-180564
   * \vprivate
   */
  template <typename U = T, typename... Args, typename Res = Result<U, E>>
  constexpr auto Replace(Args&&... args) const& noexcept(false) -> Res {
    return HasValue() ? Res{Res::kInPlaceValue, std::forward<Args>(args)...} : Res{Result::kInPlaceError, Error()};
  }

  /* VECTOR Next Construct AutosarC++17_10-A13.3.1: MD_VAC_A13.3.1_forwardingFunctionsShallNotBeOverloaded */
  /*!
   * \brief   Create a new Result with the given new value if this instance contains a value, otherwise it
   *          return the contained error.
   * \details The new value is inplace-constructed from the given parameters \a args.
   * \tparam  U The type of the given value, defaulted to \a T.
   * \tparam  Args Construction arguments for \a U.
   * \tparam  Res The return type of the Replace function.
   * \param   args Arguments for the in place construction of the replacement value.
   * \return  A new Result instance with the constructed replacement value or the contained error.
   * \remark  This function can be used to replace the contained value, which type may differ from \a T.
   * \trace   CREQ-180564
   * \vprivate
   */
  template <typename U = T, typename... Args, typename Res = Result<U, E>>
      auto Replace(Args&&... args) && noexcept(false) -> Res {
    return HasValue() ? Res{Res::kInPlaceValue, std::forward<Args>(args)...}
                      : Res{Result::kInPlaceError, std::move(*this).Error()};
  }

  /*!
   * \brief  Returns the contained value, else it returns the given \a alternative.
   * \tparam E1 New error type.
   * \param  alternative Other result.
   * \return The value of this, else \a alternative.
   * \trace  CREQ-180565
   * \vprivate
   */
  template <typename E1>
  constexpr auto Or(Result<T, E1> const& alternative) const& noexcept(false) -> Result<T, E1> {
    /*! \brief Local result type alias. */
    using R = Result<T, E1>;
    return HasValue() ? R{Result::kInPlaceValue, Value()} : alternative;
  }

  /*!
   * \brief  Returns the contained value, else it returns the given \a alternative.
   * \tparam E1 New error type.
   * \param  alternative Other result.
   * \return The value of this, else \a alternative.
   * \trace  CREQ-180565
   * \vprivate
   */
  template <typename E1>
      auto Or(Result<T, E1>&& alternative) && noexcept(false) -> Result<T, E1> {
    /*! \brief Local result type alias. */
    using R = Result<T, E1>;
    return HasValue() ? R{Result::kInPlaceValue, std::move(*this).Value()} : std::move(alternative);
  }

  /*!
   * \brief  Returns the contained value, else transforms the contained error using the passed function.
   * \tparam F Transform function for the error. Must be compatible to the signature Result<T,XX> f(E const&).
   * \param  f Transform function for the error.
   * \return The value of this, or the transformation result of the error.
   * \trace  CREQ-180566
   * \vprivate
   */
  template <typename F>
  constexpr auto OrElse(F&& f) const& noexcept(false) -> std::result_of_t<F(error_type const&)> {
    /*! \brief Local result type alias. */
    using R = std::result_of_t<F(error_type const&)>;
    static_assert(is_result<R>::value, "Return type of f must be a Result.");
    static_assert(std::is_same<value_type, typename R::value_type>::value, "Value type must not change.");
    return HasValue() ? R{Result::kInPlaceValue, Value()} : std::forward<F>(f)(Error());
  }

  /* VECTOR Next Construct AutosarC++17_10-A13.3.1: MD_VAC_A13.3.1_forwardingFunctionsShallNotBeOverloaded */
  /*!
   * \brief  Returns the contained value, else transforms the contained error using the passed function.
   * \tparam F Transform function for the error. Must be compatible to the signature Result<T,XX> f(E&&).
   * \param  f Transform function for the error.
   * \return The value of this, or the transformation result of the error.
   * \trace  CREQ-180566
   * \vprivate
   */
  template <typename F>
      auto OrElse(F&& f) && noexcept(false) -> std::result_of_t<F(error_type&&)> {
    /*! \brief Local result type alias. */
    using R = std::result_of_t<F(error_type &&)>;
    static_assert(is_result<R>::value, "Return type of f must be a Result.");
    static_assert(std::is_same<value_type, typename R::value_type>::value, "Value type must not change.");
    return HasValue() ? R{Result::kInPlaceValue, std::move(*this).Value()}
                      : std::forward<F>(f)(std::move(*this).Error());
  }

  /*!
   * \brief  Returns the passed Result if this has a value, else it returns the error of this.
   * \tparam U New value type.
   * \param  other Other result.
   * \return \a other if this has a value, else the error of this.
   * \trace  CREQ-180668
   * \vprivate
   */
  template <typename U>
  constexpr auto And(Result<U, E> const& other) const& noexcept(false) -> Result<U, E> {
    /*! \brief Local result type alias. */
    using R = Result<U, E>;
    return HasValue() ? other : R{Result::kInPlaceError, Error()};
  }

  /*!
   * \copydoc Result::And(Result<U, E> const&)const&
   * \vprivate
   */
  template <typename U>
      auto And(Result<U, E>&& other) && noexcept(false) -> Result<U, E> {
    /*! \brief Local result type alias. */
    using R = Result<U, E>;
    return HasValue() ? std::move(other) : R{Result::kInPlaceError, std::move(const_cast<E&>(Error()))};
  }

  /*!
   * \brief  Transforms the contained value using the passed function, else returns the contained error.
   * \tparam F Transform function for the value. Must be compatible with the signature Result<XX,E> f(T const&).
   * \param  f Transform function for the error.
   * \return The transformation result of the value, else the error of \a this.
   * \trace  CREQ-180669
   * \vprivate
   */
  template <typename F>
  constexpr auto AndThen(F&& f) const& noexcept(false) -> std::result_of_t<F(value_type const&)> {
    /*! \brief Local result type alias. */
    using R = std::result_of_t<F(value_type const&)>;
    static_assert(is_result<R>::value, "Return type of f must be a Result.");
    static_assert(std::is_same<error_type, typename R::error_type>::value, "Error type must not change.");
    return HasValue() ? std::forward<F>(f)(Value()) : R{Result::kInPlaceError, Error()};
  }

  /* VECTOR Next Construct AutosarC++17_10-A13.3.1: MD_VAC_A13.3.1_forwardingFunctionsShallNotBeOverloaded */
  /*!
   * \brief  Transforms the contained value using the passed function, else returns the contained error.
   * \tparam F Transform function for the value. Must be compatible with the signature Result<XX,E> f(T&&).
   * \param  f Transform function for the error.
   * \return The transformation result of the value, else the error of \a this.
   * \trace  CREQ-180669
   * \vprivate
   */
  template <typename F>
      auto AndThen(F&& f) && noexcept(false) -> std::result_of_t<F(value_type&&)> {
    /*! \brief Local result type alias. */
    using R = std::result_of_t<F(value_type &&)>;
    static_assert(is_result<R>::value, "Return type of f must be a Result.");
    static_assert(std::is_same<error_type, typename R::error_type>::value, "Error type must not change.");
    return HasValue() ? std::forward<F>(f)(std::move(*this).Value())
                      : R{Result::kInPlaceError, std::move(*this).Error()};
  }

  /*!
   * \brief   Returns the unchanged value if it fulfills the predicate \a pred.
   * \details The function has three possible results:
   *          (1) the contained error if \a this holds an error;
   *          (2) the contained value if \a this value fulfills the predicate \a pred;
   *          (3) the provided \a error if \a this value does not fulfill the predicate \a pred.
   * \tparam  F Unary function. Result must be convertible to bool. Must accept T const& as parameter.
   * \tparam  E1 Must be convertible to E.
   * \param   pred Unary function for checking the value.
   * \param   error Error that is returned if the value does not fulfill the predicate \a pred.
   * \return  The unchanged value if fulfills the \a pred, if not \a error is returned. Otherwise, the contained error.
   * \trace   CREQ-180760
   * \vprivate
   */
  template <typename F, typename E1>
  constexpr auto Filter(F&& pred, E1&& error) const& noexcept(false) -> Result {
    static_assert(std::is_convertible<std::result_of_t<F(value_type const&)>, bool>::value,
                  "Return type of predicate must be convertible to boolean.");
    static_assert(std::is_convertible<E1&&, E>::value, "E1 must be convertible to E.");
    return AndThen([&pred, &error](value_type const& value) -> Result {
      return pred(value) ? Result{Result::kInPlaceValue, value}
                         : Result{Result::kInPlaceError, std::forward<E1>(error)};
    });
  }

  /* VECTOR Next Construct AutosarC++17_10-A13.3.1: MD_VAC_A13.3.1_forwardingFunctionsShallNotBeOverloaded */
  /*!
   * \brief   Returns the unchanged value if it fulfills the predicate \a pred.
   * \details The function has three possible results:
   *          (1) the contained error if \a this holds an error;
   *          (2) the contained value if \a this value fulfills the predicate \a pred;
   *          (3) the provided \a error if \a this value does not fulfill the predicate \a pred.
   * \tparam  F Unary function. Result must be convertible to bool. Must accept T const& as parameter.
   * \param   pred Unary function for checking the value.
   * \param   error Error that is returned if the value does not fulfill the predicate \a pred.
   * \return  The unchanged value if fulfills the \a pred, if not \a error is returned. Otherwise, the contained error.
   * \trace   CREQ-180760
   * \vprivate
   */
  template <typename F, typename E1>
      auto Filter(F&& pred, E1&& error) && noexcept(false) -> Result {
    static_assert(std::is_convertible<std::result_of_t<F(value_type const&&)>, bool>::value,
                  "Return type of predicate must be convertible to boolean.");
    static_assert(std::is_convertible<E1&&, E>::value, "E1 must be convertible to E.");
    return std::move(*this).AndThen([&pred, &error](value_type&& value) -> Result {
      return pred(static_cast<value_type const&&>(value)) ? Result{Result::kInPlaceValue, std::move(value)}
                                                          : Result{Result::kInPlaceError, std::forward<E1>(error)};
    });
  }

  /* VECTOR Next Construct AutosarC++17_10-A13.3.1: MD_VAC_A13.3.1_forwardingFunctionsShallNotBeOverloaded */
  /*!
   * \brief   Returns the unchanged value if it fulfills the predicate \a pred, which returns a Result.
   * \details The function has three possible results:
   *          (1) the contained error if \a this holds an error. Depending on the result of the \a pred call with \a
   *          this value as parameter it returns the contained value of \a this (2), or the returned error (3).
   * \tparam  F Function returning a Result. Must accept T const& as parameter.
   * \param   pred Function for checking the contained value.
   * \return  The unchanged value if fulfills the \a pred, if not the error of \a pred is returned. Otherwise,
   *          the error contained in \a this is returned.
   * \trace   CREQ-180759
   * \vprivate
   */
  template <typename F>
  constexpr auto Filter(F&& pred) const& noexcept(false) -> Result {
    /*! \brief Local result type alias. */
    using R = std::result_of_t<F(value_type const&)>;
    static_assert(is_result<R>::value, "Return type of f must be a Result.");
    static_assert(std::is_same<error_type, typename R::error_type>::value, "Error type must not change.");
    return AndThen(
        [&pred](value_type const& value) -> Result { return pred(value).template Replace<value_type>(value); });
  }

  /* VECTOR Next Construct AutosarC++17_10-A13.3.1: MD_VAC_A13.3.1_forwardingFunctionsShallNotBeOverloaded */
  /*!
   * \copydoc Result::Filter(F&&)const&
   * \vprivate
   */
  template <typename F>
      auto Filter(F&& pred) && noexcept(false) -> Result {
    /*! \brief Local result type alias. */
    using R = std::result_of_t<F(value_type const&&)>;
    static_assert(is_result<R>::value, "Return type of f must be a Result.");
    static_assert(std::is_same<error_type, typename R::error_type>::value, "Error type must not change.");
    return std::move(*this).AndThen([&pred](value_type&& value) -> Result {
      return pred(static_cast<value_type const&&>(value)).template Replace<value_type>(std::move(value));
    });
  }

  /*!
   * \brief   Removes the value of the result.
   * \details If \a this Result object contains a value, the function returns an empty Result, else the contained error.
   * \return  An empty Result, or the contained error.
   * \trace   CREQ-180957
   * \vprivate
   */
  constexpr auto Drop() const -> Result<void, E> { return Replace<void>(); }

  /*!
   * \brief  Executes a given callable with the contained value as parameter. If \a this contains an error the callable
   *         is not executed.
   * \tparam F Callable, must have the signature void f(T const&).
   * \param  f Callable function.
   * \return The unchanged Result.
   * \trace  CREQ-180958
   * \vprivate
   */
  template <typename F>
  constexpr auto Inspect(F&& f) const& noexcept(false) -> Result {
    static_assert(std::is_void<std::result_of_t<F(value_type const&)>>::value,
                  "Return type of the callable must be void.");
    return AndThen([&f](value_type const& value) -> Result {
      std::forward<F>(f)(value);
      return Result{Result::kInPlaceValue, value};
    });
  }

  /* VECTOR Next Construct AutosarC++17_10-A13.3.1: MD_VAC_A13.3.1_forwardingFunctionsShallNotBeOverloaded */
  /*!
   * \copydoc Result::Inspect(F&&f)const&
   * \vprivate
   */
  template <typename F>
      auto Inspect(F&& f) && noexcept(false) -> Result {
    static_assert(std::is_void<std::result_of_t<F(value_type const&&)>>::value,
                  "Return type of the callable must be void.");
    return std::move(*this).AndThen([&f](value_type&& value) -> Result {
      std::forward<F>(f)(static_cast<value_type const&&>(value));
      return Result{Result::kInPlaceValue, std::move(value)};
    });
  }

  /*!
   * \brief  Executes a given callable with the contained error as parameter. If \a this contains a value the callable
   *         is not executed.
   * \tparam F Callable, must have the signature void f(E const&). \param  f callable function.
   * \return The unchanged Result.
   * \trace  CREQ-180959
   * \vprivate
   */
  template <typename F>
  constexpr auto InspectError(F&& f) const& noexcept(false) -> Result {
    static_assert(std::is_void<std::result_of_t<F(error_type const&)>>::value,
                  "Return type of the callable must be void.");
    return OrElse([&f](error_type const& error) -> Result {
      std::forward<F>(f)(error);
      return Result{Result::kInPlaceError, error};
    });
  }

  /* VECTOR Next Construct AutosarC++17_10-A13.3.1: MD_VAC_A13.3.1_forwardingFunctionsShallNotBeOverloaded */
  /*!
   * \copydoc Result::InspectError(F&&f)const&
   * \vprivate
   */
  template <typename F>
      auto InspectError(F&& f) && noexcept(false) -> Result {
    static_assert(std::is_void<std::result_of_t<F(error_type const&&)>>::value,
                  "Return type of the callable must be void.");
    return std::move(*this).OrElse([&f](error_type&& error) -> Result {
      std::forward<F>(f)(static_cast<error_type const&&>(error));
      return Result{Result::kInPlaceError, std::move(error)};
    });
  }

  /*!
   * \brief  Checks for equality of two results.
   * \param  that Another instance.
   * \return True if both are equal else false.
   * \vprivate
   */
  auto operator==(Result const& that) const -> bool { return this->data_ == that.data_; }

  /*!
   * \brief  Checks for inequality of two results.
   * \param  that Another instance.
   * \return True if both are equal else false.
   * \vprivate
   */
  auto operator!=(Result const& that) const -> bool { return !(*this == that); }

 private:
  /*!
   * \brief Union like container holding either the Good Type T or the Error Type E.
   */
  vac::language::detail::Either<E, T> data_;
};

/* VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate */
/*!
 * \brief  Swap the contents of the two given arguments.
 * \tparam T The type of value.
 * \tparam E The type of error.
 * \param  lhs Left hand side Result to swap.
 * \param  rhs Right hand side Result to swap.
 * \vpublic
 */
template <typename T, typename E>
inline void swap(Result<T, E>& lhs, Result<T, E>& rhs) noexcept(noexcept(lhs.Swap(rhs))) {
  lhs.Swap(rhs);
}

/*!
 * \brief  Checks for equality of a result and a value.
 * \tparam T The type of value.
 * \tparam E The type of error.
 * \param  result The result to compare.
 * \param  value A value.
 * \return True if the result contains a value and both are equal
 * \vprivate
 */
template <typename T, typename E>
constexpr auto operator==(Result<T, E> const& result, T const& value) noexcept(false) -> bool {
  return result.HasValue() && (result.Value() == value);
}

/*!
 * \brief  Checks for inequality of two results.
 * \tparam T The type of value.
 * \tparam E The type of error.
 * \param  result The result to compare.
 * \param  value A value.
 * \return False if the result contains a value or both are equal.
 * \vprivate
 */
template <typename T, typename E>
constexpr auto operator!=(Result<T, E> const& result, T const& value) noexcept(false) -> bool {
  return !(result == value);
}

/*!
 * \brief  Checks for equality of a result and a value.
 * \tparam T The type of value.
 * \tparam E The type of error.
 * \param  value A value.
 * \param  result The result to compare.
 * \return True if the result contains a value and both are equal.
 * \vprivate
 */
template <typename T, typename E>
constexpr auto operator==(T const& value, Result<T, E> const& result) noexcept(false) -> bool {
  return result == value;
}

/*!
 * \brief  Checks for inequality of two results.
 * \tparam T The type of value.
 * \tparam E The type of error.
 * \param  value A value.
 * \param  result The result to compare.
 * \return False if the result contains a value or both are equal.
 * \vprivate
 */
template <typename T, typename E>
constexpr auto operator!=(T const& value, Result<T, E> const& result) noexcept(false) -> bool {
  return result != value;
}

/*!
 * \brief  Checks for equality of a result and an error.
 * \tparam T The type of value.
 * \tparam E The type of error.
 * \param  err An error.
 * \param  result The result to compare.
 * \return True if the result contains an error and both are equal.
 * \vprivate
 */
template <typename T, typename E>
constexpr auto operator==(Result<T, E> const& result, E const& err) noexcept(false) -> bool {
  return !result.HasValue() && (result.Error() == err);
}

/*!
 * \brief  Checks for inequality of two results.
 * \tparam T The type of value.
 * \tparam E The type of error.
 * \param  result The result to compare.
 * \param  err An error.
 * \return False if the result contains a value or both are equal.
 * \vprivate
 */
template <typename T, typename E>
constexpr auto operator!=(Result<T, E> const& result, E const& err) noexcept(false) -> bool {
  return !(result == err);
}

/*!
 * \brief  Checks for equality of a result and an error.
 * \tparam T The type of value.
 * \tparam E The type of error.
 * \param  result The result to compare.
 * \param  err An error.
 * \return True if the result contains an error and both are equal.
 * \vprivate
 */
template <typename T, typename E>
constexpr auto operator==(E const& err, Result<T, E> const& result) noexcept(false) -> bool {
  return result == err;
}

/*!
 * \brief  Checks for inequality of two results.
 * \tparam T The type of value.
 * \tparam E The type of error.
 * \param  err An error.
 * \param  result The result to compare.
 * \return False if the result contains a value or both are equal.
 * \vprivate
 */
template <typename T, typename E>
constexpr auto operator!=(E const& err, Result<T, E> const& result) noexcept(false) -> bool {
  return result != err;
}

/*!
 * \brief  Specialization of Result for void.
 * \tparam E The type of error.
 * \trace  CREQ-166420
 * \vpublic
 */
template <typename E>
class Result<void, E> final {
  // Assertions for E
  static_assert(std::is_object<E>::value, "E must must be object/integral type.");
  static_assert(std::is_move_constructible<E>::value,
                "E must be move-constructible. Otherwise it cannot be used as return type!");
  static_assert(std::is_copy_constructible<E>::value,
                "E must be copy-constructible. Move-only Error types may require additional r-value overloads of "
                "member functions for Result.");

  /*!
   * \brief Phantom Type to mark that the Result is empty
   */
  struct Empty {};

  /*! \brief Storage type */
  using StorageType = Result<Empty, E>;

  /*!
   * \brief  SFINAE for callable returning Result<...>.
   * \tparam F Callable.
   * \vprivate
   */
  template <typename F>
  using CallableReturnsResult = std::enable_if_t<is_result<std::result_of_t<F()>>::value>;

  /*!
   * \brief  SFINAE for callable returning void.
   * \tparam F Callable.
   * \vprivate
   */
  template <typename F>
  using CallableReturnsVoid = std::enable_if_t<std::is_void<std::result_of_t<F()>>::value>;

  /*!
   * \brief  SFINAE for callable not returning Result<...>.
   * \tparam F Callable.
   * \vprivate
   */
  template <typename F>
  using CallableReturnsNoResult =
      std::enable_if_t<(!is_result<std::result_of_t<F()>>::value) && (!std::is_void<std::result_of_t<F()>>::value)>;

  /*!
   * \brief Helper trait to determine if Result is no-throw swappable.
   * \vprivate
   */
  constexpr static bool is_nothrow_swappable{(std::is_nothrow_destructible<E>::value) &&
                                             (std::is_nothrow_move_constructible<E>::value)};

  /* VECTOR Next Construct AutosarC++17_10-A2.11.2: MD_VAC_A2.11.2_templateConstructorInTypeWithReferencesOnly */
  /*!
   * \brief  Helper wrapper template in order pass a callable to the delegate.
   * \tparam F Callable. Must accept empty argument list.
   * \vprivate
   */
  template <typename F>
  class VoidToAnyFuncT {
   public:
    /*!
     * \brief  Constructor.
     * \tparam F1 Callable. Must accept empty argument list.
     * \param  f Callable.
     */
    template <typename F1>
    constexpr explicit VoidToAnyFuncT(F1&& f) noexcept(false) : f_{std::forward<F>(f)} {}

    /* VECTOR Next Construct AutosarC++17_10-M0.1.8: MD_VAC_M0.1.8_voidFunctionHasNoExternalSideEffect */
    /* VECTOR Next Construct AutosarC++17_10-A18.9.2: MD_VAC_A18.9.2_useStdMoveIfTheValueIsARvalueReference */
    /*!
     * \brief Executes the function given in the constructor.
     */
    template <typename U>
    constexpr auto operator()(U&&) noexcept(false) -> typename std::result_of<F()>::type {
      return std::forward<F>(f_)();
    }

   private:
    /*! \brief Callable. */
    F&& f_;
  };
  /*!
   * \brief   Returns a callable ignoring it's argument.
   * \details If the function gets the provide callable \a f is executed without any arguments.
   * \tparam  F Callable. Must have the signature XX f().
   * \param   f Callable to be executed.
   * \return  Callable ignoring it's argument.
   * \vprivate
   */
  template <typename F>
  static auto VoidToAnyFunc(F&& f) noexcept(false) -> VoidToAnyFuncT<F> {
    return VoidToAnyFuncT<F>{std::forward<F>(f)};
  }

 public:
  /*! \brief Value type of the Result. */
  using value_type = void;
  /*! \brief Error type of the Result. */
  using error_type = E;
  /*! \brief In-place construct constant for value_type. */
  constexpr static in_place_type_t<value_type> kInPlaceValue{};
  /*! \brief In-place construct constant for error_type. */
  constexpr static in_place_type_t<error_type> kInPlaceError{};

  /*!
   * \brief  Build a new Result from void Result type.
   * \return Result that contains a void Result type.
   * \trace  CREQ-216932
   */
  constexpr static auto FromValue() noexcept -> Result { return Result{Result::kInPlaceValue}; }

  /*!
   * \brief  Build a new Result from the specified error (given as lvalue).
   * \param  e The error to put into the Result.
   * \return Result that contains a void Result type.
   * \trace  CREQ-216950
   */
  constexpr static auto FromError(E const& e) -> Result { return Result{Result::kInPlaceError, e}; }

  /*!
   * \brief  Build a new Result from the specified error (given as rvalue).
   * \param  e The error to put into the Result.
   * \return Result that contains a void Result type.
   * \trace  CREQ-216951
   */
  constexpr static auto FromError(E&& e) -> Result { return Result{Result::kInPlaceError, std::move(e)}; }

  /*!
   * \brief  Build a new Result from an error that is constructed in-place from the given arguments.
   * \tparam Args... The types of arguments given to this function.
   * \param  args The arguments used for constructing the error.
   * \return A Result that contains an error.
   * \vpublic
   */
  template <typename... Args, typename = std::enable_if_t<(std::is_constructible<E, Args&&...>::value) &&
                                                          (!std::is_same<E, detail::FirstTypeT<Args...>>::value) &&
                                                          (!is_result<detail::FirstTypeT<Args...>>::value)>>
  constexpr static auto FromError(Args&&... args) noexcept(false) -> Result {
    return Result{Result::kInPlaceError, std::forward<Args>(args)...};
  }

  /*!
   * \brief Construct a new Result of value type void.
   * \vpublic
   */
  constexpr Result() noexcept : Result{Result::kInPlaceValue} {}

  /* VECTOR Disable AutosarC++17_10-A12.1.5: MD_VAC_A12.1.5_useDelegatingConstructor */
  /*!
   * \brief Construct a new Result of value type void.
   * \vprivate
   */
  constexpr explicit Result(in_place_type_t<void>) noexcept : data_{StorageType::kInPlaceValue} {}

  /*!
   * \brief Construct a new Result from the specified error (given as lvalue).
   * \param e The error to put into the Result.
   * \vpublic
   */
  constexpr explicit Result(E const& e) : Result{Result::kInPlaceError, e} {}

  /*!
   * \brief Construct a new Result from the specified error (given as rvalue).
   * \param e The error to put into the Result.
   * \vpublic
   */
  constexpr explicit Result(E&& e) : Result{Result::kInPlaceError, std::move(e)} {}

  /*!
   * \brief In-place tag constructor for an Error.
   * \param args Construction parameter.
   * \vprivate
   */
  template <typename... Args>
  constexpr Result(in_place_type_t<E>, Args&&... args) noexcept(false)
      : data_{Result::kInPlaceError, std::forward<Args>(args)...} {}

  /*! \brief Storage constructor for delegate usage.
   *  \vprivate
   */
  constexpr explicit Result(StorageType&& storage) : data_{std::move(storage)} {}

  /*!
   * \brief Copy-construct a new Result from another instance.
   * \param other The other instance.
   * \vpublic
   */
  constexpr Result(Result const& other) = default;

  /* VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate */
  /*!
   * \brief Move-construct a new Result from another instance.
   * \param other The other instance.
   * \vpublic
   */
  constexpr Result(Result&& other) = default;

  /* VECTOR Next Construct AutosarC++17_10-M0.1.8: MD_VAC_M0.1.8_voidFunctionHasNoExternalSideEffect */
  /* VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate */
  /*!
   * \brief Destructor.
   * \vpublic
   */
  ~Result() = default;

  /*!
   * \brief Copy-assign another Result to this instance.
   * \param other The other instance.
   * \return *this, containing the contents of \a other.
   * \vpublic
   */
  auto operator=(Result const& other) & -> Result& = default;

  /* VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate */
  /*!
   * \brief Move-assign another Result to this instance.
   * \param other The other instance.
   * \return *this, containing the contents of \a other.
   * \vpublic
   */
  auto operator=(Result&& other) & -> Result& = default;

  /*!
   * \brief Reset error code.
   * \return
   * \vpublic
   */
  auto EmplaceValue() noexcept -> void { this->data_.EmplaceValue(); }

  /*!
   * \brief  Put a new error into this instance, constructed in-place from the given arguments.
   * \tparam Args... The types of arguments given to this function.
   * \param  args The arguments used for constructing the error.
   * \return
   * \vpublic
   */
  template <typename... Args>
  auto EmplaceError(Args&&... args) noexcept(false) -> void {
    this->data_.EmplaceError(std::forward<Args>(args)...);
  }

  /* VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate */
  /*!
   * \brief         Exchange the contents of this instance with those of \a other.
   * \param[in,out] other The other instance.
   * \return
   * \vpublic
   */
  auto Swap(Result& other) noexcept(is_nothrow_swappable) -> void { this->data_.Swap(other.data_); }

  /*!
   * \brief  Check whether *this contains a value.
   * \return True if *this contains a value, false otherwise.
   * \trace  CREQ-166422
   * \vpublic
   */
  constexpr explicit operator bool() const noexcept { return this->HasValue(); }

  /*!
   * \brief  Check whether *this contains a value.
   * \return True if *this contains no error type.
   * \trace  CREQ-166422
   * \vpublic
   */
  constexpr auto HasValue() const noexcept -> bool { return this->data_.HasValue(); }

  /*!
   * \brief   Do nothing.
   * \details This function only exists for helping with generic programming.
   * \return  void
   * \trace   CREQ-166423
   * \vpublic
   */
  constexpr void Value() const noexcept { return assert(HasValue()); }

  /*!
   * \brief   Access the contained error.
   * \details The behavior of this function is undefined if *this does not contain an error.
   * \return  A reference to the contained error.
   * \trace   CREQ-166423
   * \vpublic
   */
  constexpr auto Error() const& -> E const& { return this->data_.Error(); }

  /*!
   * \brief   Access the contained error.
   * \details The behavior of this function is undefined if *this does not contain an error.
   * \return  A rvalue reference to the contained error.
   * \trace   SWS_CORE_00431
   * \trace   RS_AP_00130
   * \trace   CREQ-166423
   * \vpublic
   */
  auto Error() && -> E&& { return std::move(data_).Error(); }

  /*!
   * \brief   Return the contained error or the given default error.
   * \details If *this contains an error, it is returned. Otherwise, the specified default error is returned,
   *          static_cast'd to E.
   * \tparam  G The type of \a defaultError.
   * \param   defaultError The error to use if *this does not contain an error.
   * \return  The error.
   * \trace   CREQ-180761
   * \vpublic
   */
  template <typename G>
  auto ErrorOr(G&& defaultError) const noexcept(false) -> E {
    return this->data_.ErrorOr(std::forward<G>(defaultError));
  }

  /*!
   * \brief  Return whether this instance contains the given error.
   * \tparam G The type of the error \a e.
   * \param  error The error to check.
   * \return True if *this contains the given error, false otherwise.
   * \vpublic
   */
  template <typename G>
  auto CheckError(G&& error) const noexcept(false) -> bool {
    return this->data_.CheckError(std::forward<G>(error));
  }

  /*!
   * \brief   Throws an exception when error occurred.
   * \details This function does not participate in overload resolution when the compiler toolchain does not support
   *          C++ exceptions.
   * \return  void
   * \throws <TYPE> The exception type associated with the contained error, exceptionsafety{Strong}.
   * \trace   CREQ-166424
   * \vpublic
   */
  auto ValueOrThrow() const noexcept(false) -> void { static_cast<void>(this->data_.ValueOrThrow()); }

  /*!
   * \brief   Apply the given Callable to the value of this instance, and return a new Result with the result of the
   *          call.
   * \details The Callable is expected to be compatible to one of these two interfaces:
   *          <code>
   *          Result<XXX, E> f();
   *          XXX f();
   *          </code>
   *          meaning that the Callable either returns a Result<XXX> or a XXX directly,
   *          where XXX can be any type that is suitable for use by class Result.
   *          The return type of this function is always Result<XXX, E>.
   *          If this instance does not contain a value, a new Result<XXX, E> is still
   *          created and returned, with the original error contents of this instance
   *          being copied into the new instance.
   * \tparam  F The type of the Callable \a f.
   * \tparam  CallableReturnsResult<F> Enable template when callable \a F returns Result<...> type.
   * \param   f The Callable.
   * \return  A new Result instance of the possibly transformed type.
   * \trace   CREQ-180561
   * \vprivate
   */
  template <typename F, typename = CallableReturnsResult<F>>
  constexpr auto Bind(F&& f) const noexcept(false) -> decltype(f()) {
    return AndThen(std::forward<F>(f));
  }

  /* VECTOR Next Construct AutosarC++17_10-A13.3.1: MD_VAC_A13.3.1_forwardingFunctionsShallNotBeOverloaded */
  /*!
   * \brief  Apply the given Callable to the value of this instance, and return a new Result with the result of the
   *         call.
   * \tparam F The type of the Callable \a f.
   * \tparam CallableReturnsNoResult<F> Enable template when callable \a f does not return Result<...> type.
   * \param  f The Callable.
   * \return A new Result instance of the possibly transformed type.
   * \trace  CREQ-180561
   * \vprivate
   */
  template <typename F, typename = CallableReturnsNoResult<F>>
  constexpr auto Bind(F&& f) const noexcept(false) -> Result<std::result_of_t<F()>, E> {
    return Map(std::forward<F>(f));
  }

  /* VECTOR Next Construct AutosarC++17_10-A13.3.1: MD_VAC_A13.3.1_forwardingFunctionsShallNotBeOverloaded */
  /*!
   * \brief  Apply the given Callable to the value of this instance, and return a new Result with the result of the
   *         call.
   * \tparam F The type of the Callable \a f.
   * \tparam CallableReturnsNoResult<F> Enable template when callable \a f does not return Result<...> type.
   * \param  f The Callable.
   * \return A new Result instance of the possibly transformed type.
   * \trace  CREQ-180561
   * \vprivate
   */
  template <typename F, typename = CallableReturnsVoid<F>>
  constexpr auto Bind(F&& f) const noexcept(false) -> Result {
    return Inspect(std::forward<F>(f));
  }

  /*!
   * \brief  Executes the given Callable if this contains a value, or returns the contained error.
   * \tparam F The type of the Callable \a f. Must have the signature XXX f(). Return type must not be void.
   * \param  f The Callable.
   * \return A new Result instance of the possibly transformed type or the contained error.
   * \remark This function can be used to transform the contained value.
   * \trace  CREQ-180560
   * \vprivate
   */
  template <typename F>
  auto Map(F&& f) const noexcept(false) -> Result<std::result_of_t<F()>, E> {
    return data_.Map(VoidToAnyFunc(std::forward<F>(f)));
  }

  /*!
   * \copydoc Result<T,E>::MapError(F&&)const&
   * \vprivate
   */
  template <typename F>
  constexpr auto MapError(F&& f) const noexcept(false) -> Result<void, std::result_of_t<F(error_type const&)>> {
    // VECTOR Next Construct AutosarC++17_10-A2.11.2: MD_VAC_A2.11.2_usingNameReusedInLocalScope
    /*! \brief Local error type alias. */
    using E1 = std::result_of_t<F(error_type const&)>;
    // VECTOR Next Construct AutosarC++17_10-A2.11.2: MD_VAC_A2.11.2_usingNameReusedInLocalScope
    /*! \brief Local result type alias. */
    using R = Result<void, E1>;
    return R{data_.MapError(std::forward<F>(f))};
  }

  /*!
   * \brief   Create a new Result with the given new value if this instance contains a value, otherwise it
   *          return the contained error.
   * \details The new value is inplace-constructed from the given parameters \a args.
   * \tparam  U Rhe type of the given value, defaulted to \a void.
   * \tparam  Args Construction arguments for \a U.
   * \param   args Arguments for the in place construction of the replacement value.
   * \return  A new Result instance with the constructed replacement value or the contained error.
   * \remark  This function can be used to replace the contained value, which type may differ from \a void.
   * \trace   CREQ-180564
   * \vprivate
   */
  template <typename U = void, typename... Args>
  constexpr auto Replace(Args&&... args) const noexcept(false) -> Result<U, E> {
    return data_.template Replace<U>(std::forward<Args>(args)...);
  }

  /*!
   * \brief  Returns an empty Result if \a this has a value, else it returns the given \a alternative.
   * \tparam E1 New error type.
   * \param  alternative Other result.
   * \return Empty Result, else \a alternative.
   * \trace  CREQ-180565
   * \vprivate
   */
  template <typename E1>
  constexpr auto Or(Result<void, E1> const& alternative) const noexcept(false) -> Result<void, E1> {
    /*! \brief Local result type alias. */
    using R = Result<void, E1>;
    return HasValue() ? R{Result::kInPlaceValue} : alternative;
  }

  /*!
   * \brief  Returns an empty Result if \a this has a value, else it transforms the contained Error using the passed
   *         function.
   * \tparam F Transform function for the error. Must be compatible to the signature Result<void,E1> f(E const&).
   * \param  f Transform function for the error.
   * \return An empty Result if \a this has a value, else the transformation result of the error.
   * \trace  CREQ-180566
   * \vprivate
   */
  template <typename F>
  constexpr auto OrElse(F&& f) const noexcept(false) -> std::result_of_t<F(error_type const&)> {
    /*! \brief Local result type alias. */
    using R = std::result_of_t<F(error_type const&)>;
    static_assert(is_result<R>::value, "Return type of f must be a Result.");
    static_assert(std::is_void<typename R::value_type>::value, "Value type must not change (must be void).");
    return HasValue() ? R{Result::kInPlaceValue} : std::forward<F>(f)(Error());
  }

  /*!
   * \brief  Returns the passed Result if this has a value, else it returns the error of this.
   * \tparam U New value type.
   * \param  other Other result.
   * \return \a other if this has a value, else the error of this.
   * \trace  CREQ-180668
   * \vprivate
   */
  template <typename U>
  constexpr auto And(Result<U, E> const& other) const noexcept(false) -> Result<U, E> {
    return data_.And(other);
  }

  /*!
   * \brief  Returns the result the passed function if \a this has a value, else returns the contained error.
   * \tparam F Transform function for the value. Must be compatible with the signature Result<XX,E> f().
   * \param  f Transform function for the error.
   * \return The transformation result of the value, else the error of \a this.
   * \trace  CREQ-180669
   * \vprivate
   */
  template <typename F>
  constexpr auto AndThen(F&& f) const noexcept(false) -> std::result_of_t<F()> {
    return data_.AndThen(VoidToAnyFunc(std::forward<F>(f)));
  }

  /*!
   * \brief  Executes a given callable if \a this contains a value. If \a this contains an error the callable is not
   *         executed.
   * \tparam F Callable, must have the signature void f().
   * \param  f Callable function.
   * \return The unchanged Result.
   * \trace  CREQ-180958
   * \vprivate
   */
  template <typename F>
  constexpr auto Inspect(F&& f) const noexcept(false) -> Result {
    return Result{data_.Inspect(VoidToAnyFunc(std::forward<F>(f)))};
  }

  /*!
   * \copydoc Result<T,E>::InspectError(F&&)const&
   * \vprivate
   */
  template <typename F>
  constexpr auto InspectError(F&& f) const noexcept(false) -> Result {
    return Result{data_.InspectError(std::forward<F>(f))};
  }

 private:
  /*! \brief Optional containing error type E, when error occurred. */
  StorageType data_;
};

/*!
 * \brief  This class is a type that contains either a value or an error.
 * \tparam T the type of value
 * \tparam E the type of error
 * \trace  CREQ-166420
 * \vprivate
 */
template <typename T, typename E>
class Result<T&, E> final {
  // Assertions for E
  static_assert(std::is_object<E>::value, "E must must be object/integral type.");
  static_assert(std::is_move_constructible<E>::value,
                "E must be move-constructible. Otherwise it cannot be used as return type!");
  static_assert(std::is_copy_constructible<E>::value,
                "E must be copy constructible! Move-only Error types may require additional r-value overloads of "
                "member functions for Result.");

  /*! \brief Wrapper type for storing the reference. */
  using WrappedReferenceType = std::reference_wrapper<T>;

  /*! \brief Storage type. */
  using StorageType = Result<WrappedReferenceType, E>;

  /*!
   * \brief  SFINAE for callable returning Result<...>.
   * \tparam F Callable.
   */
  template <typename F>
  using CallableReturnsResult = std::enable_if_t<is_result<std::result_of_t<F(T const&)>>::value>;

  /*!
   * \brief  SFINAE for callable returning void.
   * \tparam F Callable.
   */
  template <typename F>
  using CallableReturnsVoid = std::enable_if_t<std::is_void<std::result_of_t<F(T const&)>>::value>;

  /*!
   * \brief  SFINAE for callable not returning Result<...>.
   * \tparam F Callable.
   */
  template <typename F>
  using CallableReturnsNoResult = std::enable_if_t<(!is_result<std::result_of_t<F(T const&)>>::value) &&
                                                   (!std::is_void<std::result_of_t<F(T const&)>>::value)>;

  /*!
   * \brief Helper trait to determine if Result is no-throw swappable.
   */
  constexpr static bool is_nothrow_swappable{(std::is_nothrow_destructible<E>::value) &&
                                             (std::is_nothrow_move_constructible<E>::value)};

 public:
  /*! \brief Value type of the Result. */
  using value_type = T&;
  /*! \brief Error type of the Result. */
  using error_type = E;
  /*! \brief In-place construct constant for value_type. */
  constexpr static in_place_type_t<value_type> kInPlaceValue{};
  /*! \brief In-place construct constant for error_type. */
  constexpr static in_place_type_t<error_type> kInPlaceError{};

  /*!
   * \brief  Build a new Result from the specified value (given as lvalue).
   * \param  t The value to put into the Result.
   * \return Result that contains the value \a t.
   */
  static auto FromValue(T& t) noexcept -> Result { return Result{kInPlaceValue, t}; }

  /*!
   * \brief  Build a new Result from the specified error (given as lvalue).
   * \param  e The error to put into the Result.
   * \return a Result that contains the error \a e.
   * \trace  CREQ-216950
   */
  static auto FromError(E const& e) -> Result { return Result{Result::kInPlaceError, e}; }

  /*!
   * \brief  Build a new Result from the specified error (given as rvalue).
   * \param  e The error to put into the Result.
   * \return A Result that contains the error \a e.
   * \trace  CREQ-216951
   */
  static auto FromError(E&& e) -> Result { return Result{Result::kInPlaceError, std::move(e)}; }

  /*!
   * \brief  Build a new Result from an error that is constructed in-place from the given arguments.
   * \tparam Args... The types of arguments given to this function.
   * \param  args The arguments used for constructing the error.
   * \return A Result that contains an error.
   */
  template <typename... Args, typename = std::enable_if_t<(std::is_constructible<E, Args&&...>::value) &&
                                                          (!std::is_same<E, detail::FirstTypeT<Args...>>::value) &&
                                                          (!is_result<detail::FirstTypeT<Args...>>::value)>>
  static auto FromError(Args&&... args) -> Result {
    return Result{Result::kInPlaceError, std::forward<Args>(args)...};
  }

  /*!
   * \brief Construct a new Result from the specified value (given as lvalue).
   * \param t The value to put into the Result.
   */
  explicit Result(T& t) noexcept : Result{Result::kInPlaceValue, t} {} /* COV_VAC_EMPTY_CONSTRUCTOR */

  /* VECTOR Next Construct AutosarC++17_10-A12.1.5: MD_VAC_A12.1.5_useDelegatingConstructor */
  /*!
   * \brief Construct a new Result from the specified value (given as lvalue).
   * \param t The value to put into the Result.
   */
  Result(in_place_type_t<T&>, T& t) noexcept : data_{StorageType::kInPlaceValue, t} {}

  /*!
   * \brief Construct a new Result from the specified error (given as lvalue).
   * \param e The error to put into the Result.
   */
  explicit Result(E const& e) : Result{Result::kInPlaceError, e} {}

  /*!
   * \brief Construct a new Result from the specified error (given as rvalue).
   * \param e The error to put into the Result.
   */
  explicit Result(E&& e) : Result{Result::kInPlaceError, std::move(e)} {}

  /*!
   * \brief In-place tag constructor for an Error.
   * \param args Construction parameter.
   */
  template <typename... Args>
  Result(in_place_type_t<E>, Args&&... args) noexcept(false)
      : data_{Result::kInPlaceError, std::forward<Args>(args)...} {}

  /*! \brief Storage constructor for delegate usage. */
  explicit Result(StorageType&& storage) : data_{std::move(storage)} {}

  /*!
   * \brief Copy-construct a new Result from another instance.
   * \param other The other instance.
   */
  Result(Result const& other) = default;

  /* VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate */
  /*!
   * \brief Move-construct a new Result from another instance.
   * \param other The other instance.
   */
  Result(Result&& other) = default;

  /*!
   * \brief  Copy-assign another Result to this instance.
   * \param  other The other instance.
   * \return *this, containing the contents of \a other.
   */
  auto operator=(Result const& other) & -> Result& = default;

  /* VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate */
  /*!
   * \brief  Move-assign another Result to this instance.
   * \param  other The other instance.
   * \return *this, containing the contents of \a other.
   */
  auto operator=(Result&& other) & -> Result& = default;

  /*!
   * \brief Put a new value into this instance, constructed in-place from the given arguments.
   * \param t The value to put into the Result.
   * \return
   */
  auto EmplaceValue(T& t) -> void { this->data_.EmplaceValue(t); }

  /*!
   * \brief  Put a new error into this instance, constructed in-place from the given arguments.
   * \tparam Args...  The types of arguments given to this function.
   * \param  args  The arguments used for constructing the error.
   * \return
   */
  template <typename... Args>
  auto EmplaceError(Args&&... args) noexcept(false) -> void {
    this->data_.EmplaceError(std::forward<Args>(args)...);
  }

  /* VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate */
  /*!
   * \brief         Exchange the contents of this instance with those of \a other.
   * \param[in,out] other The other instance.
   * \return
   */
  auto Swap(Result& other) noexcept(is_nothrow_swappable) -> void { this->data_.Swap(other.data_); }

  /*!
   * \brief  Check whether *this contains a value.
   * \return True if *this contains a value, false otherwise.
   * \trace  CREQ-166422
   */
  constexpr explicit operator bool() const noexcept { return this->HasValue(); }

  /*!
   * \brief  Check whether *this contains a value.
   * \return True if *this contains a value, false otherwise.
   * \trace  CREQ-166422
   */
  constexpr auto HasValue() const noexcept -> bool { return this->data_.HasValue(); }

  /*!
   * \brief   Access the contained value.
   * \details This function's behavior is undefined if *this does not contain a value.
   * \return  A reference to the contained value.
   * \trace   CREQ-166423
   */
  auto operator*() const -> T const& { return Value(); }

  /*!
   * \brief   Access the contained value.
   * \details This function's behavior is undefined if *this does not contain a value.
   * \return  A pointer to the contained value.
   * \trace   CREQ-166423
   */
  auto operator-> () const -> const T* { return std::addressof(Value()); }

  /*!
   * \brief   Access the contained value.
   * \details The behavior of this function is undefined if *this does not contain a value.
   * \return  A reference to the contained value.
   * \trace   CREQ-166423
   */
  auto Value() const& -> T const& { return this->data_.Value(); }

  /*!
   * \brief  Non-const access the contained value.
   *         The behavior of this function is undefined if *this does not contain a value.
   * \return A non-const reference to the contained value.
   * \trace  CREQ-166423
   */
  auto Value() & -> T& { return this->data_.Value(); }

  /*!
   * \brief   Access the contained error.
   * \details The behavior of this function is undefined if *this does not contain an error.
   * \return  A reference to the contained error.
   * \trace   CREQ-166423
   */
  auto Error() const& -> E const& { return this->data_.Error(); }

  /*!
   * \brief   Access the contained error.
   * \details The behavior of this function is undefined if *this does not contain an error.
   * \return  A rvalue reference to the contained error.
   * \trace   CREQ-166423
   */
  auto Error() && -> E&& { return std::move(data_).Error(); }

  /*!
   * \brief   Return the contained value or the given default value.
   * \details If *this contains a value, it is returned. Otherwise, the specified default value is returned.
   * \param   defaultValue The value to use if *this does not contain a value.
   * \return  The value.
   * \trace   CREQ-180563
   */
  auto ValueOr(T& defaultValue) const& -> T& { return this->data_.ValueOr(defaultValue); }

  /*!
   * \brief   Return the contained error or the given default error.
   * \details If *this contains an error, it is returned. Otherwise, the specified default error is returned,
   *          static_cast'd to E.
   * \tparam  G The type of \a defaultError.
   * \param   defaultError The error to use if *this does not contain an error.
   * \return  The error.
   * \trace   CREQ-180761
   */
  template <typename G>
  auto ErrorOr(G&& defaultError) const noexcept(false) -> E {
    return this->data_.ErrorOr(std::forward<G>(defaultError));
  }

  /*!
   * \brief  Return whether this instance contains the given error.
   * \tparam G The type of the error \a e.
   * \param  error The error to check.
   * \return True if *this contains the given error, false otherwise.
   */
  template <typename G>
  auto CheckError(G&& error) const noexcept(false) -> bool {
    return this->data_.CheckError(std::forward<G>(error));
  }

  /*!
   * \brief   Return the contained value or throw an exception.
   * \details This function does not participate in overload resolution when the compiler toolchain does not support
   *          C++ exceptions.
   * \return  The value.
   * \throws  <TYPE> The exception type associated with the contained error, exceptionsafety{Strong}.
   * \trace   CREQ-166424
   */
  auto ValueOrThrow() const noexcept(false) -> T const& { return this->data_.ValueOrThrow(); }

  /*!
   * \brief   Returns the contained value or return the result of a function call.
   * \details If *this contains a value, it is returned. Otherwise, the specified callable is invoked and its return
   *          value which is to be compatible to type T is returned from this function.
   *          The Callable is expected to be compatible to this interface:
   *          <code>
   *          T f(E const&);
   *          </code>
   * \tparam  F The type of the Callable \a f.
   * \param   f The Callable.
   * \return  The contained value or return the result of a function call.
   * \trace   CREQ-180562
   */
  template <typename F>
  auto Resolve(F&& f) const noexcept(false) -> value_type {
    return data_.Resolve(std::forward<F>(f));
  }

  /*!
   * \brief   Apply the given Callable to the value of this instance, and return a new Result with the result of the
   *          call.
   * \details The Callable is expected to be compatible to one of these two interfaces:
   *          <code>
   *          Result<XXX, E> f(T const&);
   *          XXX f(T const&);
   *          </code>
   *          meaning that the Callable either returns a Result<XXX> or a XXX directly,
   *          where XXX can be any type that is suitable for use by class Result.
   *          The return type of this function is always Result<XXX, E>.
   *          If this instance does not contain a value, a new Result<XXX, E> is still
   *          created and returned, with the original error contents of this instance
   *          being copied into the new instance.
   * \tparam  F The type of the Callable \a f.
   * \tparam  CallableReturnsResult<F> Enable template when callable \a F returns Result<...> type.
   * \param   f The Callable.
   * \return  A new Result instance of the possibly transformed type.
   * \trace   CREQ-180561
   */
  template <typename F, typename = CallableReturnsResult<F>>
  auto Bind(F&& f) const noexcept(false) -> std::result_of_t<F(value_type&)> {
    return this->data_.Bind(std::forward<F>(f));
  }

  /* VECTOR Next Construct AutosarC++17_10-A13.3.1: MD_VAC_A13.3.1_forwardingFunctionsShallNotBeOverloaded */
  /*!
   * \brief  Apply the given Callable to the value of this instance, and return a new Result with the result of the
   *         call.
   * \tparam F The type of the Callable \a f.
   * \tparam CallableReturnsNoResult<F> Enable template when callable \a f does not return Result<...> type.
   * \param  f The Callable.
   * \return A new Result instance of the possibly transformed type.
   * \trace  CREQ-180561
   */
  template <typename F, typename = CallableReturnsNoResult<F>>
  auto Bind(F&& f) const noexcept(false) -> Result<std::result_of_t<F(value_type&)>, E> {
    return this->data_.Bind(std::forward<F>(f));
  }

  /* VECTOR Next Construct AutosarC++17_10-A13.3.1: MD_VAC_A13.3.1_forwardingFunctionsShallNotBeOverloaded */
  /*!
   * \brief  Apply the given Callable to the value of this instance, and return a new Result with the result of the
   *         call.
   * \tparam F The type of the Callable \a f.
   * \tparam CallableReturnsNoResult<F> Enable template when callable \a f does not return Result<...> type.
   * \param  f The Callable.
   * \return A new Result instance of the possibly transformed type.
   * \trace  CREQ-180561
   */
  template <typename F, typename = CallableReturnsVoid<F>>
  auto Bind(F&& f) const noexcept(false) -> Result<void, E> {
    return this->data_.Bind(std::forward<F>(f));
  }

  /*!
   * \copydoc Result<T,E>::Map(F&&)const&
   */
  template <typename F>
  auto Map(F&& f) const noexcept(false) -> Result<std::result_of_t<F(value_type&)>, E> {
    return data_.Map(std::forward<F>(f));
  }

  /*!
   * \copydoc Result<T,E>::MapError(F&&)const&
   */
  template <typename F>
  auto MapError(F&& f) const noexcept(false) -> Result<value_type, std::result_of_t<F(error_type const&)>> {
    // VECTOR Next Construct AutosarC++17_10-A2.11.2: MD_VAC_A2.11.2_usingNameReusedInLocalScope
    /*! \brief Local error type alias. */
    using E1 = std::result_of_t<F(error_type const&)>;
    // VECTOR Next Construct AutosarC++17_10-A2.11.2: MD_VAC_A2.11.2_usingNameReusedInLocalScope
    /*! \brief Local result type alias. */
    using R = Result<value_type, E1>;
    return R{data_.MapError(std::forward<F>(f))};
  }

  /*!
   * \brief   Create a new Result with the given new value if this instance contains a value, otherwise it
   *          return the contained error.
   * \details The new value is inplace-constructed from the given parameters \a args.
   * \tparam  U The type of the given value, defaulted to \a T&.
   * \tparam  Args Construction arguments for \a U.
   * \param   args Arguments for the in place construction of the replacement value.
   * \return  A new Result instance with the constructed replacement value or the contained error.
   * \remark  This function can be used to replace the contained value, which type may differ from \a T&.
   * \trace   CREQ-180564
   */
  template <typename U = T&, typename... Args>
  auto Replace(Args&&... args) const noexcept(false) -> Result<U, E> {
    return data_.template Replace<U>(std::forward<Args>(args)...);
  }

  /*!
   * \copydoc Result<T,E>::Or(Result<T, E1> const& other)const&
   */
  template <typename E1>
  auto Or(const Result<T&, E1>& alternative) const& noexcept(false) -> Result<T&, E1> {
    /*! \brief Local result type alias. */
    using R = Result<T&, E1>;
    return HasValue() ? R{Result::kInPlaceValue, GetRefUnsafe()} : alternative;
  }

  /*!
   * \brief  Returns the contained value, else transforms the contained error using the passed function.
   * \tparam F Transform function for the error. Must be compatible to the signature Result<T&,E1> f(E const&);
   * \param  f Transform function for the error.
   * \return The value of this, or the transformation result of the error.
   * \trace  CREQ-180566
   */
  template <typename F>
  auto OrElse(F&& f) const& noexcept(false) -> std::result_of_t<F(error_type const&)> {
    /*! \brief Local result type alias. */
    using R = std::result_of_t<F(error_type const&)>;
    static_assert(is_result<R>::value, "Return type of f must be a Result.");
    static_assert(std::is_same<value_type, typename R::value_type>::value, "Value type must not change.");
    return HasValue() ? R{Result::kInPlaceValue, GetRefUnsafe()} : std::forward<F>(f)(Error());
  }

  /*!
   * \brief  Returns the passed Result if this has a value, else it returns the error of this.
   * \tparam U New value type.
   * \param  other Other result.
   * \return \a other if this has a value, else the error of this.
   * \trace  CREQ-180668
   */
  template <typename U>
  auto And(const Result<U, E>& other) const noexcept(false) -> Result<U, E> {
    return data_.And(other);
  }

  /*!
   * \brief  Transforms the contained value using the passed function, else returns the contained error.
   * \tparam F Transform function for the value. Must be compatible with the signature Result<XX,E> f(T const&).
   * \param  f Transform function for the error.
   * \return The transformation result of the value, else the error of \a this.
   * \trace  CREQ-180669
   */
  template <typename F>
  auto AndThen(F&& f) const noexcept(false) -> std::result_of_t<F(value_type&)> {
    return data_.AndThen(std::forward<F>(f));
  }

  /*!
   * \brief   Returns the unchanged value if it fulfills the predicate \a pred.
   * \details The function has three possible results: (1) the contained error if \a this holds an error; (2) the
   *          contained value if \a this value fulfills the predicate \a pred; (3) the provided \a error if \a this
   *          value does not fulfill the predicate \a pred.
   * \tparam  F Unary function. Result must be convertible to bool. Must accept T const& as parameter.
   * \param   pred Unary function for checking the value.
   * \param   error Error that is returned if the value does not fulfill the predicate \a pred.
   * \return  The unchanged value if fulfills the \a pred, if not \a error is returned. Otherwise, the contained error.
   * \trace   CREQ-180760
   */
  template <typename F>
  auto Filter(F&& pred, E const& error) const noexcept(false) -> Result {
    return Result{data_.Filter(std::forward<F>(pred), error)};
  }

  /* VECTOR Next Construct AutosarC++17_10-A13.3.1: MD_VAC_A13.3.1_forwardingFunctionsShallNotBeOverloaded */
  /*!
   * \brief   Returns the unchanged value if it fulfills the predicate \a pred, which returns a Result.
   * \details The function has three possible results: (1) the contained error if \a this holds an error. Depending on
   *          the result of the \a pred call with \a this value as parameter it returns the contained value of \a this
   *          (2), or the returned error (3).
   * \tparam  F Function returning a Result. Must accept T const& as parameter.
   * \param   pred Function for checking the contained value.
   * \return  The unchanged value if fulfills the \a pred, if not the error of \a pred is returned.
   *          Otherwise, the error contained in \a this is returned.
   * \trace   CREQ-180759
   */
  template <typename F>
  auto Filter(F&& pred) const noexcept(false) -> Result {
    return Result{data_.Filter(std::forward<F>(pred))};
  }

  /*!
   * \brief   Removes the value of the result.
   * \details If \a this contains a value the function returns an empty Result, else the contained error.
   * \return  An empty Result, or the contained error.
   * \trace   CREQ-180957
   */
  auto Drop() const -> Result<void, E> { return data_.Drop(); }

  /*!
   * \copydoc Result<T,E>::Inspect(F&&)const&
   */
  template <typename F>
  auto Inspect(F&& f) const noexcept(false) -> Result {
    return Result{data_.Inspect(std::forward<F>(f))};
  }

  /*!
   * \copydoc Result<T,E>::InspectError(F&&)const&
   */
  template <typename F>
  auto InspectError(F&& f) const noexcept(false) -> Result {
    return Result{data_.InspectError(std::forward<F>(f))};
  }

  /*!
   * \brief  Checks for equality of two results.
   * \param  that Another instance.
   * \return True if both are equal else false.
   */
  auto operator==(Result const& that) const -> bool { return this->data_ == that.data_; }

  /*!
   * \brief  Checks for inequality of two results.
   * \param  that Another instance.
   * \return True if both are equal else false.
   */
  auto operator!=(Result const& that) const -> bool { return !(*this == that); }

 private:
  /*! \brief Union like container holding either the Good Type T or the Error Type E. */
  StorageType data_;

  /* VECTOR Next Construct AutosarC++17_10-A5.2.3: MD_VAC_A5.2.3_constCastReducesCodeDuplication */
  /*!
   * \brief       Returns the contained reference.
   * \return      The contained reference.
   * \internal
   *              Justification for the const_cast: The const_cast is needed to be able to use the implementation for 'T
   *              const' and 'T'. As Value() does not return a reference of class-data, it is not violation of the
   *              encapsulation of this class.
   * \endinternal
   */
  T& GetRefUnsafe() const { return const_cast<Result*>(this)->Value(); }
};

}  // namespace core
}  // namespace ara

/* COV_JUSTIFICATION_BEGIN
 * \ID COV_VAC_EMPTY_CONSTRUCTOR
 *   \ACCEPT TF
 *   \REASON The function could not be covered due to the fact that there is no instrumentation
 *           for empty braces in constructors when running the Bullselye. The function is
 *           covered by test case.
 * COV_JUSTIFICATION_END
 */

#endif  // LIB_VAC_INCLUDE_ARA_CORE_RESULT_H_
