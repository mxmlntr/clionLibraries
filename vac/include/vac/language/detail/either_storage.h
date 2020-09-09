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
/*!        \file  either_storage.h
 *        \brief  Contains the memory handling of the Either type.
 *
 *      \details  Implements the rule of 5 & union for storing the two types.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_LANGUAGE_DETAIL_EITHER_STORAGE_H_
#define LIB_VAC_INCLUDE_VAC_LANGUAGE_DETAIL_EITHER_STORAGE_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <type_traits>
#include <utility>

#include "ara/core/utility.h"
#include "vac/language/cpp17_backport.h"
#include "vac/language/detail/copy_move_enabler.h"
#include "vac/language/detail/either_trait.h"

namespace vac {
namespace language {
namespace detail {

/*!
 * \brief  An alias for ara::core::in_place_type_t.
 */
template <typename T>
using in_place_type_t = ara::core::in_place_type_t<T>;

/*! \brief Type to use in uninitialized union. */
struct EmptyUnionT {};

/* VECTOR Next Construct VectorC++-V11-0-2: MD_VAC_V11-0-2_mutableUnionMemberShallBePrivate */
/* VECTOR Next Construct AutosarC++17_10-A9.5.1: MD_VAC_A9.5.1_unionsShallNotBeUsed */
/* VECTOR Next Construct AutosarC++17_10-M11.0.1: MD_VAC_M11.0.1_memberDataInNonPODUnionTypesShallBePrivate */
/*! \brief Union for L and R type of Either when both are trivially destructible. */
template <typename L, typename R, bool = EitherTrait<L, R>::is_trivially_destructible>
union EitherUnion {
  /*! \brief Default constructor without member initialization. */
  constexpr EitherUnion() noexcept : empty_{} {}

  /*! \brief In place constructor for Left type. */
  template <typename... Args>
  constexpr explicit EitherUnion(in_place_type_t<L>, Args&&... args) : left_{std::forward<Args>(args)...} {}

  /*! \brief In place constructor for Right type. */
  template <typename... Args>
  constexpr explicit EitherUnion(in_place_type_t<R>, Args&&... args) : right_{std::forward<Args>(args)...} {}

  /*! \brief Left type. */
  L left_;
  /*! \brief Right type. */
  R right_;
  /*! \brief Unused uninitialized type. */
  EmptyUnionT empty_;
};

/* VECTOR Next Construct VectorC++-V11-0-2: MD_VAC_V11-0-2_mutableUnionMemberShallBePrivate */
/* VECTOR Next Construct AutosarC++17_10-A9.5.1: MD_VAC_A9.5.1_unionsShallNotBeUsed */
/* VECTOR Next Construct AutosarC++17_10-M11.0.1: MD_VAC_M11.0.1_memberDataInNonPODUnionTypesShallBePrivate */
/*! \brief Union for L and R type of Either when both are not trivially destructible. */
template <typename L, typename R>
union EitherUnion<L, R, false> {
  /*! \brief Default constructor without member initialization. */
  constexpr EitherUnion() noexcept : empty_{} {}

  /*! \brief In place constructor for Left type. */
  template <typename... Args>
  constexpr explicit EitherUnion(in_place_type_t<L>, Args&&... args) : left_{std::forward<Args>(args)...} {}

  /*! \brief In place constructor for Right type. */
  template <typename... Args>
  constexpr explicit EitherUnion(in_place_type_t<R>, Args&&... args) : right_{std::forward<Args>(args)...} {}

  /* VECTOR Next Construct AutosarC++17_10-M0.1.8: MD_VAC_M0.1.8_destructorHasNoExternalSideEffect */
  /*! \brief Non-trivial destructor. */
  ~EitherUnion() {}
  /*! \brief Default copy constructor */
  EitherUnion(EitherUnion const&) = default;
  /*! \brief Default move constructor */
  EitherUnion(EitherUnion&&) = default;
  /*! \brief Default copy assignment */
  EitherUnion& operator=(EitherUnion const&) & = default;
  /*! \brief Default move assignment */
  EitherUnion& operator=(EitherUnion&&) & = default;

  /*! \brief Left type. */
  L left_;
  /*! \brief Right type. */
  R right_;
  /*! \brief Unused uninitialized type. */
  EmptyUnionT empty_;
};

