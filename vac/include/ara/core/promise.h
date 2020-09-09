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
/**        \file  ara/core/promise.h
 *        \brief  Specific implementation of a promise for ara::core.
 *
 *      \details  ara::core::Promise is the corresponding promise that returns an ara::core::Future.
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_ARA_CORE_PROMISE_H_
#define LIB_VAC_INCLUDE_ARA_CORE_PROMISE_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <functional>
#include <future>
#include <memory>
#include <utility>

#include "ara/core/future.h"
#include "ara/core/internal/future_continuation.h"

namespace ara {
namespace core {

/*!
 * \brief  ara::core specific Promise.
 * \tparam T Value type.
 * \tparam E Error type.
 * \trace  SPEC-7552479
 * \trace  CREQ-200633
 * \vpublic
 */
template <typename T, typename E = ErrorCode>
class Promise final {
  /*!
   * \brief Alias for Result.
   */
  using R = Result<T, E>;

 public:
  /*!
   * \brief The promised type.
   */
  using ValueType = T;

  /*!
   * \brief The future continuation pointer type.
   */
  using FutureContinuationPtr = std::shared_ptr<ara::core::internal::FutureContinuation<ValueType, E>>;

  /*!
   * \brief Default constructor.
   * \trace SPEC-7552480
   * \vpublic
   */
  Promise() = default;

  /*!
   * \brief Default copy constructor deleted.
   * \trace SPEC-7552481
   * \vpublic
   */
  Promise(Promise const&) = delete;

  /*!
   * \brief Move constructor.
   * \trace SPEC-7552482
   * \vpublic
   */
  Promise(Promise&&) noexcept = default;

  /*!
   * \brief Destructor.
   * \trace SPEC-7552483
   * \vpublic
   */
  ~Promise() = default;

  /*!
   * \brief  Default copy assignment operator deleted.
   * \return
   * \trace  SPEC-7552484
   * \vpublic
   */
  Promise& operator=(Promise const&) & = delete;

  /*!
   * \brief  Move assignment operator.
   * \return A reference to "*this".
   * \trace  SPEC-7552485
   * \vpublic
   */
  Promise& operator=(Promise&&) & noexcept = default;

  /*!
   * \brief Exchanges the shared states of this and other.
   * \param other The other instance.
   * \trace SPEC-7552486
   * \trace CREQ-200643
   * \vpublic
   */
  void swap(Promise& other) noexcept {
    using std::swap;
    swap(delegate_promise_, other.delegate_promise_);
    swap(future_continuation_, other.future_continuation_);
  }

  /*!
   * \brief   Return a Future with the same shared state.
   * \details The returned Future is set as soon as this Promise receives the result or an error.
   *          This method must only be called once as it is not allowed to have multiple Futures per Promise.
   * \return  Future with same shared state.
   * \trace   SPEC-7552487
   * \trace   CREQ-200644
   * \vpublic
   */
  Future<ValueType, E> get_future() {
    future_continuation_ = std::make_shared<ara::core::internal::FutureContinuation<ValueType, E>>();
    return ara::core::Future<ValueType, E>(delegate_promise_.get_future(), future_continuation_);
  }

  /*!
   * \brief Copy result into the Future.
   * \param value Value to be set.
   * \trace SPEC-7552488
   * \trace CREQ-200645
   * \vpublic
   */
  void set_value(ValueType const& value) {
    R r{R::template FromValue(value)};

    SetValueAndExecuteCallBack(r);
  }

  /*!
   * \brief Move the result into the Future.
   * \param value Value to be set.
   * \trace SPEC-7552489
   * \trace CREQ-200645
   * \vpublic
   */
  void set_value(ValueType&& value) {
    R r{R::FromValue(std::move(value))};

    SetValueAndExecuteCallBack(std::move(r));
  }

  /*!
   * \brief Move an error into the Future.
   * \param err The error to store.
   * \trace SPEC-7552490
   * \trace CREQ-200646
   * \vpublic
   */
  void SetError(E&& err) {
    R r{R::FromError(std::move(err))};

    SetValueAndExecuteCallBack(std::move(r));
  }

  /*!
   * \brief Copy an error into the Future.
   * \param err The error to store.
   * \trace SPEC-7552491
   * \trace CREQ-200646
   * \vpublic
   */
  void SetError(E const& err) {
    R r{R::template FromError(err)};

    SetValueAndExecuteCallBack(r);
  }

 private:
  /*!
   * \brief Delegation object to std::promise.
   */
  std::promise<R> delegate_promise_;

  /*!
   * \brief Future continuation object to manage the call back function.
   */
  FutureContinuationPtr future_continuation_;

