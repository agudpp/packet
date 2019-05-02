#ifndef PACKET_DEFS_H_
#define PACKET_DEFS_H_

#include <cstdint>

namespace packet {

typedef std::uint8_t byte_t;
static_assert(sizeof(byte_t) == sizeof(std::uint8_t), "We assume a byte is only 8 bits");

typedef std::uint32_t data_len_t;


}

#endif // PACKET_DEFS_H_
