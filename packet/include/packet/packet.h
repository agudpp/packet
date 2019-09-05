#ifndef PACKET_PACKET_H_
#define PACKET_PACKET_H_

#include <vector>
#include <cstdint>
#include <limits>
#include <iostream>

#include <packet/defs.h>
#include <packet/buffer_part.h>
#include <packet/pattern.h>


namespace packet {

/**
 * @brief The Packet class provides an interface for reading a packet and verify if it is
 *        valid or not, also for serializing a piece of data.
 */
class Packet {
  public:
    enum class Status {
      INCOMPLETE,
      INVALID,
      COMPLETE
    };

  public:
    Packet(const data_len_t max_data_len = std::numeric_limits<data_len_t>::max(),
           const Pattern& head_pattern = std::vector<byte_t>{0xAF, 0xFA},
           const Pattern& tail_pattern = std::vector<byte_t>{0xFF});

    inline Status
    status(void) const;

    std::size_t
    appendData(const byte_t* data, const std::size_t len);

    inline byte_t*
    remainingBuffer(void);

    inline std::size_t
    remainingBytes(void) const;

    std::size_t
    updateDataOffset(const std::size_t data_len_added);


    // returns the packet data, we need to have status == completed
    inline std::size_t
    dataLen(void) const;

    // returns the data ptr of size dataLen()
    inline const byte_t*
    data(void) const;

    // make sure Status == Completed! otherwise this data is not complete..
    const std::vector<byte_t>&
    allData(void) const;

    bool
    serialize(const byte_t* data, const data_len_t len, std::ostream& out) const;


  private:

    enum class State {
      HEAD_PATTERN = 0,
      DATA_SIZE,
      DATA,
      TAIL_PATTERN,
      NONE,
    };

  private:

    void
    newDataAdded(void);

    inline bool
    canAddMoreData(void) const;

    inline State
    nextState(void) const;

    void
    setupState(const State state);

    bool
    verifyCurrentStateData(void) const;

    inline std::size_t
    dataPtrIndex(void) const;

  private:
    State reading_state_;
    Status status_;
    std::vector<byte_t> buffer_;
    BufferPart buffer_part_;
    data_len_t max_len_;
    data_len_t pkt_data_len_;
    Pattern head_pattern_;
    Pattern tail_pattern_;
    std::size_t current_data_idx_;
};



#include <packet/packet_impl.h>

}

#endif // PACKET_PACKET_H_
