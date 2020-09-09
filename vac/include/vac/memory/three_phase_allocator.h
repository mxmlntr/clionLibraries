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
/*!        \file  three_phase_allocator.h
 *        \brief  Test whether memory allocations/deallocations are functional in the phase.
 *
 *      \details  Three Phase Allocator manage three different allocation states, allocation,
 *                static phase and de-allocation phase. The de-allocation is not allowed in
 *                the allocation phase. Neither allocation nor de-allocation is allowed in the
 *                static phase. The allocation is not allowed in the de-allocation phase.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_MEMORY_THREE_PHASE_ALLOCATOR_H_
#define LIB_VAC_INCLUDE_VAC_MEMORY_THREE_PHASE_ALLOCATOR_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <algorithm>
#include <atomic>
#include <cassert>
#include <memory>
#include <mutex>
#include <type_traits>
#include <utility>

#include "vac/language/throw_or_terminate.h"

namespace vac {

namespace testing {
class AllocationPhaseManagerTestAdapter;
}  // namespace testing

namespace memory {

/*!
 * \brief Singleton class to manage the allocation phase during program execution.
 */
class AllocationPhaseManager {
 public:
  /*!
   * \brief Possible allocation phases.
   */
  enum class AllocationPhase : std::uint8_t { allocation = 0, steady = 1, deallocation = 2 };

  /*!
   * \brief Initialize a new AllocationPhaseManager or get access to the existing instance.
   */
  static AllocationPhaseManager& GetInstance();

  /*!
   * \brief   Switch to a new AllocationPhase.
   * \details The switch is only performed if the new AllocationPhase is larger than the old AllocationPhase.
   * \param   requested_allocation_phase The new allocation phase to switch to.
   */
  void SetPhase(AllocationPhase requested_allocation_phase) {
    // Obtain the original value
    AllocationPhase current_phase{allocation_phase_};

    // Loop to deal with parallel writes to the allocation phase.
    // The loop will terminate when current_phase is greater or equal the requested allocation_phase.
    // Iterations of the loop are limited: In every iteration, either the update succeeds and the loop terminates.
    // Or the current_phase will increase by at least 1. As the number of enum values in AllocationPhase is limited, the
    // loop will eventually terminate.
    while (requested_allocation_phase > current_phase) {
      static_cast<void>(allocation_phase_.compare_exchange_strong(current_phase, requested_allocation_phase));
      current_phase = allocation_phase_;
    }
  }

  /*!
   * \brief  Test whether allocations are allowed in the current phase.
   * \return True in case allocations are allowed. False otherwise.
   */
  bool IsAllocationAllowed() const { return allocation_phase_ == AllocationPhase::allocation; }

  /*!
   * \brief  Test whether deallocations are allowed in the current phase.
   * \return True in case deallocations are allowed. False otherwise.
   */
  bool IsDeallocationAllowed() const { return allocation_phase_ == AllocationPhase::deallocation; }

  /*!
   * \brief Return the current allocation phase.
   */
  AllocationPhase GetPhase() const { return allocation_phase_; }

 private:
  /*!
   * \brief Constructor.
   */
  AllocationPhaseManager() : allocation_phase_(AllocationPhase::allocation) {}

  /*!
   * \brief the current allocation phase.
   */
  std::atomic<AllocationPhase> allocation_phase_;

  /* VECTOR Next Line AutosarC++17_10-A11.3.1: MD_VAC_A11.3.1_doNotUseFriend */
  friend class AllocationPhaseManagerFixture;

  /* VECTOR Next Line AutosarC++17_10-A11.3.1: MD_VAC_A11.3.1_doNotUseFriend */
  friend class vac::testing::AllocationPhaseManagerTestAdapter;
};

/*!
 * \brief Class that only allocates and logs messages on deallocation but does not actually deallocate.
 *        This allocator class is intended for use with STL containers. It should satisfy their allocation needs and on
 *        the other hand notify us when resources are deallocated.
 * \trace CREQ-158629
 */
