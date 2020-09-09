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
/*!        \file  ara/core/posix_error_domain.h
 *        \brief  Error code enum, exception and domain class for AUTOSAR PosixErrorDomain.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_ARA_CORE_POSIX_ERROR_DOMAIN_H_
#define LIB_VAC_INCLUDE_ARA_CORE_POSIX_ERROR_DOMAIN_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <algorithm>
#include <cerrno>
#include <cstring>
#include <iterator>
#include <limits>

#include "ara/core/error_code.h"
#include "ara/core/error_domain.h"
#include "ara/core/exception.h"
#include "vac/language/throw_or_terminate.h"

namespace ara {
namespace core {

/*!
 * \brief POSIX error code enum.
 * \vpublic
 */
enum class PosixErrc : ErrorDomain::CodeType {
  address_family_not_supported = 97,       /*! \brief address family not supported (EAFNOSUPPORT) */
  address_in_use = 98,                     /*! \brief address in use (EADDRINUSE) */
  address_not_available = 99,              /*! \brief address not available (EADDRNOTAVAIL) */
  already_connected = 106,                 /*! \brief already conntected (EISCONN) */
  argument_list_too_long = 7,              /*! \brief argument list too long (E2BIG) */
  argument_out_of_domain = 33,             /*! \brief argument out of domain (EDOM) */
  bad_address = 14,                        /*! \brief bad address (EFAULT) */
  bad_file_descriptor = 9,                 /*! \brief bad file descriptor (EBADF) */
  bad_message = 74,                        /*! \brief bad message (EBADMSG) */
  broken_pipe = 32,                        /*! \brief broken pipe (EPIPE) */
  connection_aborted = 103,                /*! \brief connection aborted (ECONNABORTED) */
  connection_already_in_progress = 114,    /*! \brief connection already in progress (EALREADY) */
  connection_refused = 111,                /*! \brief connection refused (ECONNREFUSED) */
  connection_reset = 104,                  /*! \brief connection reset (ECONNRESET) */
  cross_device_link = 18,                  /*! \brief cross-device link (EXDEV) */
  destination_address_required = 89,       /*! \brief destination address required (EDESTADDRREQ) */
  device_or_resource_busy = 16,            /*! \brief device or resource busy (EBUSY) */
  directory_not_empty = 39,                /*! \brief directory not empty (ENOTEMPTY) */
  executable_format_error = 8,             /*! \brief executable format error (ENOEXEC) */
  file_exists = 17,                        /*! \brief file exists (EEXIST) */
  file_too_large = 27,                     /*! \brief file too large (EFBIG) */
  filename_too_long = 36,                  /*! \brief filename too long (ENAMETOOLONG) */
  function_not_supported = 38,             /*! \brief function not supported (ENOSYS) */
  host_unreachable = 113,                  /*! \brief host unreachable (EHOSTUNREACH) */
  identifier_removed = 43,                 /*! \brief identifier removed (EIDRM) */
  illegal_byte_sequence = 84,              /*! \brief illegal byte sequence (EILSEQ) */
  inappropriate_io_control_operation = 25, /*! \brief inappropriate io control operation (ENOTTY) */
  interrupted = 4,                         /*! \brief interrupted (EINTR) */
  invalid_argument = 22,                   /*! \brief invalid argument (EINVAL) */
  invalid_seek = 29,                       /*! \brief invalid seek (ESPIPE) */
  io_error = 5,                            /*! \brief I/O error (EIO) */
  is_a_directory = 21,                     /*! \brief is a directory (EISDIR) */
  message_size = 90,                       /*! \brief message size (EMSGSIZE) */
  network_down = 100,                      /*! \brief network down (ENETDOWN) */
  network_reset = 102,                     /*! \brief network reset (ENETRESET) */
  network_unreachable = 101,               /*! \brief network unreachable (ENETUNREACH) */
  no_buffer_space = 105,                   /*! \brief no buffer space (ENOBUFS) */
  no_child_process = 10,                   /*! \brief no child process (ECHILD) */
  no_link = 67,                            /*! \brief no link (ENOLINK) */
  no_lock_available = 37,                  /*! \brief no lock available (ENOLCK) */
  no_message_available = 61,               /*! \brief no message available (ENODATA)  */
  no_message = 42,                         /*! \brief no message (ENOMSG) */
  no_protocol_option = 92,                 /*! \brief no protocol option (ENOPROTOOPT) */
  no_space_on_device = 28,                 /*! \brief no space on device (ENOSPC) */
  no_stream_resources = 63,                /*! \brief no stream resources (ENOSR) */
  no_such_device_or_address = 6,           /*! \brief no such device or address (ENXIO) */
  no_such_device = 19,                     /*! \brief no such device (ENODEV) */
  no_such_file_or_directory = 2,           /*! \brief no such file or directory (ENOENT) */
  no_such_process = 3,                     /*! \brief no such process (ESRCH) */
  not_a_directory = 20,                    /*! \brief not a directory (ENOTDIR) */
  not_a_socket = 88,                       /*! \brief not a socket (ENOTSOCK) */
  not_a_stream = 60,                       /*! \brief not a stream (ENOSTR) */
  not_connected = 107,                     /*! \brief not connected (ENOTCONN) */
  not_enough_memory = 12,                  /*! \brief not enough memory (ENOMEM) */
  not_supported = 202,                     /*! \brief not supported (ENOTSUP) */
  operation_canceled = 125,                /*! \brief operation canceled (ECANCELED) */
  operation_in_progress = 115,             /*! \brief operation in progress (EINPROGRESS) */
  operation_not_permitted = 1,             /*! \brief operation not permitted (EPERM) */
  operation_not_supported = 95,            /*! \brief operation not supported (EOPNOTSUPP) */
  operation_would_block = 201,             /*! \brief operation would block (EWOULDBLOCK) */
  owner_dead = 130,                        /*! \brief owner dead (EOWNERDEAD) */
  permission_denied = 13,                  /*! \brief permission denied (EACCES) */
  protocol_error = 71,                     /*! \brief protocol error (EPROTO) */
  protocol_not_supported = 93,             /*! \brief protocol not supported (EPROTONOSUPPORT) */
  read_only_file_system = 30,              /*! \brief read-only file system (EROFS) */
  resource_deadlock_would_occur = 35,      /*! \brief resource deadlock would occur (EDEADLK) */
  resource_unavailable_try_again = 11,     /*! \brief resource unavailable, try again (EAGAIN) */
  result_out_of_range = 34,                /*! \brief result out of range (ERANGE) */
  state_not_recoverable = 131,             /*! \brief state not recoverable (ENOTRECOVERABLE) */
  stream_timeout = 62,                     /*! \brief stream timeout (ETIME) */
  text_file_busy = 26,                     /*! \brief text file busy (ETXTBSY) */
  timed_out = 110,                         /*! \brief timed out (ETIMEDOUT) */
  too_many_files_open_in_system = 23,      /*! \brief too many files open in system (ENFILE) */
  too_many_files_open = 24,                /*! \brief too many files open (EMFILE) */
  too_many_links = 31,                     /*! \brief too many links (EMLINK) */
  too_many_symbolic_link_levels = 40,      /*! \brief too many symbolic link levels (ELOOP) */
  value_too_large = 75,                    /*! \brief value too large (EOVERFLOW) */
  wrong_protocol_type = 91,                /*! \brief wrong protocol type (EPROTOTYPE) */
};

