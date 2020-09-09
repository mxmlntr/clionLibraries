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
/*!        \file  static_vector.h
 *        \brief  Static vector manages elements in contiguous memory.
 *
 *      \details  A contiguous memory is allocated from the allocator StaticVector::reserve().
 *                The memory segment will be deallocated when the object is destroyed.
 *                Besides, the elements in the vector can be removed or added.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_CONTAINER_STATIC_VECTOR_H_
#define LIB_VAC_INCLUDE_VAC_CONTAINER_STATIC_VECTOR_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <memory>
#include <utility>

#include "ara/core/span.h"
#include "vac/language/throw_or_terminate.h"

namespace vac {
namespace container {

/*!
 * \brief Class to (owning) manage a contiguous memory segment.
 *        The size of the memory segment can be set once. The memory segment is allocated when calling
 *        StaticVector::reserve() and is deallocated when the object is destroyed. Allocations/Deallocations are
 *        performed from the given allocator.
 * \trace CREQ-158594, CREQ-158595
 */
template <typename T, typename alloc = vac::memory::PhaseManagedAllocator<T>>
class StaticVector final {
 public:
  /*!
   * \brief Typedef for the ara::core::Span used internally.
   */
  using span_type = ara::core::Span<T>;

  /*!
   * \brief Typedef for the size value.
   */
  using size_type = typename span_type::size_type;

  /*!
   * \brief Typedef for the contained element.
   */
  using value_type = typename span_type::value_type;

  /*!
   * \brief Typedef for a pointer.
   */
  using pointer = typename span_type::pointer;

  /*!
   * \brief Typedef for a const pointer.
   */
  using const_pointer = const T*;

  /*!
   * \brief Typedef for a reference.
   */
  using reference = typename span_type::reference;

  /*!
   * \brief Typedef for a const reference.
   */
  using const_reference = const T&;

  /*!
   * \brief Typedef for the allocator used.
   */
  using allocator_type = alloc;

  /*!
   * \brief Typedef for the allocator type used after rebinding.
   */
  using actual_allocator_type = typename allocator_type::template rebind<T>::other;

  /*!
   * \brief Typedef for an iterator.
   * \trace CREQ-160857
   */
  using iterator = typename span_type::iterator;

  /*!
   * \brief Typedef for a const iterator.
   */
  using const_iterator = typename span_type::const_iterator;

  /*!
   * \brief Initialize a StaticVector.
   * \param allocator The allocator to use for initialization, default is allocator_type().
   */
  explicit StaticVector(const allocator_type& allocator = allocator_type())
      : delegate_(), allocator_(allocator), max_num_elements_(0) {}

  /*!
   * \brief Default copy constructor deleted.
   */
  StaticVector(const StaticVector&) = delete;
  /*!
   * \brief  Default copy assignment operator deleted.
   */
  StaticVector& operator=(const StaticVector&) = delete;

  /*!
   * \brief Move constructor.
   * \param other The vector from which to construct the new vector.
   */
  StaticVector(StaticVector&& other) : StaticVector() { this->swap(other); }

  /*!
   * \brief  Move Assignment.
   * \param  other The vector from which to construct the new vector.
   * \return A reference to the assigned-to object.
   */
  StaticVector& operator=(StaticVector&& other) {
    this->swap(other);
    return *this;
  }

  /*!
   * \brief Destructor.
   */
  ~StaticVector() {
    shorten(0);
    if (delegate_.data() != nullptr) {
      allocator_.deallocate(delegate_.data(), max_num_elements_);
    }
  }

  /*!
   * \brief  Allocate a memory segment from the allocator.
   * \param  num_elements The number of elements of T to allocate.
   * \throws std::runtime_error If reserve() was called before.
   * \trace  CREQ-158592
   */
  void reserve(size_type num_elements) {
    if (delegate_.data() != nullptr) {
      vac::language::ThrowOrTerminate<std::runtime_error>("StaticVector has already been reserved");
    }
    max_num_elements_ = num_elements;
    delegate_ = span_type(allocator_.allocate(num_elements), static_cast<std::size_t>(0));
  }

