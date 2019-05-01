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


// Assertions
#define ASSERT(x)   {const bool condition = (x); if(!condition){LOG_ERROR("Assert failed " #x "\n"); assert(false);}}
#define ASSERT_PTR(x) ASSERT(x != nullptr)

#define _LOG_SET_HEADER(buff, log_type) buff << "[" << __FILE__ << ", " << __FUNCTION__ << ", " << __LINE__ << "] " << log_type << ": "
#define _LOG_BUFF(out, log_type, x) _LOG_SET_HEADER(out, log_type); out << x << std::endl;


// logging
#define LOG_ERROR(x) _LOG_BUFF(std::cerr, "ERROR", x)

#define LOG_WARNING(x) _LOG_BUFF(std::cerr, "WARNING", x)

#define LOG_INFO(x) _LOG_BUFF(std::cout, "INFO", x)



#else

#define ASSERT(x)
#define ASSERT_PTR(x)
#define LOG_ERROR(x) _LOG_BUFF(std::cerr, "ERROR", x)
#define LOG_WARNING(x)
#define LOG_INFO(x)

#endif

#endif // DEBUG_HELPER_H
