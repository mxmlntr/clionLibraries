/* VECTOR SL AutosarC++17_10-A2.9.1: MD_VAC_AutosarC++17_10-A2.9.1_headerShouldBeNamedAsATypeItDeclares */
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
/*!        \file
 *        \brief  Utility function for delegating unhandled ErrorCode values to a global default error handler.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_LANGUAGE_HANDLE_ERRORS_H_
#define LIB_VAC_INCLUDE_VAC_LANGUAGE_HANDLE_ERRORS_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <type_traits>
#include <utility>

#include "ara/core/error_code.h"

namespace vac {
namespace language {
/* VECTOR Next Construct AutosarC++17_10-A12.8.6: MD_VAC_A12.8.6_definedDeletedInBassClass */
/*!
 * \brief   Interface for a global default error handler.
 * \details Subclasses may be registered as a error handler for errors which are not handled by user defined functions
 *          in HandleErrors. Use SetGlobalDefaultErrorHandler() to register a new global default error handler.
 */
class GlobalDefaultErrorHandler {
 public:
  /*! \brief Destructor. */
  virtual ~GlobalDefaultErrorHandler() noexcept;
  /*! \brief Default constructor. */
  GlobalDefaultErrorHandler() = default;

  /*! \brief Default copy constructor. */
  GlobalDefaultErrorHandler(GlobalDefaultErrorHandler const& other) = default;

  /*! \brief Default move constructor. */
  GlobalDefaultErrorHandler(GlobalDefaultErrorHandler&& other) noexcept = default;

  /*! \brief Deleted copy operator. */
  GlobalDefaultErrorHandler& operator=(GlobalDefaultErrorHandler const& other) = delete;

  /*! \brief Deleted move operator. */
  GlobalDefaultErrorHandler& operator=(GlobalDefaultErrorHandler&& other) = delete;

  /*! \brief Function to overload for unhandled errors. */
  virtual void operator()(ara::core::ErrorCode const& error) = 0;
};

/*!
 * \brief Sets the global default error handler. Overrides the previous one. The initial global default error handler
 *        throws the error as an exception or terminates after writing the error to stderr.
 * \param handler New global default error handler.
 * \trace CREQ-LibVac-HandleErrors003
 */
void SetGlobalDefaultErrorHandler(GlobalDefaultErrorHandler& handler);

/*! \brief Retrieves the current global default error handler. */
GlobalDefaultErrorHandler& GetGlobalDefaultErrorHandler();

/*! \brief Tag type for the else error handler. */
struct ElseErrorHandlerT {};

/* VECTOR Next Construct AutosarC++17_10-M3.4.1: MD_VAC_M3.4.1_symbolsCanBeDeclaredLocally */
/*! \brief Matcher to pass to HandleErrors for the else error handler. Can only be used as the last matcher. */
static constexpr ElseErrorHandlerT else_error_handler;

namespace detail {

/* VECTOR Next Construct AutosarC++17_10-A12.4.1: MD_VAC_A12.4.1_destructorOfABaseClassShallBePublicVirtual */
/*! \brief Type containing a list of typed error codes matching for a single error handler. */
template <typename... Codes>
struct ErrorList;

/* VECTOR Next Construct AutosarC++17_10-A10.2.1: MD_VAC_A10.2.1_redefinitionOfNonVirtualFunction */
/*!
 * \brief Recursive template storing a single typed error code per level.
 *        Provides a matches predicate returning whether the list matches a specific error.
 */
template <typename Code, typename... Codes>
class ErrorList<Code, Codes...> : ErrorList<Codes...> {
 public:
  /*! \brief Stored code type */
  using CodeT = std::decay_t<Code>;

  /*! \brief Constructor */
  explicit constexpr ErrorList(CodeT code, Codes&&... error_codes)
      : ErrorList<Codes...>(std::move(error_codes)...), code_(code) {}

  /*! \brief Returns whether this list matches the error */
  constexpr bool matches(ara::core::ErrorCode const& error) const {
    return (error == code_) || (ErrorList<Codes...>::matches(error));
  }