  /*!
   * \brief Set value and execute call back if one exist.
   * \param r The Result to set.
   */
  void SetValueAndExecuteCallBack(R const& r) {
    if ((future_continuation_ != nullptr) && future_continuation_->IsCallBackSet()) {
      delegate_promise_.set_value(r);
      future_continuation_->ExecuteCallBack();
    } else {
      delegate_promise_.set_value(r);
    }
  }
  /*!
   * \brief Set value and execute call back if one exist.
   * \param r The Result to set.
   */
  void SetValueAndExecuteCallBack(R&& r) {
    if ((future_continuation_ != nullptr) && future_continuation_->IsCallBackSet()) {
      delegate_promise_.set_value(std::move(r));
      future_continuation_->ExecuteCallBack();
    } else {
      delegate_promise_.set_value(std::move(r));
    }
  }
};

/*!
 * \brief Promise specialization for void.
 * \tparam E Error type.
 * \trace CREQ-200642
 * \vpublic
 */
template <typename E>
class Promise<void, E> final {
  /*!
   * \brief Alias for Result.
   */
  using R = Result<void, E>;

 public:
  /*!
   * \brief The promised type.
   */
  using ValueType = void;

  /*!
   * \brief The future continuation pointer type.
   */
  using FutureContinuationPtr = std::shared_ptr<ara::core::internal::FutureContinuation<ValueType, E>>;

  /*!
   * \brief Default constructor.
   * \trace SPEC-7552480
   * \vpublic
   */
  Promise() = default;

  /*!
   * \brief Default copy constructor deleted.
   * \trace SPEC-7552481
   * \vpublic
   */
  Promise(Promise const&) = delete;

  /*!
   * \brief Move constructor.
   * \trace SPEC-7552482
   * \vpublic
   */
  Promise(Promise&&) noexcept = default;

  /*!
   * \brief Destructor.
   * \trace SPEC-7552483
   * \vpublic
   */
  ~Promise() = default;

  /*!
   * \brief  Default copy assignment operator deleted.
   * \return
   * \trace  SPEC-7552484
   * \vpublic
   */
  Promise& operator=(Promise const&) & = delete;

  /*!
   * \brief  Move assignment operator.
   * \return A reference to "*this".
   * \trace  SPEC-7552485
   * \vpublic
   */
  Promise& operator=(Promise&&) & noexcept = default;

  /*!
   * \brief Exchanges the shared states of this and other.
   * \param other The other instance.
   * \trace SPEC-7552486
   * \trace CREQ-200643
   * \vpublic
   */
  void swap(Promise& other) noexcept {
    using std::swap;
    swap(delegate_promise_, other.delegate_promise_);
    swap(future_continuation_, other.future_continuation_);
  }

  /*!
   * \brief   Return a Future with the same shared state.
   * \details The returned Future is set as soon as this Promise receives the result or an error.
   *          This method must only be called once as it is not allowed to have multiple Futures per Promise.
   * \return  Future with same shared state.
   * \trace   CREQ-200644
   * \trace   SPEC-7552487
   * \vpublic
   */
  Future<ValueType, E> get_future() {
    future_continuation_ = std::make_shared<ara::core::internal::FutureContinuation<ValueType, E>>();
    return ara::core::Future<ValueType, E>(delegate_promise_.get_future(), future_continuation_);
  }

  /*!
   * \brief Set a value with void.
   * \trace CREQ-200645
   * \vpublic
   */
  void set_value() {
    R r{R::FromValue()};

    SetValueAndExecuteCallBack(r);
  }

  /*!
   * \brief Move an error into the Future.
   * \param err The error to store.
   * \trace SPEC-7552490
   * \trace CREQ-200646
   * \vpublic
   */
  void SetError(E&& err) {
    R r{R::FromError(std::move(err))};

    SetValueAndExecuteCallBack(std::move(r));
  }

  /*!
   * \brief Copy an error into the Future.
   * \param err The error to store.
   * \trace SPEC-7552491
   * \trace CREQ-200646
   * \vpublic
   */
  void SetError(E const& err) {
    R r{R::template FromError(err)};

    SetValueAndExecuteCallBack(r);
  }

 private:
  /*!
   * \brief Delegation object to std::promise.
   */
  std::promise<R> delegate_promise_;

  /*!
   * \brief Future continuation object to manage the call back function.
   */
  FutureContinuationPtr future_continuation_;

  /*!
   * \brief Set value and execute call back if one exist.
   * \param r The Result to set.
   */
  void SetValueAndExecuteCallBack(R const& r) {
    if ((future_continuation_ != nullptr) && future_continuation_->IsCallBackSet()) {
      delegate_promise_.set_value(r);
      future_continuation_->ExecuteCallBack();
    } else {
      delegate_promise_.set_value(r);
    }
  }

  /*!
   * \brief Set value and execute call back if one exist.
   * \param r The Result to set.
   */
  void SetValueAndExecuteCallBack(R&& r) {
    if ((future_continuation_ != nullptr) && future_continuation_->IsCallBackSet()) {
      delegate_promise_.set_value(std::move(r));
      future_continuation_->ExecuteCallBack();
    } else {
      delegate_promise_.set_value(std::move(r));
    }
  }
};

}  // namespace core
}  // namespace ara

#endif  // LIB_VAC_INCLUDE_ARA_CORE_PROMISE_H_
