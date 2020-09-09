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
/*!        \file  ara/core/internal/future_continuation.h
 *        \brief  Continuation class to register, store and execute the callback for future then.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_ARA_CORE_INTERNAL_FUTURE_CONTINUATION_H_
#define LIB_VAC_INCLUDE_ARA_CORE_INTERNAL_FUTURE_CONTINUATION_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <functional>
#include <memory>
#include <utility>
#include "ara/core/future.h"
#include "ara/core/promise.h"

namespace ara {
namespace core {
namespace internal {

/* VECTOR Next Construct AutosarC++17_10-A12.8.6: MD_VAC_A12.8.6_definedDeletedInBassClass */
/*!
 * \brief CallBackHolder interface.
 * \vprivate
 */
class CallBackHolderInterface {
 public:
  /*!
   * \brief Default constructor.
   */
  CallBackHolderInterface() = default;

  /*!
   * \brief Default copy constructor.
   */
  CallBackHolderInterface(CallBackHolderInterface const& other) = default;

  /*!
   * \brief Default move constructor.
   */
  CallBackHolderInterface(CallBackHolderInterface&& other) noexcept = default;

  /*!
   * \brief Deleted copy assignment operator.
   */
  CallBackHolderInterface& operator=(CallBackHolderInterface const& other) & = delete;

  /*!
   * \brief Deleted move assignment operator.
   */
  CallBackHolderInterface& operator=(CallBackHolderInterface&& other) & = delete;

  /*!
   * \brief Destructor.
   */
  virtual ~CallBackHolderInterface() noexcept {}

  /*!
   * \brief Executes the callback.
   */
  virtual void ExecuteCallBack() = 0;

  /*!
   * \brief Checks if callback is not nullptr.
   */
  virtual bool IsExecutable() = 0;
};

/* VECTOR Next Construct VectorC++-V11-0-2: MD_VAC_V11-0-2_mutableUnionMemberShallBePrivate */
/* VECTOR Next Construct AutosarC++17_10-A12.4.1: MD_VAC_A12.4.1_destructorOfABaseClassShallBePublicVirtual */
/*!
 * \brief  Callback holder base class to perform the callback execution.
 * \tparam T is type for the calling Future.
 * \tparam E is the error type for the calling Future.
 * \tparam U is the type returned back from the registered call back function.
 * \tparam T2 The unwrapped value type returned back from the registered call back function.
 * \tparam E2 The unwrapped error type returned back from the registered call back function.
 * \vprivate
 */
template <typename T, typename E, typename U, typename T2, typename E2>
class CallBackHolderBase : public CallBackHolderInterface {
 public:
  /*!
   * \brief Callback handler type.
   */
  using CallBackHandler = std::function<U(ara::core::Future<T, E>)>;

  /*!
   * \brief Constructor.
   * \param new_promise New promise to be set when the callback is called (it needs to be set with the return value
   *        from the callback).
   * \param calling_future The future on which the then function is called (calling future needs to be passed as a
   *        parameter to the call back function and it gets invalidated once then() is called).
   * \param handler The handler to the callback function.
   */
  CallBackHolderBase(ara::core::Promise<T2, E2>&& new_promise, ara::core::Future<T, E>&& calling_future,
                     CallBackHandler handler)
      : CallBackHolderInterface(),
        new_promise_(std::make_unique<ara::core::Promise<T2, E2>>(std::move(new_promise))),
        future_(std::move(calling_future)),
        callback_handler_(std::move(handler)) {}

  /*!
   * \brief  Checks if callback is not nullptr.
   * \return true if callback_handler_ is not nullptr.
   */
  bool IsExecutable() override { return (callback_handler_ != nullptr); }

 protected:
  /*!
   * \brief New promise to be set when the callback is called.
   */
  std::unique_ptr<ara::core::Promise<T2, E2>> new_promise_;

  /*!
   * \brief Future on which the function is called.
   */
  ara::core::Future<T, E> future_;

  /*!
   * \brief Callback handler.
   */
  CallBackHandler callback_handler_;
};

/* VECTOR Next Construct AutosarC++17_10-A12.8.3: MD_VAC_A12.8.3_dontReadAccessAMovedFromObject */
/*!
 * \brief  Callback holder class to perform the callback execution.
 * \tparam T The type for the calling Future.
 * \tparam E The error type for the calling Future.
 * \tparam U The type returned back from the registered call back function.
 * \tparam T2 The unwrapped value type returned back from the registered call back function.
 * \tparam E2 The unwrapped error type returned back from the registered call back function.
 * \vprivate
 */
template <typename T, typename E, typename U, typename T2, typename E2>
class CallBackHolder : public CallBackHolderBase<T, E, U, T2, E2> {
 public:
  using CallBackHolderBase<T, E, U, T2, E2>::CallBackHolderBase;

  /*!
   * \brief   Function to execute the registered call back.
   * \details Sets the new promise.
   */
  void ExecuteCallBack() override {
    U ret_val{this->callback_handler_(std::move(this->future_))};
    this->new_promise_->set_value(ret_val);
  }
};

/* VECTOR Next Construct AutosarC++17_10-A12.8.3: MD_VAC_A12.8.3_dontReadAccessAMovedFromObject */
/*!
 * \brief  Specialization callback holder class to perform the callback execution for when U is void.
 * \tparam T is type for the calling future.
 * \tparam E is the error type for the calling Future.
 * \tparam T2 The unwrapped value type returned back from the registered call back function.
 * \tparam E2 The unwrapped error type returned back from the registered call back function.
 * \vprivate
 */
template <typename T, typename E, typename T2, typename E2>
class CallBackHolder<T, E, void, T2, E2> : public CallBackHolderBase<T, E, void, T2, E2> {
 public:
  using CallBackHolderBase<T, E, void, T2, E2>::CallBackHolderBase;