/*!
 * \brief Exception type for PosixErrorDomain.
 * \vpublic
 */
class PosixException final : public Exception {
 public:
  /*!
   * \brief Inheriting constructor.
   * \vpublic
   */
  using Exception::Exception;
};

/*!
 * \brief Error domain for POSIX system errors.
 * \vpublic
 */
class PosixErrorDomain final : public ErrorDomain {
  /*!
   * \brief ID of the domain.
   */
  static constexpr ErrorDomain::IdType kId{0x8000000000000012U};

 public:
  /*!
   * \brief Error code enum of the domain.
   */
  using Errc = PosixErrc;

  /*!
   * \brief Exception type of the domain.
   */
  using Exception = PosixException;

  /*!
   * \brief Constructor. Initializes the domain id with PosixErrorDomain::kId.
   * \vpublic
   */
  constexpr PosixErrorDomain() noexcept : ErrorDomain(kId) {}

  /* VECTOR Disable AutosarC++17_10-A3.9.1: MD_VAC_A3.9.1_useOfBasetypeOutsideTypedef */
  /*!
   * \copybrief ErrorDomain::Name()
   * \return    "PosixError".
   * \vpublic
   */
  char const* Name() const noexcept final { return "PosixError"; }

  /*!
   * \copydoc ErrorDomain::Message()
   * \param   error_code The domain-specific error code.
   * \vpublic
   */
  char const* Message(ErrorDomain::CodeType error_code) const noexcept final {
    static_assert((std::numeric_limits<ErrorDomain::CodeType>::max() <= std::numeric_limits<int>::max()) &&
                      (std::numeric_limits<ErrorDomain::CodeType>::min() >= std::numeric_limits<int>::min()),
                  "Narrowing conversion!");
    return std::strerror(error_code);
  }

  /*!
   * \brief  Throws the given errorCode as Exception.
   * \param  error_code Error code to be thrown.
   * \throw  PosixException for all error codes.
   * \remark If the is compiled without exceptions, this function call will terminate.
   * \vpublic
   */
  [[noreturn]] void ThrowAsException(ErrorCode const& error_code) const noexcept(false) final {
    vac::language::ThrowOrTerminate<PosixException>(error_code);
  }
};

namespace internal {
/* VECTOR Next Construct AutosarC++17_10-M3.4.1: MD_VAC_M3.4.1_symbolsCanBeDeclaredLocally */
/*!
 * \brief Global PosixErrorDomain instance.
 * \vprivate
 */
constexpr PosixErrorDomain g_posixErrorDomain;
}  // namespace internal

/*!
 * \brief  Returns a reference to the global PosixErrorDomain instance.
 * \return PosixErrorDomain instance.
 * \vpublic
 */
inline constexpr ErrorDomain const& GetPosixDomain() noexcept { return internal::g_posixErrorDomain; }

/*!
 * \brief  Creates an ErrorCode from the PosixErrorDomain.
 * \param  code error code enum value
 * \param  data support data.
 * \param  message user message.
 * \return the created error code.
 * \vpublic
 */
inline constexpr ErrorCode MakeErrorCode(PosixErrorDomain::Errc code, ErrorDomain::SupportDataType data,
                                         char const* message) noexcept {
  return ErrorCode(static_cast<ErrorDomain::CodeType>(code), GetPosixDomain(), data, message);
}
/* VECTOR Enable AutosarC++17_10-A3.9.1: MD_VAC_A3.9.1_useOfBasetypeOutsideTypedef */

}  // namespace core
}  // namespace ara

/* VECTOR Next Construct AutosarC++17_10-M16.0.1: MD_VAC_M16.0.1_includePreceded */
/*!
 * \brief Need to include this here as some other modules using it through include by chance,
 *        and those modules were able to compile/build successfully before moving PosixErrorDomain
 *        from vac::language to ara::core.
 */
#include "vac/language/posix_error_domain.h"
#endif  // LIB_VAC_INCLUDE_ARA_CORE_POSIX_ERROR_DOMAIN_H_
