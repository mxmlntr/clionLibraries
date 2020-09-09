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
/**        \file
 *        \brief  Specific implementation of a future for ara::core.
 *
 *      \details  ara::core::Future is an asynchronous return object similar to std::future but implemented for use with
 *                ara::core::Result.
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_ARA_CORE_FUTURE_H_
#define LIB_VAC_INCLUDE_ARA_CORE_FUTURE_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <chrono>
#include <future>
#include <memory>
#include <utility>
#include "ara/core/error_code.h"
#include "ara/core/future_error_domain.h"
#include "ara/core/result.h"

namespace ara {
namespace core {

template <typename T, typename E>
class Future;

template <typename T, typename E>
class Promise;

namespace internal {

// Forward declaration of FutureContinuation to avoid circular dependency.
template <typename T, typename E>
class FutureContinuation;

/*!
 * \brief  Sets a value or error to a Promise \a p given a Result \a res.
 * \tparam T The value type of the Promise and Result.
 * \tparam E The error type of the Promise and Result.
 * \param  p The Promise to set a value or error to.
 * \param  res The Result to get the value or error from.
 * \vprivate
 */
template <typename T, typename E>
void SetValueOrError(Promise<T, E>& p, Result<T, E> res) noexcept(false) {
  if (res.HasValue()) {
    p.set_value(res.Value());
  } else {
    p.SetError(res.Error());
  }
}

/*!
 * \brief  Sets a value or error to a Promise \a p given a Result \a res when T is void.
 * \tparam E The error type of the Promise and Result.
 * \param  p The Promise to set a value or error to.
 * \param  res The Result to get the value or error from.
 * \vprivate
 */
template <typename E>
void SetValueOrError(Promise<void, E>& p, Result<void, E> res) noexcept(false) {
  if (res.HasValue()) {
    p.set_value();
  } else {
    p.SetError(res.Error());
  }
}

/*!
 * \brief  Helper class to get a valid Future based on a given callable.
 * \remark Given the following specializations, T2 should be equal to U if this specialization is used.
 * \tparam T1 The type of the calling Future.
 * \tparam E1 The error type of the calling Future.
 * \tparam F  The callable's type.
 * \tparam U  The callable's return type.
 * \tparam T2 The unwrapped type of \a U.
 * \tparam E2 The unwrapped error type of \a U.
 * \vprivate
 */
template <typename T1, typename E1, typename F, typename U, typename T2, typename E2>
class GetFuture {
 public:
  /*!
   * \brief  Set the callable's return value to a Promise and returns a connected Future.
   * \param  func The callable, taking \a rdy_future as input parameter.
   * \param  rdy_future A ready Future.
   * \return a new Future.
   * \vprivate
   */
  auto operator()(F&& func, Future<T1, E1>&& rdy_future) -> Future<T2, E2> {
    Promise<T2, E2> promise_new;
    U func_value{func(std::move(rdy_future))};
    promise_new.set_value(func_value);
    return promise_new.get_future();
  }
};

/* VECTOR Next Construct AutosarC++17_10-M7.1.2: MD_VAC_M7.1.2_parameterAsReferencePointerToConst */
/*!
 * \brief  Helper class to get a valid Future based on a given callable which returns void.
 * \tparam T1 The type of the calling Future.
 * \tparam E1 The error type of the calling Future.
 * \tparam F  The callable's type.
 * \tparam E2 The unwrapped error type of U.
 * \vprivate
 */
template <typename T1, typename E1, typename F, typename E2>
class GetFuture<T1, E1, F, void, void, E2> {
 public:
  /*!
   * \brief  Set the callable's return value to a Promise and returns a connected Future.
   * \param  func The callable, taking \a rdy_future as input parameter.
   * \param  rdy_future A ready Future.
   * \return a new Future.
   * \vprivate
   */
  auto operator()(F&& func, Future<T1, E1>&& rdy_future) -> Future<void, E2> {
    Promise<void, E2> promise_new;
    func(std::move(rdy_future));
    promise_new.set_value();
    return promise_new.get_future();
  }
};

