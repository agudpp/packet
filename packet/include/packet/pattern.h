#ifndef PACKET_PATTERN_H_
#define PACKET_PATTERN_H_

#include <cstring>
#include <cstdint>
#include <array>

#include <packet/defs.h>

namespace packet {


/**
 * @brief Here we will define the interface of what a pattern should be
 *
 *  struct PatternInterface {
 *    static inline std::size_t size(void);
 *    static inline bool areEqual(const byte_t* data);
 *    static inline void copyTo(byte_t* dest)
 *  };
 */

template<unsigned int N>
static std::array<byte_t, N> buildArray(void)
{
  std::array<byte_t, N> result;
  for (unsigned int i = 0; i < N; i++) {
    result[i] = 0xAF; // random but fixed
  }
  return result;
}

template <unsigned int N>
struct SimplePattern {
    static const std::array<byte_t, N> pattern_data = buildArray<N>();
    static inline std::size_t size(void) { return N; }
    static inline bool areEqual(const byte_t* data)
    {
      for (unsigned int i = 0; i < N; i++) {
        if (pattern_data[i] != data[i]) {
          return false;
        }
      }
      return true;
    }
    static inline void copyTo(byte_t* dest)
    {
      std::memcpy(dest, pattern_data.data(), pattern_data.size());
    }
};


}

#endif // PACKET_PATTERN_H_
