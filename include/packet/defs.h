#ifndef PACKET_DEFS_H_
#define PACKET_DEFS_H_

#include <string>
#include <limits>
#include <cstdint>

namespace packet {

typedef std::uint8_t byte_t;
static_assert(sizeof(byte_t) == sizeof(std::uint8_t), "We assume a byte is only 8 bits");


/**
 * @brief length calculates the length of a static string
 * @param str   The string
 * @return the length
 */
int constexpr
staticLength(const char* str)
{
    return *str ? 1 + staticLength(str + 1) : 0;
}

template <int N>
struct LengthCalculator {
    static constexpr int value = N;
};


/**
 * @brief The packet configuration and definitions. This struct will be used by the Packet
 *        to understand what data types should be used
 */
template <  typename HeadPatternType,
            typename TailPatternType,
            typename DataLenT,
            std::size_t MaxDataLen
          >
struct ConfigT {
    /**
     * @brief HEAD_PATTERN will be appended at the beginning of each packet to be able
     *                     to (early) identify invalid packets
     */
    static constexpr const char* HEAD_PATTERN = HeadPatternType::value;
    /**
     * @brief TAIL_PATTERN Similar than head pattern but in the end of the packet
     */
    static constexpr const char* TAIL_PATTERN = TailPatternType::value;
    /**
     * @brief data_len_t The data length type to be used, note that for now we use htonl and
     *                   ntohl to serialize the size of the packets
     */
    using data_len_t = DataLenT;
    /**
     * @brief MAX_DATA_LEN Maximum data we are allowed to send on the packets
     */
    static constexpr data_len_t MAX_DATA_LEN = MaxDataLen;
};

struct DefaultStartPattern { static constexpr const char* value = "<"; };
struct DefaultEndPattern { static constexpr const char* value = ">"; };

struct DefaultConfig : ConfigT <DefaultStartPattern, DefaultEndPattern, std::uint32_t, std::numeric_limits<std::uint32_t>::max()> {
};

}

#endif // PACKET_DEFS_H_
