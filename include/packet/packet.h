#ifndef PACKET_PACKET_H_
#define PACKET_PACKET_H_

#include <vector>
#include <cstdint>
#include <limits>
#include <iostream>
#include <array>
#include <arpa/inet.h>
#include <string>

#include <packet/defs.h>
#include <packet/buffer_part.h>


namespace packet {



/**
 * @brief The Packet class provides an interface for reading a packet and verify if it is
 *        valid or not, also for serializing a piece of data.
 * @tparam Cfg  The configuration to be used on the packet
 */
template<typename Cfg>
class PacketT {
  public:

    // Extraction of the configuration types here
    using data_len_t = typename Cfg::data_len_t;
    static constexpr const int HEAD_PATTERN_SIZE = LengthCalculator<staticLength(Cfg::HEAD_PATTERN)>::value;
    static constexpr const int TAIL_PATTERN_SIZE = LengthCalculator<staticLength(Cfg::TAIL_PATTERN)>::value;


  public:
    inline PacketT();

    /**
     * @brief Return the current status
     * @return the current status
     */
    inline Status
    status(void) const;

    /**
     * @brief Reset the current packet
     */
    inline void
    reset(void);

    /**
     * @brief Interface for adding data to the packet
     * @param data the raw data to be added
     * @param len the length of the data to be added
     * @return the amount of bytes added
     */
    inline std::size_t
    appendData(const byte_t* data, const std::size_t len);

    /**
     * @brief Returns the remaining buffer pointer
     * @return the remaining buffer pointer
     */
    inline byte_t*
    remainingBuffer(void);

    /**
     * @brief Returns the remaining bytes that can be added to the packet
     * @return the remaining bytes that can be added to the packet
     */
    inline std::size_t
    remainingBytes(void) const;

    /**
     * @brief If the data was added from a different interface than "appendData" (for example
     *        copying directly into the buffer), this method should be called to update
     *        the amount of data writed on the buffer
     * @param data_len_added the data writed on the buffer
     * @return the amount of bytes actually handled
     */
    inline std::size_t
    updateDataOffset(const std::size_t data_len_added);


    /**
     * @brief The amount of data of de packet without headers and extra fields. Note that
     *        we must have status == completed in order to properly have this value
     * @return the amount of data of the packet (without headers)
     */
    inline std::size_t
    dataLen(void) const;

    /**
     * @brief Returns the pointer to the data buffer of the packet, this should have dataLen()
     *        bytes
     * @return the pointer to the data buffer of the packet, this should have dataLen() bytes
     * @note that the status == COMPLETED
     */
    inline const byte_t*
    data(void) const;

    /**
     * @brief Returns the full buffer with headers, size and data. Make sure status == Completed
     * @return the full buffer with headers, size and data
     * @note take into account that status == Completed
     */
    inline const std::vector<byte_t>&
    allData(void) const;


    /**
     * @brief Generates a serialized packet from the packet data (content)
     * @param packet_content  The packet content we wanto to serialize
     * @param len             The len of the packet
     * @param out             The output buffer where we want to serialize it
     * @return true on success | false otherwise
     */
    static inline bool
    serialize(const byte_t* packet_content, const data_len_t len, std::ostream& out);
    static inline bool
    serialize(const byte_t* packet_content, const data_len_t len, std::vector<byte_t>& out);


  private:

    enum class State {
      HEAD_PATTERN = 0,
      DATA_SIZE,
      DATA,
      TAIL_PATTERN,
      NONE,
    };

  private:

    inline void
    newDataAdded(void);

    inline bool
    canAddMoreData(void) const;

    inline State
    nextState(void) const;

    inline void
    setupState(const State state);

    inline bool
    verifyCurrentStateData(void) const;

    inline std::size_t
    dataPtrIndex(void) const;

  private:
    State reading_state_;
    Status status_;
    std::vector<byte_t> buffer_;
    BufferPart buffer_part_;
    data_len_t pkt_data_len_;
    std::size_t current_data_idx_;
};



#include <packet/packet_impl.h>


// Default definition of a packet
using DefaultPacket = PacketT<DefaultConfig>;

}

#endif // PACKET_PACKET_H_
