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
/*!        \file  buffer_provider.h
 *        \brief  BufferProvider class that return a pointer to a free buffer of type T.
 *
 *      \details  Currently, this class supports only one initial allocation of memory.
 *                All memory is allocated immediately.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_MEMORY_BUFFER_PROVIDER_H_
#define LIB_VAC_INCLUDE_VAC_MEMORY_BUFFER_PROVIDER_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <algorithm>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <utility>

#include "vac/container/static_map.h"
#include "vac/language/throw_or_terminate.h"
#include "vac/testing/test_adapter.h"

namespace vac {
namespace memory {
/*!
 * \brief BufferProvider Class to manage buffers of objects of a specific type.
 * \trace CREQ-158631
 */
template <class T>
class BufferProvider final {
  FRIEND_TEST(BufferProvider, Capacity);
  FRIEND_TEST(BufferProvider, AllocateOnlyOnce);

 public:
  /*!
   * \brief Typedef for raw pointers.
   */
  using pointer = T*;

  /*!
   * \brief Typedef for the size type used in this implementation.
   */
  using size_type = std::size_t;

  /*!
   * \brief Constructor.
   */
  BufferProvider() {}

  /*!
   * \brief Default copy constructor deleted.
   */
  BufferProvider(const BufferProvider&) = delete;
  /*!
   * \brief  Default copy assignment operator deleted.
   */
  BufferProvider& operator=(const BufferProvider&) = delete;
  /*!
   * \brief Default move constructor deleted.
   */
  BufferProvider(BufferProvider&&) = delete;
  /*!
   * \brief  Default copy assignment operator deleted.
   */
  BufferProvider operator=(BufferProvider&&) = delete;

  /*!
   * \brief Default destructor.
   */
  ~BufferProvider() = default;  // TODO(vsarcbosu): The execption from destructor should be fixed.

  /*!
   * \brief  Update the memory allocation.
   *         The current implementation only allows a single allocation. All further reservations will only limit the
   *         visible memory. All subsequent calls to reserve() where new_capacity is greater than the initial
   *         new_capacity will be rejected with a bad_alloc.
   * \param  number_buffer Number of buffers.
   * \param  number_elements Number of elements inside each buffer.
   * \throws std::logic_error if buffer_storage_ is already initialized.
   * \trace  CREQ-158632
   */
  void reserve(size_type number_buffer, size_type number_elements) {
    std::lock_guard<std::mutex> lock{buffer_mutex_};
    if ((number_buffer * number_elements) > (reserved_number_buffer_ * reserved_number_elements_)) {
      // We need to allocate additional memory. Current implementation can only allocate once initially.
      if (buffer_storage_) {
        vac::language::ThrowOrTerminate<std::logic_error>("Reallocation not implemented");
      } else {
        /* VECTOR Next Construct AutosarC++17_10-A18.1.1: MD_VAC_A18.1.1_cStyleArraysShouldNotBeUsed */
        buffer_storage_ = std::make_unique<T[]>(number_buffer * number_elements);
        reserved_number_elements_ = number_elements;
        reserved_number_buffer_ = number_buffer;
        free_buffer_map_.reserve(number_buffer);
        // Associate each available buffer with a boolean.
        for (size_type i{0}; i < reserved_number_buffer_; ++i) {
          pointer next{std::next(buffer_storage_.get(), static_cast<std::ptrdiff_t>(i) *
                                                            static_cast<std::ptrdiff_t>(reserved_number_elements_))};
          static_cast<void>(free_buffer_map_.emplace(next, true));
        }
      }
    }
  }

  /*!
   * \brief  Return a pointer to a free Buffer big enough for number_elements. Otherwise return a null pointer.
   * \param  number_elements Number of elements needed inside the buffer.
   * \return Raw pointer to a free buffer.
   */
  pointer allocate(size_type number_elements) {
    pointer ret_value{nullptr};
    std::lock_guard<std::mutex> lock{buffer_mutex_};
    if (number_elements <= reserved_number_elements_) {
      typename FreeBufferMap::iterator it{std::find_if(free_buffer_map_.begin(), free_buffer_map_.end(),
                                                       [](PairRawPtrBool const pair) { return pair.second; })};
      if (it != free_buffer_map_.end()) {
        it->second = false;
        ret_value = it->first;
      }
    }
    return ret_value;
  }

  /*!
   * \brief  Release a buffer.
   * \param  ptr Pointer to the buffer to be released.
   * \throws std::logic_error if trying to deallocate a nullptr.
   * \throws std::bad_alloc if \a ptr does not exist in free_buffer_map_.
   */
  void deallocate(pointer ptr) {
    std::lock_guard<std::mutex> lock{buffer_mutex_};
    if (ptr == nullptr) {
      vac::language::ThrowOrTerminate<std::logic_error>("Attempting to deallocate a nullptr");
    } else {
      typename FreeBufferMap::iterator it{free_buffer_map_.find(ptr)};
      if (it == free_buffer_map_.end()) {
        vac::language::ThrowOrTerminate<std::bad_alloc>();
      } else {
        it->second = true;
      }
    }
  }

 private:
  /* VECTOR Next Construct AutosarC++17_10-A18.1.1: MD_VAC_A18.1.1_cStyleArraysShouldNotBeUsed */
  /*!
   * \brief Typedef for storage unique pointers.
   */
  using StorageUniquePtr = std::unique_ptr<T[]>;

  /*!
   * \brief Type definition for a pair of raw_pointer and boolean.
   */
  using PairRawPtrBool = std::pair<pointer, bool>;

