#ifndef DEBUG_HELPER_H
#define DEBUG_HELPER_H


#if defined(DEBUG)

#include <cassert>
#include <iostream>
#include <sstream>

#if defined(_WIN32) || defined(CYGWIN)

#elif defined(linux) || defined(_linux) || defined(__linux) || defined(__linux__) || defined(__APPLE__)
#  ifndef _GNU_SOURCE
#    define _GNU_SOURCE
#  endif
#  include <cstring>
#else
#  error "Unsupported platform, aborting compilation."
#endif
// Relative file name string, i.e. without path.
#ifndef __FILENAME__
#   define  __FILENAME__  (basename(__FILE__))
#endif


// PKT_ASSERTions
#define PKT_ASSERT(x)   {const bool condition = (x); if(!condition){PKT_LOG_ERROR("PKT_ASSERT failed " #x "\n"); assert(false);}}
#define PKT_ASSERT_PTR(x) PKT_ASSERT(x != nullptr)

#define _PKT_LOG_SET_HEADER(buff, log_type) buff << "[" << __FILE__ << ", " << __FUNCTION__ << ", " << __LINE__ << "] " << log_type << ": "
#define _PKT_LOG_BUFF(out, log_type, x) _PKT_LOG_SET_HEADER(out, log_type); out << x << std::endl;


// logging
#define PKT_LOG_ERROR(x) _PKT_LOG_BUFF(std::cerr, "ERROR", x)

#define PKT_LOG_WARNING(x) _PKT_LOG_BUFF(std::cerr, "WARNING", x)

#define PKT_LOG_INFO(x) _PKT_LOG_BUFF(std::cout, "INFO", x)



#else

#define PKT_ASSERT(x)
#define PKT_ASSERT_PTR(x)
#define PKT_LOG_ERROR(x) _PKT_LOG_BUFF(std::cerr, "ERROR", x)
#define PKT_LOG_WARNING(x)
#define PKT_LOG_INFO(x)

#endif

#endif // DEBUG_HELPER_H