/* VECTOR Next Construct AutosarC++17_10-M7.1.2: MD_VAC_M7.1.2_parameterAsReferencePointerToConst */
/*!
 * \brief  Helper class to get a valid Future based on a given callable which returns Result<T2, E2>.
 * \tparam T1 The type of the calling Future.
 * \tparam E1 The error type of the calling Future.
 * \tparam F  The callable's type.
 * \tparam T2 The unwrapped type of U.
 * \tparam E2 The unwrapped error type of U.
 * \vprivate
 */
template <typename T1, typename E1, typename F, typename T2, typename E2>
class GetFuture<T1, E1, F, Result<T2, E2>, T2, E2> {
 public:
  /*!
   * \brief  Unwrapps the callable's returned Result, sets its contents to a Promise, and returns a connected Future.
   * \param  func The callable, taking \a rdy_future as input parameter.
   * \param  rdy_future A ready Future.
   * \return a new Future.
   * \vprivate
   */
  auto operator()(F&& func, Future<T1, E1>&& rdy_future) -> Future<T2, E2> {
    Promise<T2, E2> promise_new;
    SetValueOrError(promise_new, func(std::move(rdy_future)));
    return promise_new.get_future();
  }
};

/* VECTOR Next Construct AutosarC++17_10-M7.1.2: MD_VAC_M7.1.2_parameterAsReferencePointerToConst */
/*!
 * \brief  Helper class to get a valid Future based on a given callable which returns Future<T2, E2>.
 * \tparam T1 The type of the calling Future.
 * \tparam E1 The error type of the calling Future.
 * \tparam F  The callable's type.
 * \tparam T2 The unwrapped type of U.
 * \tparam E2 The unwrapped error type of U.
 * \vprivate
 */
template <typename T1, typename E1, typename F, typename T2, typename E2>
class GetFuture<T1, E1, F, Future<T2, E2>, T2, E2> {
 public:
  /*!
   * \brief  Returns the callable's returned Future.
   * \param  func The callable, taking \a rdy_future as input parameter.
   * \param  rdy_future A ready Future.
   * \return a new Future.
   * \vprivate
   */
  auto operator()(F&& func, Future<T1, E1>&& rdy_future) -> Future<T2, E2> { return func(std::move(rdy_future)); }
};

/*!
 * \brief  Helper class to unwrap the return type of the callable sent to Future::then().
 * \tparam U The callable's return type.
 * \tparam E The calling Future's error type.
 * \vprivate
 */
template <typename U, typename E>
class TypeUnwrapping {
 public:
  /*! \brief Type alias. */
  using type = Future<U, E>;

  /*! \brief Type alias. */
  using value_type = U;

  /*! \brief Type alias. */
  using error_type = E;
};

/*!
 * \brief  Helper class to do implicit Result unwrapping.
 * \tparam E The calling Future's error type.
 * \vprivate
 */
template <typename T2, typename E2, typename E>
class TypeUnwrapping<Result<T2, E2>, E> {
 public:
  /*! \brief Type alias. */
  using type = Future<T2, E2>;

  /*! \brief Type alias. */
  using value_type = T2;

  /*! \brief Type alias. */
  using error_type = E2;
};

/*!
 * \brief  Helper class to do implicit Future unwrapping.
 * \tparam E The calling Future's error type.
 * \vprivate
 */
template <typename T2, typename E2, typename E>
class TypeUnwrapping<Future<T2, E2>, E> {
 public:
  /*! \brief Type alias. */
  using type = Future<T2, E2>;

  /*! \brief Type alias. */
  using value_type = T2;

  /*! \brief Type alias. */
  using error_type = E2;
};

}  // namespace internal

/*!
 * \brief Operation status for the timed wait functions wait_for() and wait_until().
 * \trace SPEC-7552463
 * \vpublic
 */
enum class future_status : uint8_t {
  /*!
   * \brief The shared state is ready.
   */
  ready = 0,
  /*!
   * \brief The shared state did not become ready before the specified timeout has passed.
   */
  timeout = 1
};

/*!
 * \brief  Provides ara::core specific Future operations to collect the results of an asynchronous call.
 * \tparam T Value type
 * \tparam E Error type
 * \trace  SPEC-7552464
 * \trace  CREQ-200633
 * \vpublic
 */
template <typename T, typename E = ErrorCode>
class Future final {
  /*!
   * \brief Alias for Result.
   */
  using R = Result<T, E>;

