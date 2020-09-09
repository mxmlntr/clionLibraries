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
/*!        \file  timer.h
 *        \brief  Header file for the Timer class.
 *
 *      \details  An abstract timer, one-shot or periodic. Users should subclass Timer
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_TIMER_TIMER_H_
#define LIB_VAC_INCLUDE_VAC_TIMER_TIMER_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <sys/time.h>
#include <chrono>

#include "vac/timer/timer_manager.h"

namespace vac {
namespace timer {

/*!
 * \brief Base Class representing a timer.
 *        Timers can be periodic or one-shot. Create a subclass of Timer to implement a time-based callback.
 * \trace CREQ-158642, CREQ-158643
 */
class Timer {
 public:
  /*!
   * \brief Typedef representing the clock type this timer operates on.
   *        We use steady_clock to avoid forward and backward leaps, causing unwanted expiration by suspend-to-RAM or by
   *        system clock adjustment. Meets the need to use the highest resolution clock on QNX, because
   *        high_resolution_clock resolves to steady_clock, there.
   */
  using Clock = std::chrono::steady_clock;

  /*!
   * \brief Constructor taking a timer_manager this Timer is associated with.
   */
  explicit Timer(TimerManager* timer_manager);

  Timer(Timer const&) = delete;
  Timer& operator=(Timer const&) & = delete;
  Timer(Timer&&) = delete;
  Timer& operator=(Timer&&) & = delete;

  /*!
   * \brief Destructor.
   */
  virtual ~Timer();

  /*!
   * \brief  Returns the time_point at which this timer expires next.
   *         Note that the value is only valid if the Timer is started.
   * \return The next_expiry_ point.
   */
  Clock::time_point const& GetNextExpiry() const { return next_expiry_; }

  /*!
   * \brief Sets the timer to periodic mode with the given period.
   *        The running state of the timer is not modified.
   * \param period The period at which this timer should fire.
   * \trace CREQ-158643
   */
  void SetPeriod(Clock::duration period);

  /*!
   * \brief Sets the timer to one-shot mode with the given expiry point.
   *        The running state of the timer is not modified.
   * \param time_point The Time Point at which this timer should fire.
   * \trace CREQ-158642
   */
  void SetOneShot(Clock::time_point time_point);

  /*!
   * \brief Sets the timer to one-shot mode with an expiry point relative to the current time.
   *        The running state of the timer is not modified.
   * \param timeout The Time until this timer should fire.
   * \trace CREQ-158642
   */
  void SetOneShot(Clock::duration timeout);

  /*!
   * \brief   Starts the timer.
   * \details If the timer has already been scheduled, it will be rescheduled.
   * \trace   CREQ-158642, CREQ-158643
   */
  void Start();

  /*!
   * \brief Stops the timer.
   * \trace CREQ-158642, CREQ-158643
   */
  void Stop();

  /*!
   * \brief   Callback that is activated when the timer expires.
   *          This callback can indicate whether the timer should be scheduled again.
   * \details In a multithreaded context, the callback must not start another thread
   *          that can call timer manager APIs before this callback completes.
   * \return  True if the timer shall be scheduled again, false otherwise. Returning true has no effect when the timer
   * is not periodic.
   */
  virtual bool HandleTimer() = 0;

  /*!
   * \brief Handler method called by the TimerManager.
   */
  void DoHandleTimer();

  /*!
   * \brief  Returns whether the expiry point of this timer is greater than the current time.
   * \return True if the next_expiry_ point has passed, false otherwise.
   */
  bool IsExpired() const;

  /*!
   * \brief  Convert the next_expiry_ of this timer to a timeval struct.
   * \return The current next_expiry_ converted to a timeval relative to the current time.
   */
  struct timeval const ToTimeval() const;

 private:
  /*!
   * \brief Reschedules a periodic timer.
   */
  void RestartPeriodic();

  /*!
   * \brief Timer managing this timer.
   */
  TimerManager* timer_manager_;
  /*!
   * \brief Flag indicating this is a one time action.
   */
  bool one_shot_;
  /*!
   * \brief Interval for periodic timers.
   */
  Clock::duration period_;
  /*!
   * \brief Time for the next expiry.
   */
  Clock::time_point next_expiry_;
};

}  // namespace timer
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_TIMER_TIMER_H_
