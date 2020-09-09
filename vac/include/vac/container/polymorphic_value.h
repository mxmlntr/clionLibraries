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
/*!        \file  polymorphic_value.h
 *        \brief  Contains the PolymorphicValue class implementation.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_CONTAINER_POLYMORPHIC_VALUE_H_
#define LIB_VAC_INCLUDE_VAC_CONTAINER_POLYMORPHIC_VALUE_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/

#include <type_traits>
#include <utility>
#include "ara/core/variant.h"
#include "vac/language/cpp17_backport.h"
#include "vac/language/throw_or_terminate.h"
#include "vac/testing/test_adapter.h"

namespace vac {
namespace container {

/*!
 * \brief   A polymorphic object with value semantics.
 * \details A polymorphic object that supports copying without slicing.
 * \trace   CREQ-158605
 */
template <class TBase, class... TDerivedArgs>
class PolymorphicValue final {
  // Assertions.
  static_assert(language::compile_time::all<std::is_base_of<TBase, TDerivedArgs>::value...>::value,
                "PolymorphicValues must derive from a common base");

  /*! \brief Base type. */
  using base_type = TBase;
  /*! \brief Base type pointer. */
  using base_pointer_type = typename std::add_pointer<TBase>::type;
  /*! \brief Base type pointer. */
  using const_base_pointer_type = typename std::add_pointer<const TBase>::type;
  /*! \brief Base type reference. */
  using base_reference_type = typename std::add_lvalue_reference<TBase>::type;
  /*! \brief Base type reference. */
  using const_base_reference_type = typename std::add_lvalue_reference<const TBase>::type;
  /*! \brief Type used to store the object. base_pointer_type is stored as variant to store nullptr. */
  using storage_type = ara::core::Variant<base_pointer_type, TDerivedArgs...>;

 public:
  /*!
   * \brief   Default construction.
   * \details The default constructor exploits that default construction of a variant is supported (only) for its first
   *          specialization, in our case this is always the base _pointer_ type - which is initialized to nullptr.
   */
  PolymorphicValue() noexcept(std::is_nothrow_constructible<storage_type, base_pointer_type>::value)
      : base_ptr_(nullptr), storage_(nullptr) {}

  /*!
   * \brief Construction from value.
   * \param value The value from which to construct.
   */
  template <class T, typename = typename std::enable_if<
                         !std::is_same<typename std::decay<T>::type, PolymorphicValue>::value>::type>
  explicit PolymorphicValue(T&& value) noexcept(std::is_nothrow_assignable<storage_type, T>::value)
      : base_ptr_(nullptr), storage_(std::forward<T>(value)) {
    base_ptr_ = storage_.template get_if<T>();
  }

  /*!
   * \brief Copy constructor.
   * \param other The variant from which to construct.
   */
  PolymorphicValue(PolymorphicValue const& other) noexcept(std::is_nothrow_copy_assignable<storage_type>::value)
      : base_ptr_(nullptr), storage_(other.storage_) {
    if (!other.empty()) {
      fix_base_ptr();
    }
  }

  /*!
   * \brief Move constructor.
   * \param other The variant from which to construct.
   */
  PolymorphicValue(PolymorphicValue&& other) noexcept(std::is_nothrow_move_constructible<storage_type>::value)
      : base_ptr_(nullptr), storage_(std::move(other.storage_)) {
    if (!other.empty()) {
      other.base_ptr_ = nullptr;  // assure other is completely moved.
      fix_base_ptr();
    }
  }

  /*!
   * \brief   Destructor.
   * \details Will call destructor of contained value.
   */
  ~PolymorphicValue() { base_ptr_ = nullptr; }

  /*!
   * \brief  Copy assignment.
   * \param  other The value that is to be copied.
   * \return A reference on the copied-to variant.
   */
  PolymorphicValue& operator=(PolymorphicValue const& other) noexcept(
      std::is_nothrow_copy_assignable<storage_type>::value) {
    base_ptr_ = nullptr;  // In case of exception keep empty.
    this->storage_ = other.storage_;
    if (!other.empty()) {
      fix_base_ptr();
    }
    return *this;
  }

  /*!
   * \brief  Store an object into the PolymorphicValue by copy.
   * \param  value The value that is to be set.
   * \return A reference on the copied-to PolymorphicValue.
   */
  template <class T, typename = std::enable_if_t<!std::is_same<std::decay_t<T>, PolymorphicValue>::value>>
  PolymorphicValue& operator=(T const& value) {
    base_ptr_ = nullptr;  // In case of exception keep empty.
    storage_.template emplace<T>(value);
    base_ptr_ = storage_.template get_if<T>();
    return *this;
  }