 public:
  /*!
   * \brief The future continuation pointer type.
   */
  using FutureContinuationPtr = std::shared_ptr<ara::core::internal::FutureContinuation<T, E>>;

  /*!
   * \brief Default constructor.
   * \trace SPEC-7552465
   * \vpublic
   */
  Future() noexcept = default;

  /*!
   * \brief Default copy constructor deleted.
   * \trace SPEC-7552466
   * \vpublic
   */
  Future(Future const&) = delete;

  /*!
   * \brief Move constructor.
   * \param other The other instance.
   * \trace SPEC-7552467
   * \vpublic
   */
  Future(Future&& other) noexcept = default;

  /*!
   * \brief Destructor.
   * \trace SPEC-7552468
   * \vpublic
   */
  ~Future() noexcept = default;

  /*!
   * \brief  Default copy assignment operator deleted.
   * \return
   * \trace  SPEC-7552469
   * \vpublic
   */
  Future& operator=(Future const&) & = delete;

  /*!
   * \brief  Move assignment operator.
   * \param  other The other instance.
   * \return A reference to "*this".
   * \trace  SPEC-7552470
   * \vpublic
   */
  Future& operator=(Future&& other) & noexcept = default;

  /* VECTOR Next Construct VectorC++-V6-6-1: MD_VAC_V6-6-1_multipleExit */
  /* VECTOR Next Construct AutosarC++17_10-M0.3.1: MD_VAC_M0.3.1_deadExceptionHandler */
  /* VECTOR Next Construct AutosarC++17_10-A15.3.1: MD_VAC_A15.3.1_uncheckedExecptionsShouldBeHandledOnlyInMain */
  /*!
   * \brief  Get the result (does not throw exceptions).
   * \return The value stored in the shared state.
   * \trace  SPEC-7552472
   * \trace  CREQ-200636
   * \vpublic
   */
  R GetResult() noexcept {
    try {
      /* VECTOR Next Line AutosarC++17_10-M6.6.5: MD_VAC_M6.6.5_multipleExit */
      return delegate_future_.get();
    } catch (std::future_error const& ex) {
      std::error_code const& ec{ex.code()};
      // std::future::get() can only throw no_state or broken_promise.
      future_errc err{future_errc::no_state};
      if (ec == std::future_errc::broken_promise) {
        err = future_errc::broken_promise;
      }
      /* VECTOR Next Line AutosarC++17_10-M6.6.5: MD_VAC_M6.6.5_multipleExit */
      return R::FromError(err);
    }
  }

  /*!
   * \brief  Get the value.
   * \return The value stored in the shared state.
   * \throws ara::core::FutureException Internal Future/Promise errors see FutureErrc.
   * \throws ara::core::Exception Any error which is set in SetError. The concrete exception type depends on the
   *         Exception type corresponding to the error domain of the ErrorCode.
   * \trace  SPEC-7552471
   * \trace  CREQ-200636
   * \vpublic
   */
  T get() {
    R res{GetResult()};
    if (!res.HasValue()) {
      res.Error().ThrowAsException();
    }
    return std::move(res).Value();
  }

  /*!
   * \brief  Check if the Future has any shared state.
   * \return True if *this refers to a shared state, otherwise false.
   * \trace  SPEC-7552473
   * \trace  CREQ-200637
   * \vpublic
   */
  bool valid() const noexcept { return delegate_future_.valid(); }

  /*!
   * \brief Block until the shared state is ready.
   * \trace SPEC-7552474
   * \trace CREQ-200639
   * \vpublic
   */
  void wait() const noexcept(false) { delegate_future_.wait(); }

  /*!
   * \brief  Wait for a specified relative time.
   * \tparam Rep An arithmetic type representing the number of ticks.
   * \tparam Period Type representing the tick period.
   * \param  timeout_duration Maximum duration to wait for.
   * \return Status that indicates whether the timeout hit or if a value is available.
   * \trace  SPEC-7552475
   * \trace  CREQ-200638
   * \vpublic
   */
  template <typename Rep, typename Period>
  future_status wait_for(std::chrono::duration<Rep, Period> const& timeout_duration) const noexcept(false) {
    future_status retval;
    std::future_status f_stat{delegate_future_.wait_for(timeout_duration)};
    if (std::future_status::ready == f_stat) {
      retval = future_status::ready;
    } else {
      retval = future_status::timeout;
    }
    return retval;
  }