/*! \brief Token for calling non-trivial constructors for EitherPayloadTrivial. */
struct NotTrivialEitherToken {};

/* VECTOR Disable AutosarC++17_10-A10.2.1: MD_VAC_A10.2.1_redefinitionOfNonVirtualFunction */
/*!
 * \brief Contains EitherUnion and provides utility functions for non-trivial copy/move construct/assignment.
 *        This layer only supports trivial construct/assignment. Non-trivial constructor/assignment is added by
 *        additional layers using utility functions defined here.
 */
template <typename L, typename R>
class EitherPayloadTrivial {
 protected:
  /* VECTOR Next Construct AutosarC++17_10-M0.1.8: MD_VAC_M0.1.8_destructorHasNoExternalSideEffect */
  /*! \brief Protected destructor. */
  ~EitherPayloadTrivial() = default;
  /*! \brief Default copy constructor. */
  EitherPayloadTrivial(EitherPayloadTrivial const&) = default;
  /*! \brief Default move constructor. */
  EitherPayloadTrivial(EitherPayloadTrivial&&) = default;
  /*! \brief Default copy assignment. */
  EitherPayloadTrivial& operator=(EitherPayloadTrivial const&) & = default;
  /*! \brief Default move assignment. */
  EitherPayloadTrivial& operator=(EitherPayloadTrivial&&) & = default;

  /*! \brief In place constructor for Left type. */
  /*! \brief Left type without const specifier */
  using NonConstL = std::remove_const_t<L>;
  /*! \brief Right type without const specifier */
  using NonConstR = std::remove_const_t<R>;

  /* VECTOR Disable AutosarC++17_10-A12.1.5: MD_VAC_A12.1.5_useDelegatingConstructor */
 public:
  /*! \brief In place constructor for Left type */
  template <typename... Args>
  constexpr explicit EitherPayloadTrivial(in_place_type_t<L>, Args&&... args)
      : storage_{in_place_type_t<L>{}, std::forward<Args>(args)...}, is_left_{true} {}

  /*! \brief In place constructor for Right type. */
  template <typename... Args>
  constexpr explicit EitherPayloadTrivial(in_place_type_t<R>, Args&&... args)
      : storage_{in_place_type_t<R>{}, std::forward<Args>(args)...}, is_left_{false} {}

  /*! \brief Non-trivial copy constructor auxiliary. */
  constexpr EitherPayloadTrivial(NotTrivialEitherToken, EitherPayloadTrivial const& other) : storage_{}, is_left_{} {
    if (other.is_left_) {
      ConstructLeft(other.GetL());
    } else {
      ConstructRight(other.GetR());
    }
  }

  /*! \brief Non-trivial move constructor auxiliary. */
  constexpr EitherPayloadTrivial(NotTrivialEitherToken, EitherPayloadTrivial&& other) : storage_{}, is_left_{} {
    if (other.is_left_) {
      ConstructLeft(std::move(other.GetL()));
    } else {
      ConstructRight(std::move(other.GetR()));
    }
  }

  /* VECTOR Enable AutosarC++17_10-M0.1.8: MD_VAC_A12.1.5_useDelegatingConstructor */
  /* VECTOR Next Construct AutosarC++17_10-M5.2.12: MD_VAC_M5.2.12_arraytoPointerDecay */
  /*! \brief Constructs Left type in place without calling destructor. */
  template <typename... Args>
  void ConstructLeft(Args&&... args) {
    new (&storage_.left_) NonConstL(std::forward<Args>(args)...);
    is_left_ = true;
  }

  /* VECTOR Next Construct AutosarC++17_10-M5.2.12: MD_VAC_M5.2.12_arraytoPointerDecay */
  /*! \brief Constructs Right type in place without calling destructor. */
  template <typename... Args>
  void ConstructRight(Args&&... args) {
    new (&storage_.right_) NonConstR(std::forward<Args>(args)...);
    is_left_ = false;
  }