  /*! \brief One code in the list. */
  CodeT const code_;
};

/* VECTOR Next Construct AutosarC++17_10-A12.4.1: MD_VAC_A12.4.1_destructorOfABaseClassShallBePublicVirtual */
/*! \brief Recursion tail which matches nothing. */
template <>
class ErrorList<> {
 public:
  /*! \brief Recursion tail which matches nothing. */
  constexpr static bool matches(ara::core::ErrorCode const&) { return false; }
};

/*! \brief Returns whether a type is ErrorList, default false. */
template <typename>
struct IsErrorList : std::false_type {};

/* VECTOR Next Construct AutosarC++17_10-M0.1.5: MD_VAC_M0.1.5_UnusedTypeDeclaration */
/*! \brief Returns whether a type is ErrorList, specialization for true. */
template <typename... Args>
struct IsErrorList<ErrorList<Args...>> : std::true_type {};

/*! \brief Template providing matches predicate depending on input type. */
template <typename T, bool = std::is_enum<std::decay_t<T>>::value, bool = IsErrorList<std::decay_t<T>>::value,
          bool = std::is_same<std::decay_t<T>, ElseErrorHandlerT>::value>
struct ErrorMatcher;

/*! \brief Matcher predicate for a single ErrorCode. */
template <typename EnumT>
class ErrorMatcher<EnumT, true, false, false> {
 public:
  /*! \brief Matcher predicate for a single ErrorCode. */
  static bool matches(ara::core::ErrorCode const& error, EnumT errc) { return error == errc; }
};

/*! \brief Matcher predicate for an ErrorList. */
template <typename ErrorListT>
class ErrorMatcher<ErrorListT, false, true, false> {
 public:
  /*! \brief Matcher predicate for an ErrorList. */
  static bool matches(ara::core::ErrorCode const& error, ErrorListT const& error_list) {
    return error_list.matches(error);
  }
};

/*! \brief Matcher predicate for a user provided else error handler. */
template <typename ElseT>
class ErrorMatcher<ElseT, false, false, true> {
 public:
  /*! \brief Else error handler is only allowed as the last link and always matches. */
  constexpr static bool matches(ara::core::ErrorCode const&, ElseT) { return true; }
};

/*! \brief Template which applies an error handler if a matcher matches. */
template <typename... Args>
class HandleErrorsAux;

/*! \brief Applies global default error handler when no user provided matcher matches. */
template <>
class HandleErrorsAux<> {
 public:
  /*! \brief Calls the global default error handler when no user provided matcher matches. */
  static void handle(ara::core::ErrorCode const& error) { GetGlobalDefaultErrorHandler()(error); }
};

/* VECTOR Next Construct AutosarC++17_10-A18.9.2: MD_VAC_A18.9.2_useStdMoveIfTheValueIsARvalueReference */
/*!
 * \brief  Uses two types from the input list, a matcher and an error handler.
 * \tparam T matcher type which is an Enum, ErrorList, or ElseErrorHandlerT.
 * \tparam F error handler a callable accepting an ErrorCode const reference.
 */
template <typename T, typename F, typename... Args>
class HandleErrorsAux<T, F, Args...> {
 public:
  /*!
   * \brief Handles an error using provided handler if matcher matches, otherwise forwards to next pair of matcher and
   *        handler.
   */
  static void handle(ara::core::ErrorCode const& error, T const& errc, F&& f, Args&&... args) {
    static_assert((!std::is_same<std::decay_t<T>, ElseErrorHandlerT>::value) || (0 == sizeof...(args)),
                  "ElseErrorHandler can only be used as the last handler!");
    if (detail::ErrorMatcher<T>::matches(error, errc)) {
      f(error);
    } else {
      HandleErrorsAux<Args...>::handle(error, std::forward<Args>(args)...);
    }
  }
};
}  // namespace detail

/* VECTOR Next Construct AutosarC++17_10-A15.4.4: MD_VAC_A15.4.4_explicitNoexceptSpecificationMissing */
/*!
 * \brief   Handles an error with an error handler selected using the first positive matcher.
 * \details If no matcher matches, the error will be routed to the global default error handler. To specify a local
 *          default error handler, use the any-matcher tag vac::language::else_error_handler with your handler as the
 *          last parameters.
 *          Example: HandleErrors(res.Error(),
 *                                PosixErrc::invalid_argument, [](ErrorCode const&) {},
 *                                ErrorList(PosixErrc::argument_out_of_domain, PosixErrc::argument_list_too_long),
 *                                [](ErrorCode const&) {});
 *          Example: HandleErrors(res.Error(),
 *                                PosixErrc::invalid_argument, [](ErrorCode const&) {},
 *                                else_error_handler, [](ErrorCode const&) { handleMyElseCase(); });
 *
 * \param   error error to be handled.
 * \param   args a list of matcher and handler pairs, where a matcher is an Enum, ErrorList, or ElseErrorHandlerT.
 * \trace   CREQ-LibVac-HandleErrors001, CREQ-LibVac-HandleErrors002
 */
template <typename... Args>
void HandleErrors(ara::core::ErrorCode const& error, Args&&... args) {
  detail::HandleErrorsAux<Args...>::handle(error, std::forward<Args>(args)...);
}

/* VECTOR Next Construct AutosarC++17_10-A2.11.5: MD_VAC_A2.11.5_nameOfObjectOrFunctionReused */
/*!
 * \brief  Produces an ErrorList which matches all of its contained error codes.
 * \tparam Args enum types of error codes.
 * \param  args enum values of error codes.
 * \return error list type.
 */
template <typename... Args>
constexpr auto ErrorList(Args&&... args) noexcept -> detail::ErrorList<Args...> {
  return detail::ErrorList<Args...>{std::forward<Args>(args)...};
}

}  // namespace language
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_LANGUAGE_HANDLE_ERRORS_H_
