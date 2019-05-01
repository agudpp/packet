#ifndef PACKET_PATTERN_H_
#define PACKET_PATTERN_H_

#include <cstdint>

#include <packet/defs.h>

namespace packet {

/**
 * @brief Here we will define the interface of what a pattern should be
 *
 *  struct PatternInterface {
 *    inline std::size_t size(void) const;
 *    inline bool areEqual(const byte_t* data) const;
 *  };
 */

template<unsigned int N>
SimplePattern {

}


}

#endif // PACKET_PATTERN_H_