  /*!
   * \brief   Function to execute the registered call back.
   * \details Sets the new promise.
   */
  void ExecuteCallBack() override {
    this->callback_handler_(std::move(this->future_));
    this->new_promise_->set_value();
  }
};

/* VECTOR Next Construct AutosarC++17_10-A12.8.3: MD_VAC_A12.8.3_dontReadAccessAMovedFromObject */
/*!
 * \brief  Specialization callback holder class to perform the callback execution for when U is Future<T2, E2>.
 * \tparam T is type for the calling future.
 * \tparam E is the error type for the calling Future.
 * \tparam T2 The unwrapped value type returned back from the registered call back function.
 * \tparam E2 The unwrapped error type returned back from the registered call back function.
 * \vprivate
 */
template <typename T, typename E, typename T2, typename E2>
class CallBackHolder<T, E, Future<T2, E2>, T2, E2> : public CallBackHolderBase<T, E, Future<T2, E2>, T2, E2> {
 public:
  using CallBackHolderBase<T, E, Future<T2, E2>, T2, E2>::CallBackHolderBase;

  /*!
   * \brief   Function to execute the registered call back.
   * \details Sets the new promise.
   */
  void ExecuteCallBack() override {
    Future<T2, E2> chain_future = this->callback_handler_(std::move(this->future_));
    static_cast<void>(chain_future.then([&](Future<T2, E2> unwrapped_future) {
      Result<T2, E2> res = unwrapped_future.GetResult();
      SetValueOrError(*(this->new_promise_), res);
    }));
  }
};

/* VECTOR Next Construct AutosarC++17_10-A12.8.3: MD_VAC_A12.8.3_dontReadAccessAMovedFromObject */
/*!
 * \brief  Specialization callback holder class to perform the callback execution for when U is Result<T2, E2>.
 * \tparam T is type for the calling future.
 * \tparam E is the error type for the calling Future.
 * \tparam T2 The unwrapped value type returned back from the registered call back function.
 * \tparam E2 The unwrapped error type returned back from the registered call back function.
 * \vprivate
 */
template <typename T, typename E, typename T2, typename E2>
class CallBackHolder<T, E, Result<T2, E2>, T2, E2> : public CallBackHolderBase<T, E, Result<T2, E2>, T2, E2> {
 public:
  using CallBackHolderBase<T, E, Result<T2, E2>, T2, E2>::CallBackHolderBase;

  /*!
   * \brief   Function to execute the registered call back.
   * \details Sets the new promise.
   */
  void ExecuteCallBack() override {
    Result<T2, E2> ret_val{this->callback_handler_(std::move(this->future_))};
    SetValueOrError(*(this->new_promise_), ret_val);
  }
};

/*!
 * \brief   Future continuation class accessible to the Future and Promise.
 * \details Promise creates object of FutureContinuation and passes it to the Future to be able to store and access the
 *          callback function.
 * \tparam  T The type for the calling Future.
 * \tparam  E The error type for the calling Future.
 * \vprivate
 */
template <typename T, typename E = ErrorCode>
class FutureContinuation final {
  // TODO(STORY-12266) Analyze the need for FutureContinuation
 public:
  /*!
   * \brief Default constructor.
   */
  FutureContinuation() = default;

  /*!
   * \brief Move constructor.
   */
  FutureContinuation(FutureContinuation&& other_object) noexcept = default;

  /*!
   * \brief Move assignment operator.
   */
  FutureContinuation& operator=(FutureContinuation&& other_object) & noexcept = default;

  /*!
   * \brief Destructor.
   */
  ~FutureContinuation() noexcept = default;

  /*!
   * \brief Default copy constructor deleted.
   */
  FutureContinuation(FutureContinuation const&) = delete;

  /*!
   * \brief Default copy assignment operator deleted.
   */
  FutureContinuation& operator=(FutureContinuation const&) = delete;

  /*!
   * \brief  Registers a callback handler to be called when the Promise is set and the state is ready.
   * \param  handler A callback handler.
   * \param  fut The calling Future.
   * \tparam Func The type for the callback function.
   * \return new future.
   */
  template <typename Func>
  auto SetCallBackHandler(Func handler, ara::core::Future<T, E>&& fut) ->
      typename TypeUnwrapping<decltype(handler(std::move(fut))), E>::type {
    /*! \brief Alias for the return type of the callable */
    using U = decltype(handler(std::move(fut)));
    using T2 = typename TypeUnwrapping<U, E>::value_type;
    using E2 = typename TypeUnwrapping<U, E>::error_type;

    ara::core::Promise<T2, E2> new_promise;
    ara::core::Future<T2, E2> new_future{new_promise.get_future()};
    call_back_ = std::make_unique<CallBackHolder<T, E, U, T2, E2>>(std::move(new_promise), std::move(fut), handler);
    return new_future;
  }

  /*!
   * \brief Executes the callback.
   */
  void ExecuteCallBack() {
    if (IsCallBackSet() && call_back_->IsExecutable()) {
      call_back_->ExecuteCallBack();
    }
  }

  /*!
   * \brief  Returns the registered state of the call back.
   * \return True if the callback is registered, false otherwise.
   */
  bool IsCallBackSet() noexcept { return (call_back_ != nullptr); }

 private:
  /*!
   * \brief Pointer to CallBackHolderInterface.
   */
  std::unique_ptr<CallBackHolderInterface> call_back_;
};

}  // namespace internal
}  // namespace core
}  // namespace ara

#endif  // LIB_VAC_INCLUDE_ARA_CORE_INTERNAL_FUTURE_CONTINUATION_H_
