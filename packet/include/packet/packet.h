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



// TODO: move this to impl file


void
Packet::newDataAdded(void)
{
  if (!canAddMoreData()) {
    return;
  }
  if (!buffer_part_.isFull()) {
    return;
  }
  current_data_idx_ += buffer_part_.dataSize();

  if (!verifyCurrentStateData()) {
    LOG_ERROR("packet is not valid for state " << int(reading_state_));
    reading_state_ = State::NONE;
    status_ = Status::INVALID;
  } else {
    setupState(nextState());
    if (reading_state_ == State::NONE) {
      // we finish
      status_ = Status::COMPLETE;
    }
  }
}

inline bool
Packet::canAddMoreData(void) const
{
  return reading_state_ != State::NONE;
}

inline Packet::State
Packet::nextState(void) const
{
  switch (reading_state_) {
    case State::HEAD_PATTERN: return State::DATA_SIZE;
    case State::DATA_SIZE: return State::DATA;
    case State::DATA: return tail_pattern_.size() > 0 ? State::TAIL_PATTERN : State::NONE;
    case State::TAIL_PATTERN: return State::NONE;
    default:
      ASSERT(false && "invalid state");
  }
  return State::NONE;
}


void
Packet::setupState(const State state)
{
  reading_state_ = state;
  switch (reading_state_) {
    case State::HEAD_PATTERN: {
      buffer_part_ = BufferPart(&buffer_, 0, head_pattern_.size());
      break;
    }
    case State::DATA_SIZE: {
      buffer_part_ = BufferPart(&buffer_, current_data_idx_, sizeof(data_len_t));
      break;
    }
    case State::DATA: {
      pkt_data_len_ = (*buffer_part_.parseAs<data_len_t>());
      buffer_part_ = BufferPart(&buffer_, current_data_idx_, pkt_data_len_);
      break;
    }
    case State::TAIL_PATTERN: {
      buffer_part_ = BufferPart(&buffer_, current_data_idx_, tail_pattern_.size());
      break;
    }
    case State::NONE: {
      buffer_part_ = BufferPart();
      break;
    }
  }
}

bool
Packet::verifyCurrentStateData(void) const
{
  switch (reading_state_) {
    case State::HEAD_PATTERN: return head_pattern_ == buffer_part_.buffer();
    case State::DATA_SIZE: return (*buffer_part_.parseAs<data_len_t>()) <= max_len_;
    case State::DATA: return true;
    case State::TAIL_PATTERN: return tail_pattern_ == buffer_part_.buffer();
    default:
      ASSERT(false && "invalid state");
  }
  return false;
}

inline std::size_t
Packet::dataPtrIndex(void) const
{
  return head_pattern_.size() + sizeof(data_len_t);
}


Packet::Packet(const data_len_t max_data_len,
               const Pattern& head_pattern,
               const Pattern& tail_pattern) :
  status_(Status::INCOMPLETE)
, max_len_(max_data_len)
, pkt_data_len_(0)
, head_pattern_(head_pattern)
, tail_pattern_(tail_pattern)
, current_data_idx_(0)
{
  setupState(head_pattern.size() > 0 ? State::HEAD_PATTERN : State::DATA_SIZE);
}


inline Packet::Status
Packet::status(void) const
{
  return status_;
}

std::size_t
Packet::appendData(const byte_t* data, const std::size_t len)
{
  ASSERT_PTR(data);
  const std::size_t result = buffer_part_.append(data, len);
  newDataAdded();
  return result;
}

inline byte_t*
Packet::remainingBuffer(void)
{
  return buffer_part_.remainingBuffer();
}

inline std::size_t
Packet::remainingBytes(void) const
{
  return buffer_part_.remainingSize();
}

std::size_t
Packet::updateDataOffset(const std::size_t data_len_added)
{
  const std::size_t result = buffer_part_.updateDataOffset(data_len_added);
  newDataAdded();
  return result;
}

inline std::size_t
Packet::dataLen(void) const
{
  return pkt_data_len_;
}

// returns the data ptr of size dataLen()
inline const byte_t*
Packet::data(void) const
{
  return dataLen() == 0 ? nullptr : &(buffer_[dataPtrIndex()]);
}

bool
Packet::serialize(const byte_t* data, const data_len_t len, std::ostream& out) const
{
  ASSERT_PTR(data);
  if (head_pattern_.size() > 0) {
    out.write(reinterpret_cast<const char*>(head_pattern_.data()), head_pattern_.size());
  }
  // TODO: write the size properly here, little / big endian
  out.write(reinterpret_cast<const char*>(&len), sizeof(data_len_t));

  out.write(reinterpret_cast<const char*>(data), len);

  if (tail_pattern_.size() > 0) {
    out.write(reinterpret_cast<const char*>(tail_pattern_.data()), tail_pattern_.size());
  }
  return true;
}


}

#endif // PACKET_PACKET_H_