  /*! \brief Copy assignment auxiliary. */
  constexpr void CopyAssign(EitherPayloadTrivial const& other) {
    if (is_left_ != other.is_left_) {
      Reset();
      if (is_left_) {
        ConstructRight(other.GetR());
      } else {
        ConstructLeft(other.GetL());
      }
    } else if (is_left_ && other.is_left_) {
      GetL() = other.GetL();
    } else {
      GetR() = other.GetR();
    }
  }

  /*! \brief Move assignment auxiliary. */
  constexpr void MoveAssign(EitherPayloadTrivial&& other) {
    if (is_left_ != other.is_left_) {
      Reset();
      if (is_left_) {
        ConstructRight(std::move(other.GetR()));
      } else {
        ConstructLeft(std::move(other.GetL()));
      }
    } else if (is_left_ && other.is_left_) {
      GetL() = std::move(other.GetL());
    } else {
      GetR() = std::move(other.GetR());
    }
  }

  /* VECTOR Next Construct AutosarC++17_10-M0.1.8: MD_VAC_M0.1.8_destructorHasNoExternalSideEffect */
  /*! \brief Calls the destructor for the stored type. */
  constexpr void Reset() noexcept {
    if (is_left_) {
      storage_.left_.~NonConstL();
    } else {
      storage_.right_.~NonConstR();
    }
  }

  /*! \brief Retrieves stored Left type. Requires Left to be the stored type. */
  constexpr L& GetL() noexcept { return storage_.left_; }
  /*! \brief Retrieves stored Left type. Requires Left to be the stored type. */
  constexpr L const& GetL() const noexcept { return storage_.left_; }
  /*! \brief Retrieves stored Right type. Requires Right to be the stored type. */
  constexpr R& GetR() noexcept { return storage_.right_; }
  /*! \brief Retrieves stored Right type. Requires Right to be the stored type. */
  constexpr R const& GetR() const noexcept { return storage_.right_; }
  /*! \brief Indicates whether a left value type is stored. */
  constexpr bool IsLeft() const noexcept { return is_left_; }

  /*! \brief The union storage for Left and Right. */
 private:
  /*! \brief The union storage for Left and Right */
  EitherUnion<NonConstL, NonConstR> storage_;
  /*! \brief Indicator for which type is stored. */
  bool is_left_;
};

/*! \brief Adds destructor, copy and move assignment if they are not trivial. */
template <typename L, typename R, bool = EitherTrait<L, R>::is_trivially_destructible,
          bool = EitherTrait<L, R>::is_trivially_copy_assignable,
          bool = EitherTrait<L, R>::is_trivially_move_assignable>
class EitherPayloadAssign;

/*! \brief Uses defaults. */
template <typename L, typename R>
class EitherPayloadAssign<L, R, true, true, true> : public EitherPayloadTrivial<L, R> {
 protected:
  /*! \brief Protected destructor. */
  ~EitherPayloadAssign() = default;

  using EitherPayloadTrivial<L, R>::EitherPayloadTrivial;
};

/*! \brief Adds copy assignment. */
template <typename L, typename R>
class EitherPayloadAssign<L, R, true, false, true> : public EitherPayloadTrivial<L, R> {
 protected:
  /*! \brief Protected destructor. */
  ~EitherPayloadAssign() = default;

  using EitherPayloadTrivial<L, R>::EitherPayloadTrivial;

  /*! \brief Default constructor. */
  EitherPayloadAssign() = default;
  /*! \brief Default copy constructor. */
  EitherPayloadAssign(EitherPayloadAssign const&) = default;
  /*! \brief Default move constructor. */
  EitherPayloadAssign(EitherPayloadAssign&&) = default;
  /*! \brief Default move assignment. */
  EitherPayloadAssign& operator=(EitherPayloadAssign&&) & = default;

  /*! \brief Non-trivial copy assignment. */
  constexpr EitherPayloadAssign& operator=(EitherPayloadAssign const& other) & {
    this->CopyAssign(other);
    return *this;
  }
};

/*! \brief Adds move assignment. */
template <typename L, typename R>
class EitherPayloadAssign<L, R, true, true, false> : public EitherPayloadTrivial<L, R> {
 protected:
  /*! \brief Protected destructor. */
  ~EitherPayloadAssign() = default;

  using EitherPayloadTrivial<L, R>::EitherPayloadTrivial;

