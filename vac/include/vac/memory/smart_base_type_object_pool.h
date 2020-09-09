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
/*!        \file  object_pool.h
 *        \brief  ObjectPool class that can dynamically allocate a set of objects and then never deallocates them but
 *                recycles the memory for new objects.
 *
 *      \details  Currently, this class supports only one initial allocation of memory. All memory is allocated
 *                immediately, even if no objects are created.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_MEMORY_SMART_BASE_TYPE_OBJECT_POOL_H_
#define LIB_VAC_INCLUDE_VAC_MEMORY_SMART_BASE_TYPE_OBJECT_POOL_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <cassert>
#include <memory>
#include <utility>

#include "vac/memory/object_pool.h"

namespace vac {
namespace memory {

// Forward declaration
class DeleteableSmartPoolObject;

/*!
 * \brief Interface for the actual Deleter for a smart pool object.
 */
class SmartObjectPoolDeleterContext {
 public:
  /*!
   * \brief Default constructor.
   */
  SmartObjectPoolDeleterContext() = default;

  /* VECTOR Next Construct AutosarC++17_10-A12.8.6: MD_VAC_A12.8.6_definedDeletedInBassClass */
  /*!
   * \brief Default copy constructor.
   */
  SmartObjectPoolDeleterContext(SmartObjectPoolDeleterContext const&) = default;

  /* VECTOR Next Construct AutosarC++17_10-A12.8.6: MD_VAC_A12.8.6_definedDeletedInBassClass */
  /*!
   * \brief Default move constructor.
   */
  SmartObjectPoolDeleterContext(SmartObjectPoolDeleterContext&&) noexcept = default;

  /* VECTOR Next Construct AutosarC++17_10-A12.8.6: MD_VAC_A12.8.6_definedDeletedInBassClass */
  /*!
   * \brief Default copy assignment operator.
   */
  SmartObjectPoolDeleterContext& operator=(SmartObjectPoolDeleterContext const&) & = default;

  /* VECTOR Next Construct AutosarC++17_10-A12.8.6: MD_VAC_A12.8.6_definedDeletedInBassClass */
  /*!
   * \brief Default move assignment operator.
   */
  SmartObjectPoolDeleterContext& operator=(SmartObjectPoolDeleterContext&&) & noexcept = default;

  /*!
   * \brief Default virtual destructor.
   */
  virtual ~SmartObjectPoolDeleterContext() = default;

  /*!
   * \brief Actual Deleter function.
   * \param ptr Pointer to smart pool object.
   */
  virtual void destroy(DeleteableSmartPoolObject* ptr) = 0;

  /*!
   * \brief Actual Deleter function.
   * \param ptr Const Pointer to smart pool object.
   */
  virtual void destroy(const DeleteableSmartPoolObject* ptr) = 0;
};

/*!
 * \brief Proxy on the object side for deleting SmartPoolObjects.
 */
class DeleteableSmartPoolObject {
 public:
  /*!
   * \brief Constructor for SmartPoolObjects.
   * \param deleter_context DeleterContext for the current object.
   */
  explicit DeleteableSmartPoolObject(SmartObjectPoolDeleterContext* deleter_context)
      : deleter_context_(deleter_context) {}

  /* VECTOR Next Construct AutosarC++17_10-A12.8.6: MD_VAC_A12.8.6_definedDeletedInBassClass */
  /*!
   * \brief Default copy constructor.
   */
  DeleteableSmartPoolObject(DeleteableSmartPoolObject const&) = default;

  /* VECTOR Next Construct AutosarC++17_10-A12.8.6: MD_VAC_A12.8.6_definedDeletedInBassClass */
  /*!
   * \brief Default move constructor.
   */
  DeleteableSmartPoolObject(DeleteableSmartPoolObject&&) noexcept = default;

  /* VECTOR Next Construct AutosarC++17_10-A12.8.6: MD_VAC_A12.8.6_definedDeletedInBassClass */
  /*!
   * \brief Default copy assignment operator.
   */
  DeleteableSmartPoolObject& operator=(DeleteableSmartPoolObject const&) & = default;

  /* VECTOR Next Construct AutosarC++17_10-A12.8.6: MD_VAC_A12.8.6_definedDeletedInBassClass */
  /*!
   * \brief Default move assignment operator.
   */
  DeleteableSmartPoolObject& operator=(DeleteableSmartPoolObject&&) & noexcept = default;

  /*!
   * \brief Destructor.
   */
  virtual ~DeleteableSmartPoolObject() = default;

  /*!
   * \brief  Returns the DeleterContext for deleting the current object.
   * \return DeleterContext.
   */
  SmartObjectPoolDeleterContext* GetDeleterContext() const { return deleter_context_; }

 private:
  /*!
   * \brief Deleter context for a smart pool object.
   */
  SmartObjectPoolDeleterContext* deleter_context_;
};