  /*!
   * \brief  Wait until a specified absolute time.
   * \tparam Clock The clock on which this time point is measured.
   * \tparam Duration A std::chrono::duration type used to measure the time since epoch.
   * \param  abs_time maximum time point to block until.
   * \return Status that indicates whether the time was reached or if a value is available.
   * \trace  SPEC-7552476
   * \trace  CREQ-200640
   * \vpublic
   */
  template <typename Clock, typename Duration>
  future_status wait_until(std::chrono::time_point<Clock, Duration> const& abs_time) const noexcept(false) {
    future_status retval;
    std::future_status f_stat{delegate_future_.wait_until(abs_time)};
    if (std::future_status::ready == f_stat) {
      retval = future_status::ready;
    } else {
      retval = future_status::timeout;
    }
    return retval;
  }

  /* VECTOR Next Construct AutosarC++17_10-A18.5.8: MD_VAC_A18.5.8_localObjectsShallBeAllocatedOnStack */
  /*!
   * \brief   Set a continuation for when the shared state is ready.
   * \details When func is called, it is guaranteed that get() and GetResult() will not block.
   * \remark  func may be called in the context of this call or in the context of Promise::set_value() or
   *          Promise::SetError() or somewhere else.
   *          Warning: This function might use dynamic memory allocation. Use with caution!
   * \tparam  Type of continuation function.
   * \param   func A continuation function to be attached.
   * \return  A new Future instance for the result of the continuation.
   * \trace   SPEC-7552477
   * \trace   CREQ-200641
   * \vpublic
   */
  template <typename F>
  auto then(F&& func) noexcept(false) -> typename internal::TypeUnwrapping<decltype(func(std::move(*this))), E>::type {
    /*! \brief Alias for the type of the value contained in the future returned from this function. */
    using U = decltype(func(std::move(*this)));

    /*! \brief Alias for the type of the value contained in the future returned from this function. */
    using T2 = typename internal::TypeUnwrapping<U, E>::value_type;

    /*! \brief Alias for the type of the value contained in the future returned from this function. */
    using E2 = typename internal::TypeUnwrapping<U, E>::error_type;

    Future<T2, E2> future;

    // Call the function if future state is ready.
    if (is_ready()) {
      future = internal::GetFuture<T, E, F, U, T2, E2>()(std::forward<F>(func), std::move(*this));

      // Save the call back function in continuation to be called later when promise is set.
    } else {
      // Move the future_continuation pointer to f_continuation to be used after this future object gets invalidated.
      FutureContinuationPtr f_continuation{std::move(future_continuation_)};
      future = f_continuation->SetCallBackHandler(std::forward<F>(func), std::move(*this));
    }

    return future;
  }

  /*!
   * \brief  Return true only when the shared state is ready. This method will return immediately and shall not do a
   *         blocking wait.
   * \remark Because the std::future has no future::get_state, we will use the
   *         capabilities we have and use future::wait_for to check for the current status.
   * \return True if the future contains a value (or exception), false if not.
   * \trace  SPEC-7552478
   * \trace  CREQ-200642
   * \vpublic
   */
  bool is_ready() const { return std::future_status::ready == delegate_future_.wait_for(std::chrono::seconds::zero()); }

 private:
  /*!
   * \brief Parameterized constructor.
   * \param delegate_future object to std::future.
   * \param future_continuation Future continuation pointer to manage the call back function.
   */
  Future(std::future<R>&& delegate_future, FutureContinuationPtr future_continuation) noexcept
      : delegate_future_(std::move(delegate_future)), future_continuation_(future_continuation) {}

  /*!
   * \brief Delegation object to std::future.
   */
  std::future<R> delegate_future_;

  /*!
   * \brief Future continuation pointer to manage the call back function.
   */
  FutureContinuationPtr future_continuation_;

  /* VECTOR Next Line AutosarC++17_10-A11.3.1: MD_VAC_A11.3.1_doNotUseFriend */
  friend class Promise<T, E>;
};

/*!
 * \brief  void specialized Future
 * \tparam E Error type
 * \trace  CREQ-158607
 * \vpublic
 */
template <typename E>
class Future<void, E> final {
  /*!
   * \brief Alias for result
   */
  using R = Result<void, E>;