  /*!
   * \brief  Move assignment.
   * \param  other The value that is to be moved.
   * \return A reference on the moved-to variant.
   */
  PolymorphicValue& operator=(PolymorphicValue&& other) noexcept(
      vac::language::is_nothrow_swappable<storage_type>::value) {
    base_ptr_ = nullptr;  // In case of exception keep empty.
    std::swap(storage_, other.storage_);
    std::swap(base_ptr_, other.base_ptr_);
    if (!empty()) {
      fix_base_ptr();
    }
    return *this;
  }

  /*!
   * \brief  Move an object into the PolymorphicValue.
   * \param  value The value that is to be set.
   * \return A reference on the copied-to PolymorphicValue.
   */
  template <class T, typename = std::enable_if_t<!std::is_same<std::decay_t<T>, PolymorphicValue>::value>>
  PolymorphicValue& operator=(T&& value) {
    base_ptr_ = nullptr;  // In case of exception keep empty.
    storage_.template emplace<T>(std::forward<T>(value));
    base_ptr_ = storage_.template get_if<T>();
    return *this;
  }

  /*!
   * \brief  Emplacement by forwarding.
   * \param  args The arguments to pass to a constructor.
   * \return Reference to the constructed element.
   */
  template <class T, class... TArgs>
  T& emplace(TArgs&&... args) {
    base_ptr_ = nullptr;  // In case of exception keep empty.
    storage_.template emplace<T>(std::forward<TArgs>(args)...);
    base_ptr_ = storage_.template get_if<T>();
    return *storage_.template get_if<T>();
  }

  /*!
   * \brief  Operator* const.
   * \return Base class reference to the stored object.
   * \trace  CREQ-158606
   */
  const_base_reference_type operator*() const {
    if (empty()) {
      // throw bad_variant_access to be consistent with get<T> while T is not currently stored.
      vac::language::ThrowOrTerminate<ara::core::bad_variant_access>();
    }
    return *base_ptr_;
  }

  /* VECTOR Disable AutosarC++17_10-A5.2.3: MD_VAC_A5.2.3_constCastReducesCodeDuplication */
  /*!
   * \brief  Operator*.
   * \return Base class reference to the stored object.
   */
  base_reference_type operator*() {
    return const_cast<base_reference_type>(static_cast<PolymorphicValue const*>(this)->operator*());
  }
  /* VECTOR Enable AutosarC++17_10-A5.2.3 */

  /*!
   * \brief  Operator ->.
   * \return Base class pointer to the stored object.
   * \trace  CREQ-158606
   */
  base_pointer_type operator->() noexcept { return base_ptr_; }

  /* VECTOR Next Construct AutosarC++17_10-M9.3.1: MD_VAC_M9.3.1_addPointerConst */
  /*!
   * \brief  Operator -> const.
   * \return Base class pointer to the stored object.
   */
  const_base_pointer_type operator->() const noexcept { return base_ptr_; }

  /*!
   * \brief  Get the base class reference.
   * \return Base class reference to the stored object.
   */
  base_reference_type get() { return this->operator*(); }

  /*!
   * \brief  Get the base class reference, const version.
   * \return Base class reference to the stored object.
   */
  const_base_reference_type get() const { return this->operator*(); }

  /*!
   * \brief  Get a derived class reference.
   * \return Reference to the stored object. will throw if the stored object is not of the requested type.
   */
  template <class T>
  auto get(std::enable_if_t<not std::is_same<std::decay_t<T>, TBase>::value> const* = nullptr)
      -> std::add_lvalue_reference_t<T> {
    static_assert(language::compile_time::any<std::is_same<T, TDerivedArgs>::value...>::value,
                  "Must use a derived type");
    return ara::core::get<T>(storage_);
  }

  /*!
   * \brief  Get a derived class reference, const version.
   * \return Reference to the stored object. will throw if the stored object is not of the requested type.
   */
  template <class T>
  auto get(std::enable_if_t<not std::is_same<std::decay_t<T>, TBase>::value> const* = nullptr) const
      -> std::add_lvalue_reference_t<T const> {
    static_assert(language::compile_time::any<std::is_same<T, TDerivedArgs>::value...>::value,
                  "Must use a derived type");
    return ara::core::get<T>(storage_);
  }