  /*! \brief Default constructor. */
  EitherPayloadAssign() = default;
  /*! \brief Default copy constructor. */
  EitherPayloadAssign(EitherPayloadAssign const&) = default;
  /*! \brief Default move constructor. */
  EitherPayloadAssign(EitherPayloadAssign&&) = default;
  /*! \brief Default copy assignment. */
  EitherPayloadAssign& operator=(EitherPayloadAssign const&) & = default;

  /*! \brief Non-trivial move assignment. */
  constexpr EitherPayloadAssign& operator=(EitherPayloadAssign&& other) &
      noexcept(EitherTrait<L, R>::is_nothrow_move_assignable) {
    this->MoveAssign(std::move(other));
    return *this;
  }
};

/*! \brief Adds copy and move assignment. */
template <typename L, typename R>
class EitherPayloadAssign<L, R, true, false, false> : public EitherPayloadTrivial<L, R> {
 protected:
  /* VECTOR Next Construct AutosarC++17_10-M0.1.8: MD_VAC_M0.1.8_destructorHasNoExternalSideEffect */
  /*! \brief Protected destructor. */
  ~EitherPayloadAssign() = default;

  using EitherPayloadTrivial<L, R>::EitherPayloadTrivial;

  /*! \brief Default constructor. */
  EitherPayloadAssign() = default;
  /*! \brief Default copy constructor. */
  EitherPayloadAssign(EitherPayloadAssign const&) = default;
  /*! \brief Default move constructor. */
  EitherPayloadAssign(EitherPayloadAssign&&) = default;

  /*! \brief Non-trivial copy assignment. */
  constexpr EitherPayloadAssign& operator=(EitherPayloadAssign const& other) & {
    this->CopyAssign(other);
    return *this;
  }

  /*! \brief Non-trivial move assignment. */
  constexpr EitherPayloadAssign& operator=(EitherPayloadAssign&& other) &
      noexcept(EitherTrait<L, R>::is_nothrow_move_assignable) {
    this->MoveAssign(std::move(other));
    return *this;
  }
};

/*! \brief Adds destructor, copy and move assignment. */
template <typename L, typename R, bool IsTrivialCopyAssign, bool IsTrivialMoveAssign>
class EitherPayloadAssign<L, R, false, IsTrivialCopyAssign, IsTrivialMoveAssign>
    : public EitherPayloadAssign<L, R, true, false, false> {
 protected:
  /* VECTOR Next Construct AutosarC++17_10-M0.1.8: MD_VAC_M0.1.8_destructorHasNoExternalSideEffect */
  /*! \brief Protected non-trivial destructor. */
  ~EitherPayloadAssign() { this->Reset(); }

  using EitherPayloadAssign<L, R, true, false, false>::EitherPayloadAssign;

  /*! \brief Default constructor. */
  EitherPayloadAssign() = default;
  /*! \brief Default copy constructor. */
  EitherPayloadAssign(EitherPayloadAssign const&) = default;
  /*! \brief Default move constructor. */
  EitherPayloadAssign(EitherPayloadAssign&&) = default;
  /*! \brief Default copy assignment. */
  EitherPayloadAssign& operator=(EitherPayloadAssign const&) & = default;
  /*! \brief Default move constructor. */
  EitherPayloadAssign& operator=(EitherPayloadAssign&&) & = default;
};

/* VECTOR Disable AutosarC++17_10-A12.1.1: MD_VAC_A12.1.1_baseClassIsNotExplicitlyInitializedInConstructor */
/*! \brief Adds copy and move constructors. */
template <typename L, typename R, bool = EitherTrait<L, R>::is_trivially_copy_constructible,
          bool = EitherTrait<L, R>::is_trivially_move_constructible>
class EitherPayloadConstruct final : public EitherPayloadAssign<L, R> {
 public:
  using EitherPayloadAssign<L, R>::EitherPayloadAssign;
  /*! \brief Non-trivial copy constructor. */
  constexpr EitherPayloadConstruct(EitherPayloadConstruct const& other)
      : EitherPayloadAssign<L, R>::EitherPayloadAssign(NotTrivialEitherToken{}, other) {}
  /* VECTOR Next Construct AutosarC++17_10-A12.8.4: MD_VAC_A12.8.4_moveConstructorShallNotUseCopySemantics */
  /*! \brief Non-trivial move constructor. */
  constexpr EitherPayloadConstruct(EitherPayloadConstruct&& other) noexcept(
      EitherTrait<L, R>::is_nothrow_move_constructible)
      : EitherPayloadAssign<L, R>::EitherPayloadAssign(NotTrivialEitherToken{}, std::move(other)) {}