template <typename T, typename DelegateAllocator = std::allocator<T>>
class ThreePhaseAllocator {
 public:
  /*!
   * \brief Type of the delegate allocator used for actual allocations.
   */
  using DelegateAllocatorType = DelegateAllocator;

  /*!
   * \brief Pointer type of the delegate allocator.
   */
  using pointer = typename DelegateAllocator::pointer;

  /*!
   * \brief Const pointer type of the delegate allocator.
   */
  using const_pointer = typename DelegateAllocatorType::const_pointer;

  /*!
   * \brief Reference type of the delegate allocator.
   */
  using reference = typename DelegateAllocatorType::reference;

  /*!
   * \brief Const reference type of the delegate allocator.
   */
  using const_reference = typename DelegateAllocatorType::const_reference;

  /*!
   * \brief Value type of the delegate allocator.
   */
  using value_type = typename DelegateAllocator::value_type;

  /*!
   * \brief Size type of the delegate allocator.
   */
  using size_type = typename DelegateAllocator::size_type;

  /*!
   * \brief Difference type of the delegate allocator.
   */
  using difference_type = typename DelegateAllocator::difference_type;

  /*!
   * \brief default Constructor.
   */
  ThreePhaseAllocator() = default;

  /* VECTOR Next Construct AutosarC++17_10-A12.8.6: MD_VAC_A12.8.6_definedDeletedInBassClass */
  /*!
   * \brief Copy constructor.
   */

  ThreePhaseAllocator(ThreePhaseAllocator const&) = default;

  /* VECTOR Next Construct AutosarC++17_10-A12.8.6: MD_VAC_A12.8.6_definedDeletedInBassClass */
  /*!
   * \brief default Copy assignment operator.
   */
  ThreePhaseAllocator& operator=(ThreePhaseAllocator const&) & = default;

  /* VECTOR Next Construct AutosarC++17_10-A12.8.6: MD_VAC_A12.8.6_definedDeletedInBassClass */
  /*!
   * \brief Move constructor.
   */
  ThreePhaseAllocator(ThreePhaseAllocator&&) noexcept(std::is_nothrow_move_constructible<DelegateAllocator>::value) =
      default;

  /*!
   * \brief Copy constructor for rebinding.
   * \param other The other allocate to initialize from.
   */
  template <typename U>
  explicit ThreePhaseAllocator(U const& other) noexcept(false) : delegate_allocator_(other.GetDelegate()) {}

  /*!
   * \brief virtual default destructor.
   */
  virtual ~ThreePhaseAllocator() noexcept = default;

  /* VECTOR Next Construct AutosarC++17_10-A12.8.6: MD_VAC_A12.8.6_definedDeletedInBassClass */
  /*!
   * \brief Move assignment.
   */
  ThreePhaseAllocator& operator=(ThreePhaseAllocator&&) &
      noexcept(std::is_nothrow_move_assignable<DelegateAllocator>::value) = default;

  /*!
   * \brief  Equality operator for allocator. Delegates to the delegate_allocator_.
   * \param  other The other allocator to compare to.
   * \return The result of DelegateAllocator::operator==(other).
   */
  template <typename OtherAllocatorType>
  bool operator==(OtherAllocatorType const& other) const noexcept {
    return delegate_allocator_ == other;
  }

  /*!
   * \brief  Equality operator overload for cases where other is also a ThreePhaseAllocator.
   *         Unwraps the delegate_allocator_ from other and delegates to both delegate_allocator_s.
   * \param  other The other allocator to compare to.
   * \return The result of DelegateAllocator::operator==(other).GetDelegate().
   */
  bool operator==(const ThreePhaseAllocator<T, DelegateAllocatorType>& other) const {
    return delegate_allocator_ == other.GetDelegate();
  }