  /*!
   * \brief  Get the base class reference.
   * \return Base class reference to the stored object.
   */
  template <class T>
  auto get(std::enable_if_t<std::is_same<std::decay_t<T>, TBase>::value> const* = nullptr)
      -> std::add_lvalue_reference_t<T> {
    return **this;
  }

  /*!
   * \brief  Get the base class reference, const version.
   * \return Base class reference to the stored object.
   */
  template <class T>
  auto get(std::enable_if_t<std::is_same<std::decay_t<T>, TBase>::value> const* = nullptr) const
      -> std::add_lvalue_reference_t<const T> {
    return **this;
  }

  /*!
   * \brief  Get the base class reference.
   * \return Base class reference to the stored object or nullptr if no object is stored.
   */
  base_pointer_type get_if() { return base_ptr_; }

  /*!
   * \brief  Get the base class reference, const version.
   * \return Base class reference to the stored object or nullptr if no object is stored.
   */
  const_base_pointer_type get_if() const { return base_ptr_; }

  /*!
   * \brief  Get a non-owning base class pointer.
   * \return Base class reference to the stored object or nullptr if no object is stored.
   */
  template <class T>
  auto get_if(std::enable_if_t<std::is_same<std::decay_t<T>, TBase>::value> const* = nullptr) -> base_pointer_type {
    return base_ptr_;
  }

  /*!
   * \brief  Get a non-owning base class pointer, const version.
   * \return Base class reference to the stored object, or nullptr if no object is stored.
   */
  template <class T>
  auto get_if(std::enable_if_t<std::is_same<std::decay_t<T>, TBase>::value> const* = nullptr) const
      -> const_base_pointer_type {
    return base_ptr_;
  }

  /*!
   * \brief  Get a non-owning pointer to the requested derived class.
   * \return Pointer to the stored class or nullptr if no or a different derived class is stored.
   */
  template <class T>
  auto get_if(std::enable_if_t<not std::is_same<std::decay_t<T>, TBase>::value> const* = nullptr)
      -> std::add_pointer_t<T> {
    static_assert(language::compile_time::any<std::is_same<T, TDerivedArgs>::value...>::value,
                  "Must use a derived type");
    return storage_.template get_if<T>();
  }

  /*!
   * \brief  Get a non-owning pointer to the requested derived class, const version.
   * \return Pointer to the stored class or nullptr if no or a different derived class is stored.
   */
  template <class T>
  auto get_if(std::enable_if_t<not std::is_same<std::decay_t<T>, TBase>::value> const* = nullptr) const
      -> std::add_pointer_t<const T> {
    static_assert(language::compile_time::any<std::is_same<T, TDerivedArgs>::value...>::value,
                  "Must use a derived type");
    return storage_.template get_if<T>();
  }

  /*!
   * \brief  Test whether a concrete derived object is stored.
   * \return True if the stored object is of the given type, otherwise false.
   */
  template <class T>
  bool holds_alternative() const noexcept {
    static_assert(language::compile_time::any<std::is_same<T, TDerivedArgs>::value...>::value,
                  "Must use a derived type");
    return ara::core::holds_alternative<T>(storage_);
  }

  /*!
   * \brief  Test if any object is stored.
   * \return True there is no stored object, otherwise false.
   */
  bool empty() const noexcept { return base_ptr_ == nullptr; }

  /*!
   * \brief Clear the polyobject.
   */
  void reset() {
    base_ptr_ = nullptr;
    storage_.template emplace<base_pointer_type>(nullptr);
  }

 private:
  /*!
   * \brief Set up base type pointer.
   */
  class get_base_ptr_op final {
   public:
    /*! \brief Returns pointer to a derived class. */
    template <class Type>
    auto operator()(Type& variant) -> base_pointer_type {
      return &variant;
    }
    /*! \brief Returns base pointer. */
    base_pointer_type operator()(base_pointer_type variant) { return variant; }
  };

  /*!
   * \brief Set up base type pointer.
   */
  void fix_base_ptr() { base_ptr_ = ara::core::visit(get_base_ptr_op{}, storage_); }

 private:
  /*!
   * \brief Pointer to an object, or null.
   */
  base_pointer_type base_ptr_;
  /*!
   * \brief Storage for a polymorphic value.
   */
  storage_type storage_;

  FRIEND_TEST(PolyValueTest, GetBasePtrOp);
};

}  // namespace container
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_CONTAINER_POLYMORPHIC_VALUE_H_