  /*!
   * \brief Type definition for map associating a buffer with a boolean to inform if a buffer is already used.
   */
  using FreeBufferMap =
      vac::container::StaticMap<typename PairRawPtrBool::first_type, typename PairRawPtrBool::second_type>;

  /*!
   * \brief The amount of elements reserved for one buffer.
   */
  size_type reserved_number_elements_{0};

  /*!
   * \brief Number of buffers currently reserved.
   */
  size_type reserved_number_buffer_{0};

  /*!
   * \brief Unique pointer to the allocated memory.
   */
  StorageUniquePtr buffer_storage_{nullptr};

  /*!
   * \brief FreeBufferMap.
   */
  FreeBufferMap free_buffer_map_{};

  /*!
   * \brief Mutex to synchronize access to the buffers.
   */
  std::mutex buffer_mutex_{};
};

/*!
 * \brief Implement smart buffer provider.
 * \trace CREQ-161250
 */
template <class T>
class SmartBufferProvider final {
 public:
  /*!
   * \brief Typedef for the size type used in this implementation.
   */
  using size_type = std::size_t;

  /*!
   * \brief Default constructor.
   */
  SmartBufferProvider() = default;
  /*!
   * \brief Default copy constructor deleted.
   */
  SmartBufferProvider(const SmartBufferProvider&) = delete;
  /*!
   * \brief  Default copy assignment operator deleted.
   */
  SmartBufferProvider& operator=(const SmartBufferProvider&) = delete;
  /*!
   * \brief Default move constructor deleted.
   */
  SmartBufferProvider(SmartBufferProvider&&) = delete;
  /*!
   * \brief  Default move assignment operator deleted.
   */
  SmartBufferProvider& operator=(SmartBufferProvider&&) = delete;

  /*!
   * \brief Default destructor.
   */
  ~SmartBufferProvider() = default;  // TODO(vsarcbosu): The execption from destructor should be fixed.

  /*!
   * \brief Deleter for unique pointers to buffers.
   */
  class SmartBufferProviderDeleter final {
   public:
    /*!
     * \brief Constructor.
     */
    SmartBufferProviderDeleter() : SmartBufferProviderDeleter(nullptr) {}

    /*!
     * \brief Overloaded constructor.
     * \param buffer_provider The buffer provider whose pointers may be.
     */
    explicit SmartBufferProviderDeleter(BufferProvider<T>* buffer_provider) : buffer_provider_(buffer_provider) {}

    /*!
     * \brief Copy constructor.
     */
    SmartBufferProviderDeleter(SmartBufferProviderDeleter const&) = default;

    /*!
     * \brief  Copy assignment.
     * \return A reference to the assigned-to object.
     */
    SmartBufferProviderDeleter& operator=(SmartBufferProviderDeleter const&) & = default;

    /*!
     * \brief Move constructor.
     */
    SmartBufferProviderDeleter(SmartBufferProviderDeleter&&) noexcept = default;

    /*!
     * \brief  Move assignment.
     * \return A reference to the assigned-to object.
     */
    SmartBufferProviderDeleter& operator=(SmartBufferProviderDeleter&&) & noexcept = default;

    /*!
     * \brief Default destructor.
     */
    ~SmartBufferProviderDeleter() noexcept = default;

    /*!
     * \brief The actual deleter function.
     * \param ptr The pointer whose memory shall be deallocated.
     */
    void operator()(typename BufferProvider<T>::pointer ptr) {
      if (buffer_provider_ != nullptr) {
        buffer_provider_->deallocate(ptr);
      }
    }

   private:
    /*!
     * \brief Pointer to the buffer provider.
     */
    BufferProvider<T>* buffer_provider_;
  };

  /*!
   * \brief Type definition for a unique pointer.
   */
  using UniqueBufferPtr = std::unique_ptr<T, SmartBufferProviderDeleter>;

  /*!
   * \brief Update the memory allocation.
   *        The current implementation only allows a single allocation. All further reservations will only limit the
   *        visible memory. All subsequent calls to reserve() where new_capacity is greater than the initial
   *        new_capacity will be rejected with a bad_alloc.
   *        \param number_buffer Number of buffers.
   *        \param size_buffer Size of each buffers.
   */
  void reserve(size_type number_buffer, size_type size_buffer) { buffer_provider_.reserve(number_buffer, size_buffer); }

  /* VECTOR Next Construct AutosarC++17_10-A18.5.8: MD_VAC_A18.5.8_localObjectsShallBeAllocatedOnStack */
  /*!
   * \brief  Return a pointer to a free Buffer big enough for number_elements. Otherwise return a null pointer.
   * \param  number_elements Number of elements needed inside the buffer.
   * \return Raw pointer to a free buffer.
   */
  UniqueBufferPtr allocate(size_type number_elements) {
    typename BufferProvider<T>::pointer buffer;
    buffer = buffer_provider_.allocate(number_elements);
    UniqueBufferPtr unique_ptr{buffer, SmartBufferProviderDeleter(&buffer_provider_)};
    return unique_ptr;
  }

  /*!
   * \brief  Return the buffer provider.
   * \return Reference to the buffer provider.
   */
  BufferProvider<T>& GetBufferProvider() { return buffer_provider_; }

 private:
  /*!
   * \brief Buffer Provider.
   */
  BufferProvider<T> buffer_provider_;
};

}  // namespace memory
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_MEMORY_BUFFER_PROVIDER_H_