  /*!
   * \brief Set the vector to be of a certain size.
   * \param num_elements The number of elements of T to allocate.
   *        Note: currently, resize() implicitly calls reserve() if it increases the size.
   */
  void resize(size_type num_elements) {
    if (num_elements > max_num_elements_) {
      reserve(num_elements);
    }

    // Resize the delegate if resizing is needed.
    if (num_elements > delegate_.size()) {
      // Record previous size of the container.
      size_type const old_num_elements{delegate_.size()};

      // Extend the delegate.
      delegate_ = span_type(delegate_.data(), num_elements);

      // Cache the end iterator. We do not modify the size of the vector here
      // and during profiling, the end() operation turned out to be fairly
      // costly for long vectors.
      iterator const end_it{end()};

      // Initialize new elements.
      for (iterator it{std::next(begin(), static_cast<std::ptrdiff_t>(old_num_elements))}; it != end_it; ++it) {
        allocator_.construct(it);
      }
    } else {
      // Destroy excess elements.
      shorten(num_elements);
    }
  }

  /*!
   * \brief Shorten the vector to the given size.
   * \param num_elements The number elements of T the vector shall be shortened to.
   * \note  Does nothing if num_elements is not smaller than size().
   */
  void shorten(size_type num_elements) {
    // will this #elements actually shorten?
    if (num_elements < delegate_.size()) {
      // Cache the end iterator. We do not modify the size of the vector here
      // and during profiling, the end() operation turned out to be fairly
      // costly for long vectors.
      iterator const end_it{end()};

      for (iterator it{std::next(begin(), static_cast<std::ptrdiff_t>(num_elements))}; it != end_it; ++it) {
        allocator_.destroy(it);
      }

      delegate_ = span_type(delegate_.data(), num_elements);
    }
  }

  /*!
   * \brief Swap the contents of two StaticVector objects including their allocator.
   * \param other The second vector whose contents are swapped.
   */
  void swap(StaticVector& other) noexcept {
    std::swap(delegate_, other.delegate_);
    std::swap(allocator_, other.allocator_);
    std::swap(max_num_elements_, other.max_num_elements_);
  }

  /*!
   * \brief  Get the length of the contained array.
   * \return The number of contained elements.
   */
  size_type size() const noexcept { return delegate_.size(); }

  /*!
   * \brief  Get the maximum number of elements this StaticVector can hold.
   * \return The number of elements this vector can store.
   */
  size_type capacity() const noexcept { return max_num_elements_; }

  /*!
   * \brief  Check whether the contained array is empty.
   * \return True if the vector is empty.
   */
  bool empty() const noexcept { return delegate_.empty(); }

  /*!
   * \brief  Get a pointer to the contained data.
   * \return A pointer to the data.
   */
  pointer data() noexcept { return delegate_.data(); }

  /*!
   * \brief  Get a const pointer to the contained data.
   * \return A pointer to the data.
   */
  const_pointer data() const noexcept { return delegate_.data(); }

  /*!
   * \brief  Iterator to the start of the array.
   * \return An iterator to the start of the vector.
   */
  iterator begin() { return delegate_.begin(); }
  /*!
   * \brief  Past-The-End iterator of the array.
   * \return An iterator to the sentinel value at the end.
   */
  iterator end() { return delegate_.end(); }

  /*!
   * \brief  Const iterator to the start of the array.
   * \return An const iterator to the start of the vector.
   */
  const_iterator begin() const { return delegate_.begin(); }
  /*!
   * \brief Const Past-The-End iterator of the array.
   */
  const_iterator end() const { return delegate_.end(); }

  /*!
   * \brief Const iterator to the start of the array.
   */
  const_iterator cbegin() const { return delegate_.cbegin(); }
  /*!
   * \brief  Const past-The-End iterator of the array.
   * \return An const iterator to the sentinel value at the end.
   */
  const_iterator cend() const { return delegate_.cend(); }

  /*!
   * \brief  Get the element at the specified position.
   * \param  pos Index of the element to get.
   * \return A reference to the resulting element.
   * \throws std::out_of_range If pos >= size().
   */
  reference at(size_type pos) { return delegate_[pos]; }

  /*!
   * \brief  Get the element at the specified position (const version).
   * \param  pos Index of the element to get.
   * \return A reference to the resulting element.
   * \throws std::out_of_range If pos >= size().
   */
  const_reference at(size_type pos) const { return delegate_[pos]; }