namespace detail {
/*!
 * \brief Master Deleter class using the DeleterContext from the object to delete.
 */
class SmartBaseTypeObjectPoolDeleter final {
 public:
  /*!
   * \brief The actual deleter function for const SmartPoolObject.
   * \param ptr Pointer to object.
   */
  void operator()(DeleteableSmartPoolObject* ptr) {
    if (ptr != nullptr) {
      SmartObjectPoolDeleterContext* context{ptr->GetDeleterContext()};
      if (context != nullptr) {
        context->destroy(ptr);
      } else {
        /* VECTOR Next Line AutosarC++17_10-A18.5.2: MD_VAC_A18.5.2_libraryFunction */
        delete ptr;
      }
    }
  }

  /*!
   * \brief The actual deleter function for const SmartPoolObject.
   * \param ptr Pointer to object.
   */
  void operator()(const DeleteableSmartPoolObject* ptr) {
    if (ptr != nullptr) {
      SmartObjectPoolDeleterContext* context{ptr->GetDeleterContext()};
      if (context != nullptr) {
        context->destroy(ptr);
      } else {
        /* VECTOR Next Line AutosarC++17_10-A18.5.2: MD_VAC_A18.5.2_libraryFunction */
        delete ptr;
      }
    }
  }
};

/* VECTOR Next Construct AutosarC++17_10-M5.2.3: MD_VAC_M5.2.3_castFromPolymorphicBaseClassToDerivedClass */
/*!
 * \brief Deleter for unique_ptrs to ObjectPool-managed objects
 *        This class is only used internally in SmartBaseTypeObjectPool<T> as a deleter for std::unique_ptr<T>, using
 *        SmartBaseTypeObjectPoolDeleter as a delegate. In this context, it is safe to use static_cast<T*>.
 */
template <class T, typename alloc>
class SmartObjectPoolDeleterContextImpl : public SmartObjectPoolDeleterContext {
  static_assert(std::is_base_of<DeleteableSmartPoolObject, T>::value, "T must implement of DeleteableSmartPoolObject");

 public:
  /*!
   * \brief Default copy constructor.
   */
  SmartObjectPoolDeleterContextImpl(SmartObjectPoolDeleterContextImpl const&) = default;

  /*!
   * \brief Default move constructor.
   */
  SmartObjectPoolDeleterContextImpl(SmartObjectPoolDeleterContextImpl&&) noexcept = default;

  /* VECTOR Next Construct AutosarC++17_10-A10.2.1: MD_VAC_A10.2.1_redefinitionOfNonVirtualFunction */
  /*!
   * \brief Default copy assignment operator.
   */
  SmartObjectPoolDeleterContextImpl& operator=(SmartObjectPoolDeleterContextImpl const&) & = default;

  /* VECTOR Next Construct AutosarC++17_10-A10.2.1: MD_VAC_A10.2.1_redefinitionOfNonVirtualFunction */
  /*!
   * \brief Default move assignment operator.
   */
  SmartObjectPoolDeleterContextImpl& operator=(SmartObjectPoolDeleterContextImpl&&) & noexcept = default;
  /*!
   * \brief Default overridden destructor.
   */
  ~SmartObjectPoolDeleterContextImpl() noexcept override = default;

  /*!
   * \brief Constructor.
   * \param pool Object pool to return an object to on destruction.
   */
  explicit SmartObjectPoolDeleterContextImpl(ObjectPool<T, alloc>* pool) noexcept
      : SmartObjectPoolDeleterContext{}, pool_{pool} {}

  /*!
   * \copydoc SmartObjectPoolDeleterContext::destroy
   */
  void destroy(DeleteableSmartPoolObject* ptr) override {
    if (ptr != nullptr) {
      if (pool_ != nullptr) {
        pool_->destroy(static_cast<T*>(ptr));
      } else {
        /* VECTOR Next Line AutosarC++17_10-A18.5.2: MD_VAC_A18.5.2_libraryFunction */
        delete ptr;
      }
    }
  }

  /*!
   * \copydoc SmartObjectPoolDeleterContext::destroy
   */
  void destroy(const DeleteableSmartPoolObject* ptr) override {
    if (ptr != nullptr) {
      if (pool_ != nullptr) {
        pool_->destroy(static_cast<const T*>(ptr));
      } else {
        /* VECTOR Next Line AutosarC++17_10-A18.5.2: MD_VAC_A18.5.2_libraryFunction */
        delete ptr;
      }
    }
  }

 private:
  /*!
   * \brief The ObjectPool to return an object to on destruction.
   */
  ObjectPool<T, alloc>* pool_;
};
}  // namespace detail

/*!
 * \brief Smart Pointer type used by the ObjectPool.
 */
template <class T>
using SmartBaseTypeObjectPoolUniquePtr = std::unique_ptr<T, detail::SmartBaseTypeObjectPoolDeleter>;

