#ifndef PACKET_PACKET_H_
#define PACKET_PACKET_H_

#include <vector>
#include <cstdint>

#include <packet/defs.h>
#include <packet/buffer_part.h>
#include <packet/header_part.h>
#include <packet/data_part.h>
#include <packet/tail_part.h>


namespace packet {

template <typename StartingPattern, data_len_t MAX_DATA_LEN, typename EndingPattern>
class Packet {
  public:
    enum class State {
      HEADER = 0,
      DATA,
      TAIL,
      INVALID,
      COMPLETED,
    };

    typedef HeaderPart<StartingPattern, MAX_DATA_LEN>   HeaderPartType;
    typedef DataPart                                    DataPartType;
    typedef TailPart<EndingPattern>                     TailPartType;

  public:

    Packet(void);

    // get buffer to fill (nullptr -> if finished (valid|invalid)) else a bufferpart valid
    BufferPart*
    bufferToFill(void);

    // check if finished, either invalid or valid and complete
    inline bool
    finished(void) const;

    inline State
    currentState(void) const;

  private:

    inline void
    setupHeaderPart(void);

    inline void
    setupDataPart(void);

    inline void
    setupTailPart(void);

  private:
    State reading_state_;
    std::vector<byte_t> buffer_;
    HeaderPartType header_part_;
    DataPartType data_part_;
    TailPartType tail_part_;

};

}

#endif // PACKET_PACKET_H_