  /*! \brief Default copy assignment. */
  EitherPayloadConstruct& operator=(EitherPayloadConstruct const&) & = default;
  /*! \brief Default move assignment. */
  EitherPayloadConstruct& operator=(EitherPayloadConstruct&&) & = default;

  /* VECTOR Next Construct AutosarC++17_10-M0.1.9: MD_VAC_M0.1.9_destructorStatementWithoutSideEffect */
  /* VECTOR Next Construct AutosarC++17_10-M0.1.8: MD_VAC_M0.1.8_destructorHasNoExternalSideEffect */
  /*! \brief Default destructor. */
  ~EitherPayloadConstruct() = default;
};
/* VECTOR Enable AutosarC++17_10-A12.1.1: MD_VAC_A12.1.1_baseClassIsNotExplicitlyInitializedInConstructor */

/*! \brief Adds copy constructor. */
template <typename L, typename R>
class EitherPayloadConstruct<L, R, false, true> final : public EitherPayloadAssign<L, R> {
 public:
  using EitherPayloadAssign<L, R>::EitherPayloadAssign;

  /* VECTOR Next Construct AutosarC++17_10-A12.1.1: MD_VAC_A12.1.1_baseClassIsNotExplicitlyInitializedInConstructor */
  /*! \brief Non-trivial copy constructor. */
  constexpr EitherPayloadConstruct(EitherPayloadConstruct const& other)
      : EitherPayloadAssign<L, R>::EitherPayloadAssign(NotTrivialEitherToken{}, other) {}
  /*! \brief Default move constructor. */
  constexpr EitherPayloadConstruct(EitherPayloadConstruct&& other) = default;

  /*! \brief Default copy assignment. */
  EitherPayloadConstruct& operator=(EitherPayloadConstruct const&) & = default;
  /*! \brief Default move assignment. */
  EitherPayloadConstruct& operator=(EitherPayloadConstruct&&) & = default;
};

/*! \brief Adds move constructor. */
template <typename L, typename R>
class EitherPayloadConstruct<L, R, true, false> final : public EitherPayloadAssign<L, R> {
 public:
  using EitherPayloadAssign<L, R>::EitherPayloadAssign;

  /*! \brief Default copy constructor. */
  constexpr EitherPayloadConstruct(EitherPayloadConstruct const& other) = default;

  /* VECTOR Next Construct AutosarC++17_10-A12.8.4: MD_VAC_A12.8.4_moveConstructorShallNotUseCopySemantics */
  /* VECTOR Next Construct AutosarC++17_10-A12.1.1: MD_VAC_A12.1.1_baseClassIsNotExplicitlyInitializedInConstructor */
  /*! \brief Non-trivial move constructor. */
  constexpr EitherPayloadConstruct(EitherPayloadConstruct&& other) noexcept(
      EitherTrait<L, R>::is_nothrow_move_constructible)
      : EitherPayloadAssign<L, R>::EitherPayloadAssign(NotTrivialEitherToken{}, std::move(other)) {}

  /*! \brief Default copy assignment. */
  EitherPayloadConstruct& operator=(EitherPayloadConstruct const&) & = default;
  /*! \brief Default move assignment. */
  EitherPayloadConstruct& operator=(EitherPayloadConstruct&&) & = default;
};

/*! \brief Uses defaults. */
template <typename L, typename R>
class EitherPayloadConstruct<L, R, true, true> final : public EitherPayloadAssign<L, R> {
 public:
  using EitherPayloadAssign<L, R>::EitherPayloadAssign;

  /*! \brief Default copy constructor. */
  constexpr EitherPayloadConstruct(EitherPayloadConstruct const& other) = default;
  /*! \brief Default move constructor. */
  constexpr EitherPayloadConstruct(EitherPayloadConstruct&& other) = default;