/*!
 * \brief Smart Pointer type used by the ObjectPool for const Objects.
 */
template <class T>
using SmartBaseTypeObjectPoolUniquePtrToConst = std::unique_ptr<const T, detail::SmartBaseTypeObjectPoolDeleter>;

/* VECTOR Next Construct AutosarC++17_10-A11.3.1: MD_VAC_A11.3.1_doNotUseFriend */
/*!
 * \brief ObjectPool that deals in unique_ptrs instead of raw pointers, allowing for base type pointers.
 * \trace CREQ-158628
 */
template <class T, typename alloc = std::allocator<T>>
class SmartBaseTypeObjectPool {
  friend class SmartBaseTypeObjectPoolTestFixture;
  static_assert(std::is_base_of<DeleteableSmartPoolObject, T>::value, "T must implement of DeleteableSmartPoolObject");

 public:
  /*!
   * \brief Typedef for the raw-pointer ObjectPool used for storage.
   */
  using StoragePoolType = ObjectPool<T, alloc>;

  /*!
   * \brief Typedef for the size type used in this ObjectPool.
   */
  using size_type = typename StoragePoolType::size_type;

  /*!
   * \brief Deleter for unique_ptrs to ObjectPool-managed objects.
   */
  using Deleter = detail::SmartBaseTypeObjectPoolDeleter;

  /*!
   * \brief Smart Pointer type used by the ObjectPool.
   */
  using UniquePtr = SmartBaseTypeObjectPoolUniquePtr<T>;

  /*!
   * \brief Smart Pointer type used by the ObjectPool.
   */
  using UniquePtrToConst = SmartBaseTypeObjectPoolUniquePtrToConst<T>;

  /*!
   * \brief Constructor.
   */
  SmartBaseTypeObjectPool() {}

  /*!
   * \brief Default copy constructor.
   */
  SmartBaseTypeObjectPool(SmartBaseTypeObjectPool const&) = default;

  /*!
   * \brief Default move constructor.
   */
  SmartBaseTypeObjectPool(SmartBaseTypeObjectPool&&) noexcept = default;

  /*!
   * \brief Default copy assignment operator.
   */
  SmartBaseTypeObjectPool& operator=(SmartBaseTypeObjectPool const&) & = default;

  /*!
   * \brief Default move assignment operator.
   */
  SmartBaseTypeObjectPool& operator=(SmartBaseTypeObjectPool&&) & noexcept = default;

  /*!
   * \brief Default virtual destructor.
   */
  virtual ~SmartBaseTypeObjectPool() = default;  // TODO(vsarcbosu): The execption from destructor should be fixed.

  /*!
   * \brief Update the memory allocation.
   *        The current implementation only allows a single allocation. All further reservations will only limit the
   *        visible memory. All subsequent calls to reserve() where new_capacity is greater than the initial
   *        new_capacity will be rejected with a bad_alloc.
   * \param new_capacity The number of T's to reserve space for.
   */
  void reserve(size_type new_capacity) { pool_.reserve(new_capacity); }

  /* VECTOR Next Construct AutosarC++17_10-A18.5.8: MD_VAC_A18.5.8_localObjectsShallBeAllocatedOnStack */
  /*!
   * \brief  Allocate and initialize an object of the pool.
   *         Calls the constructor with the given set of arguments. For all other properties, see allocate().
   * \param  args Arguments forwarded to the constructor.
   * \return A pointer to an initialized T.
   */
  template <typename... Args>
  auto create(Args&&... args) noexcept(false) -> UniquePtr {
    typename StoragePoolType::pointer ptr{pool_.create(std::forward<Args>(args)..., &deleter_context_)};
    UniquePtr smart_ptr{ptr, Deleter()};
    return smart_ptr;
  }

  /*!
   * \brief  Determine whether the allocated memory is full.
   * \return False if there is space left in this object pool, i.e., the next call to create() will
   *         succeed. True when the next call to create() will throw.
   */
  bool full() const noexcept { return pool_.full(); }

  /*!
   * \brief  Determine whether there are not objects currently allocated.
   * \return True if no element is allocated inside this object pool. false otherwise.
   */
  bool empty() const noexcept { return pool_.empty(); }

  /*!
   * \brief The total number of objects that can be allocated in this pool.
   */
  size_type capacity() const noexcept { return pool_.capacity(); }

  /*!
   * \brief The number of objects currently allocated in this pool.
   */
  size_type size() const noexcept { return pool_.size(); }

 private:
  /*!
   * \brief pool used for storage.
   */
  StoragePoolType pool_{};

  /*!
   * \brief deleter context for ObjectPool-managed objects.
   */
  detail::SmartObjectPoolDeleterContextImpl<T, alloc> deleter_context_{&pool_};
};

}  // namespace memory
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_MEMORY_SMART_BASE_TYPE_OBJECT_POOL_H_