  /*!
   * \brief  Unchecked array access.
   *         The behavior is unspecified if the container is empty or if pos >= size().
   * \param  pos Index of the element to get.
   * \return A reference to the resulting element.
   */
  reference operator[](size_type pos) { return delegate_[pos]; }

  /*!
   * \brief  Unchecked array access.
   *         The behavior is unspecified if the container is empty or if pos >= size().
   * \param  pos Index of the element to get.
   * \return A reference to the resulting element.
   */
  const_reference operator[](size_type pos) const { return delegate_[pos]; }

  /*!
   * \brief  Unchecked access to first element.
   *         The behavior is unspecified if the container is empty.
   * \return A reference to the first element.
   */
  reference front() { return delegate_[0]; }

  /*!
   * \brief  Unchecked access to first element.
   *         The behavior is unspecified if the container is empty.
   * \return A reference to the first element.
   */
  const_reference front() const { return delegate_[0]; }

  /*!
   * \brief  Unchecked access to last element.
   *         The behavior is unspecified if the container is empty.
   * \return A reference to the last element.
   */
  reference back() { return delegate_[this->size() - 1]; }

  /*!
   * \brief  Unchecked access to last element.
   *         The behavior is unspecified if the container is empty.
   * \return A reference to the last element.
   */
  const_reference back() const { return delegate_[this->size() - 1]; }

  /*!
   * \brief  Add an element to the back of the StaticVector.
   * \param  elem The element to add.
   * \throws std::bad_alloc If the StaticVector is already full.
   */
  void push_back(const T& elem) {
    size_type const delegate_size{delegate_.size()};
    if (delegate_size >= max_num_elements_) {
      vac::language::ThrowOrTerminate<std::bad_alloc>();
    } else {
      delegate_ = span_type(delegate_.data(), delegate_size + 1);
      allocator_.construct(&delegate_[this->size() - 1], elem);
    }
  }

  /*!
   * \brief  Add an element to the back of the StaticVector.
   * \param  args Constructor arguments for T.
   * \throws std::bad_alloc If the StaticVector is already full.
   * \trace  CREQ-158593
   */
  template <typename... Args>
  void emplace_back(Args&&... args) {
    size_type const delegate_size{delegate_.size()};
    if (delegate_size >= max_num_elements_) {
      vac::language::ThrowOrTerminate<std::bad_alloc>();
    } else {
      delegate_ = span_type(delegate_.data(), delegate_size + 1);
      allocator_.construct(&delegate_[this->size() - 1], std::forward<Args>(args)...);
    }
  }

  /*!
   * \brief  Remove an element to the back of the StaticVector.
   * \throws std::out_of_range If the StaticVector is empty.
   */
  void pop_back() {
    size_type const delegate_size{delegate_.size()};
    if (delegate_size <= 0) {
      vac::language::ThrowOrTerminate<std::out_of_range>("StaticVector::pop_back: vector is empty");
    } else {
      reference elem{delegate_[this->size() - 1]};
      allocator_.destroy(&elem);
      delegate_ = span_type(delegate_.data(), delegate_size - 1);
    }
  }

  /*!
   * \brief  Remove an element anywhere in the static vector.
   * \param  elem Iterator to the element to be removed.
   * \return An iterator to the element following the removed element.
   * \trace  CREQ-158593
   */
  iterator erase(iterator elem) {
    if (elem != end()) {
      iterator current{elem};
      iterator next{std::next(elem, 1)};

      while (next != end()) {
        *current = std::move(*next);
        ++current;
        ++next;
      }

      pop_back();
    }

    return elem;
  }

  /*!
   * \brief Remove all elements from the vector.
   */
  void clear() { shorten(0); }

 private:
  /*!
   * \brief A Span to implement access to the data.
   *        The Span is also used to hold the pointer to the owned data. Ownership semantics are implemented by the
   *        StaticVector itself.
   *        The size of the delegate_ represents the current length of the vector, not the amount of memory allocated.
   */
  span_type delegate_;

  /*!
   * \brief Allocator used to obtain memory.
   */
  actual_allocator_type allocator_;

  /*!
   * \brief The maximum number of elements (i.e., amount of memory allocated) that this StaticVector can hold.
   */
  size_type max_num_elements_;
};

}  // namespace container
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_CONTAINER_STATIC_VECTOR_H_