  /*!
   * \brief  Inequality operator. Delegates to !(operator==).
   * \param  other The other allocator to compare to.
   * \return The result of !(this->operator==(other)).
   */
  template <typename OtherAllocatorType>
  bool operator!=(OtherAllocatorType const& other) const noexcept {
    return !(*this == other);
  }

  /*!
   * \brief Rebind struct to adapt this allocator to a different type.
   */
  template <typename U>
  class rebind {
   public:
    /*!
     * \brief Rebind member to adapt this allocator to a different type.
     */
    using other = ThreePhaseAllocator<U, typename DelegateAllocatorType::template rebind<U>::other>;
  };

  /*!
   * \brief  Allocates a block of memory. Simply forwards to the delegate allocator.
   * \param  n The number of elements to allocate.
   * \return A pointer to the allocated (uninitialized) memory.
   * \throws std::runtime_error in case allocations are currently forbidden.
   */
  pointer allocate(std::size_t n) {
    if (!(GetAllocationPhaseManager().IsAllocationAllowed())) {
      vac::language::ThrowOrTerminate<std::runtime_error>("Allocation in wrong phase");
    }
    return delegate_allocator_.allocate(n);
  }

  /*!
   * \brief  Deallocates a block of memory. Simply forwards to the delegate allocator.
   * \param  ptr The memory location to deallocate from.
   * \param  n The number of elements to deallocate.
   * \throws std::runtime_error in case deallocations are currently forbidden.
   */
  void deallocate(pointer ptr, std::size_t n) {
    if (GetAllocationPhaseManager().IsDeallocationAllowed()) {
      delegate_allocator_.deallocate(ptr, n);
    } else {
      vac::language::ThrowOrTerminate<std::runtime_error>("Deallocation in wrong phase");
    }
  }

  /* VECTOR Next Construct AutosarC++17_10-A13.3.1: MD_VAC_A13.3.1_forwardingFunctionsShallNotBeOverloaded */
  /*!
   * \brief   Construct an object in the given memory location.
   * \details Constructs an object at the memory location pointed by p.
   * \remarks Construct does NOT allocate space for the element.
   *          It should already be available at p (see member allocate to allocate space).
   * \param   p The memory location to construct at.
   * \param   val The value to construct with.
   */
  void construct(pointer p, const_reference val) { delegate_allocator_.construct(p, val); }

  /*!
   * \brief   Construct an object in the given memory location.
   * \details Constructs an object at the memory location pointed by p.
   * \remarks Construct does NOT allocate space for the element.
   *          It should already be available at p (see member allocate to allocate space).
   * \param   p The memory location to construct at.
   * \param   args Arguments to be forwarded to the constructor.
   */
  template <typename U, typename... Args>
  void construct(U* p, Args&&... args) noexcept(false) {
    delegate_allocator_.construct(p, std::forward<Args>(args)...);
  }

  /* VECTOR Next Construct AutosarC++17_10-M0.1.8: MD_VAC_M0.1.8_voidFunctionHasNoExternalSideEffect */
  /*!
   * \brief   Destroy the object at the given memory location.
   * \details Destroys (in-place) the object at the memory location pointed to by p.
   * \remarks Destroy does NOT deallocate the storage for the element
   *          (see member deallocate to release storage space).
   * \param   p The memory location to destroy at.
   */
  void destroy(pointer p) { delegate_allocator_.destroy(p); }

  /*!
   * \brief Return the delegate allocator implementation.
   */
  DelegateAllocator const& GetDelegate() const { return delegate_allocator_; }

 private:
  /*!
   * \brief delegate allocator implementation.
   */
  DelegateAllocator delegate_allocator_;

  /*!
   * \brief Gets a reference to the stored AllocationPhaseManager.
   */
  AllocationPhaseManager const& GetAllocationPhaseManager() const { return AllocationPhaseManager::GetInstance(); }
};

}  // namespace memory
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_MEMORY_THREE_PHASE_ALLOCATOR_H_