 public:
  /*!
   * \brief The future continuation pointer type.
   */
  using FutureContinuationPtr = std::shared_ptr<ara::core::internal::FutureContinuation<void, E>>;

  /*!
   * \brief Default constructor
   * \trace SPEC-7552465
   * \vpublic
   */
  Future() noexcept = default;

  /*!
   * \brief Default copy constructor deleted
   * \trace SPEC-7552466
   * \vpublic
   */
  Future(Future const&) = delete;

  /*!
   * \brief Move constructor.
   * \param other The other instance.
   * \trace SPEC-7552467
   * \vpublic
   */
  Future(Future&& other) noexcept = default;

  /*!
   * \brief Destructor.
   * \trace SPEC-7552468
   * \vpublic
   */
  ~Future() noexcept = default;

  /*!
   * \brief  Default copy assignment operator deleted.
   * \return
   * \trace  SPEC-7552469
   * \vpublic
   */
  Future& operator=(Future const&) & = delete;

  /*!
   * \brief  Move assignment operator.
   * \param  other The other instance.
   * \return A reference to "*this".
   * \trace  SPEC-7552470
   * \vpublic
   */
  Future& operator=(Future&& other) & noexcept = default;

  /* VECTOR Next Construct AutosarC++17_10-M0.3.1: MD_VAC_M0.3.1_deadExceptionHandler */
  /* VECTOR Next Construct AutosarC++17_10-A15.3.1: MD_VAC_A15.3.1_uncheckedExecptionsShouldBeHandledOnlyInMain */
  /* VECTOR Next Construct AutosarC++17_10-V6-6-1: MD_VAC_V6-6-1_multipleExit */
  /*!
   * \brief  Get the result (does not throw exceptions).
   * \return The value stored in the shared state.
   * \trace  SPEC-7552472
   * \trace  CREQ-200636
   * \vpublic
   */
  R GetResult() noexcept {
    try {
      /* VECTOR Next Line AutosarC++17_10-M6.6.5: MD_VAC_M6.6.5_multipleExit */
      return delegate_future_.get();
    } catch (std::future_error const& ex) {
      // TODO(vsarcsesu): Considering removing this and let delegate_future_ throw all the way instead of catching here.
      std::error_code const& ec{ex.code()};
      // std::future::get() can only throw no_state or broken_promise.
      future_errc err{future_errc::no_state};
      if (ec == std::future_errc::broken_promise) {
        err = future_errc::broken_promise;
      }
      /* VECTOR Next Line AutosarC++17_10-M6.6.5: MD_VAC_M6.6.5_multipleExit */
      return R::FromError(err);
    }
  }

  /*!
   * \brief  Get the result.
   * \return The value stored in the shared state.
   * \throws ara::core::FutureException Internal Future/Promise errors see FutureErrc.
   * \throws ara::core::Exception Any error which is set in SetError. The concrete exception type depends on the
   *         Exception type corresponding to the error domain of the ErrorCode.
   * \trace  SPEC-7552471
   * \trace  CREQ-200636
   * \vpublic
   */
  void get() {
    R res{GetResult()};
    if (!res.HasValue()) {
      res.Error().ThrowAsException();
    }
    return std::move(res).Value();
  }

  /*!
   * \brief  Check if the Future has any shared state.
   * \return True if *this refers to a shared state, otherwise false.
   * \trace  SPEC-7552473
   * \trace  CREQ-200637
   * \vpublic
   */
  bool valid() const noexcept { return delegate_future_.valid(); }

  /*!
   * \brief Block until the shared state is ready.
   * \trace SPEC-7552474
   * \trace CREQ-200639
   * \vpublic
   */
  void wait() const noexcept(false) { delegate_future_.wait(); }

  /*!
   * \brief  Wait for a specified relative time.
   * \tparam Rep An arithmetic type representing the number of ticks.
   * \tparam Period Type representing the tick period.
   * \param  timeout_duration Maximum duration to block for.
   * \return Status that indicates whether the timeout hit or if a value is available.
   * \trace  SPEC-7552475
   * \trace  CREQ-200638
   * \vpublic
   */
  template <typename Rep, typename Period>
  future_status wait_for(std::chrono::duration<Rep, Period> const& timeout_duration) const noexcept(false) {
    future_status retval;
    std::future_status f_stat{delegate_future_.wait_for(timeout_duration)};
    if (std::future_status::ready == f_stat) {
      retval = future_status::ready;
    } else {
      retval = future_status::timeout;
    }
    return retval;
  }