  /*! \brief Default copy assignment. */
  EitherPayloadConstruct& operator=(EitherPayloadConstruct const&) & = default;
  /*! \brief Default move assignment. */
  EitherPayloadConstruct& operator=(EitherPayloadConstruct&&) & = default;
};
/* VECTOR Enable AutosarC++17_10-A10.2.1: MD_VAC_A10.2.1_redefinitionOfNonVirtualFunction */

/*!
 * \brief  Memory handling class for Either.
 * \tparam L The left type.
 * \tparam R The right type.
 */
template <typename L, typename R>
class EitherStorage final
    : CopyMoveEnabler<EitherTrait<L, R>::is_copy_constructible, EitherTrait<L, R>::is_move_constructible,
                      EitherTrait<L, R>::is_copy_assignable, EitherTrait<L, R>::is_move_assignable> {
 public:
  /*! \brief Traits for storage of L and R. */
  using Trait = EitherTrait<L, R>;

  /*! \brief Default copy constructor. */
  EitherStorage(EitherStorage const&) = default;
  /*! \brief Default move constructor. */
  EitherStorage(EitherStorage&&) = default;
  /*! \brief Default copy assignment. */
  EitherStorage& operator=(EitherStorage const&) & = default;
  /*! \brief Default move assignment. */
  EitherStorage& operator=(EitherStorage&&) & = default;

  /* VECTOR Next Construct AutosarC++17_10-M0.1.8: MD_VAC_M0.1.8_destructorHasNoExternalSideEffect */
  /*! \brief Default destructor. */
  ~EitherStorage() = default;

  /* VECTOR Next Construct AutosarC++17_10-A12.1.5: MD_VAC_A12.1.5_useDelegatingConstructor */
  /* VECTOR Next Construct AutosarC++17_10-A12.1.1: MD_VAC_A12.1.1_baseClassIsNotExplicitlyInitializedInConstructor */
  /*! \brief In place constructor for Left type. */
  template <typename... Args, typename = std::enable_if_t<std::is_constructible<L, Args...>::value>>
  constexpr explicit EitherStorage(in_place_type_t<L>, Args&&... args)
      : storage_{in_place_type_t<L>{}, std::forward<Args>(args)...} {}

  /* VECTOR Next Construct AutosarC++17_10-A12.1.5: MD_VAC_A12.1.5_useDelegatingConstructor */
  /* VECTOR Next Construct AutosarC++17_10-A12.1.1: MD_VAC_A12.1.1_baseClassIsNotExplicitlyInitializedInConstructor */
  /*! \brief In place constructor for Right type. */
  template <typename... Args, typename = std::enable_if_t<std::is_constructible<R, Args...>::value>>
  constexpr explicit EitherStorage(in_place_type_t<R>, Args&&... args)
      : storage_{in_place_type_t<R>{}, std::forward<Args>(args)...} {}

  /*!
   * \brief  Emplace a left value.
   * \tparam Args Types for the left constructor.
   * \param  args Values for the left constructor.
   * \return Nothing.
   */
  template <typename... Args>
  void EmplaceLeft(Args&&... args) {
    storage_.Reset();
    storage_.ConstructLeft(std::forward<Args>(args)...);
  }

  /*!
   * \brief  Emplace a right value.
   * \tparam Args Types for the right constructor.
   * \param  args Values for the right constructor.
   * \return Nothing.
   */
  template <typename... Args>
  void EmplaceRight(Args&&... args) {
    storage_.Reset();
    storage_.ConstructRight(std::forward<Args>(args)...);
  }

  /*! \brief Returns true if a left value is contained. */
  constexpr auto IsLeft() const noexcept -> bool { return storage_.IsLeft(); }

  /*! \brief Returns a const reference to the contained left value. */
  constexpr auto Left() const noexcept -> L const& {
    assert(IsLeft());
    return storage_.GetL();
  }
  /*! \brief Returns a const reference to the contained right value. */
  constexpr auto Right() const noexcept -> R const& {
    assert(!IsLeft());
    return storage_.GetR();
  }

 private:
  /*! \brief Storage for Left and Right type with constructors and assignment. */
  EitherPayloadConstruct<L, R> storage_;
};

}  // namespace detail
}  // namespace language
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_LANGUAGE_DETAIL_EITHER_STORAGE_H_