  /*!
   * \brief  Wait until a specified absolute time.
   * \tparam Clock The clock on which this time point is measured.
   * \tparam Duration A std::chrono::duration type used to measure the time since epoch.
   * \param  abs_time maximum time point to block until.
   * \return Status that indicates whether the timeout hit or if a value is available.
   * \trace  SPEC-7552476
   * \trace  CREQ-200640
   * \vpublic
   */
  template <typename Clock, typename Duration>
  future_status wait_until(std::chrono::time_point<Clock, Duration> const& abs_time) const noexcept(false) {
    future_status retval;
    std::future_status f_stat{delegate_future_.wait_until(abs_time)};
    if (std::future_status::ready == f_stat) {
      retval = future_status::ready;
    } else {
      retval = future_status::timeout;
    }
    return retval;
  }

  /* VECTOR Next Construct AutosarC++17_10-A18.5.8: MD_VAC_A18.5.8_localObjectsShallBeAllocatedOnStack */
  /*!
   * \brief   Set a continuation for when the shared state is ready.
   * \details When func is called, it is guaranteed that get() and GetResult() will not block.
   * \remark  func may be called in the context of this call or in the context of Promise::set_value() or
   *          Promise::SetError() or somewhere else.
   * \tparam  Type of continuation function.
   * \param   func A continuation function to be attached.
   * \return  A new Future instance for the result of the continuation.
   * \trace   SPEC-7552477
   * \trace   CREQ-200641
   * \vpublic
   */
  template <typename F>
  auto then(F&& func) noexcept(false) -> typename internal::TypeUnwrapping<decltype(func(std::move(*this))), E>::type {
    /*! \brief Alias for the type of the value contained in the future returned from this function. */
    using U = decltype(func(std::move(*this)));

    /*! \brief Alias for the type of the value contained in the future returned from this function. */
    using T2 = typename internal::TypeUnwrapping<U, E>::value_type;

    /*! \brief Alias for the type of the value contained in the future returned from this function. */
    using E2 = typename internal::TypeUnwrapping<U, E>::error_type;

    Future<T2, E2> future;

    // Call the function if future state is ready.
    if (is_ready()) {
      future = internal::GetFuture<void, E, F, U, T2, E2>()(std::forward<F>(func), std::move(*this));

      // Save the call back function in continuation to be called later when promise is set.
    } else {
      // Move the future_continuation pointer to f_continuation to be used after this future object gets invalidated.
      FutureContinuationPtr f_continuation{std::move(future_continuation_)};
      future = f_continuation->SetCallBackHandler(std::forward<F>(func), std::move(*this));
    }

    return future;
  }

  /*!
   * \brief  Return true only when the shared state is ready. This method will return immediately and shall not do a
   *         blocking wait.
   * \remark Because the std::future has no future::get_state, we will use the
   *         capabilities we have and use future::wait_for to check for the current status.
   * \return True if the future contains a value (or exception), false if not.
   * \trace  SPEC-7552478
   * \trace  CREQ-200642
   * \vpublic
   */
  bool is_ready() const { return std::future_status::ready == delegate_future_.wait_for(std::chrono::seconds::zero()); }

 private:
  /*!
   * \brief Parameterized constructor
   * \param delegate_future object to std::future.
   * \param future_continuation Future continuation pointer to manage the call back function.
   */
  Future(std::future<R>&& delegate_future, FutureContinuationPtr future_continuation) noexcept
      : delegate_future_(std::move(delegate_future)), future_continuation_(future_continuation) {}

  /*!
   * \brief Delegation object to std::future.
   */
  std::future<R> delegate_future_;

  /*!
   * \brief Future continuation pointer to manage the call back function.
   */
  FutureContinuationPtr future_continuation_;

  /* VECTOR Next Line AutosarC++17_10-A11.3.1: MD_VAC_A11.3.1_doNotUseFriend */
  friend class Promise<void, E>;
};

}  // namespace core
}  // namespace ara

#endif  // LIB_VAC_INCLUDE_ARA_CORE_FUTURE_H_
