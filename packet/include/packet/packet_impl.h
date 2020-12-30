



inline void
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
    PKT_LOG_ERROR("packet is not valid for state " << int(reading_state_));
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
    case State::DATA: return (sizeof(TAIL_PATTERN) > 0 ? State::TAIL_PATTERN : State::NONE);
    case State::TAIL_PATTERN: return State::NONE;
    default:
      PKT_ASSERT(false && "invalid state");
  }
  return State::NONE;
}


inline void
Packet::setupState(const State state)
{
  reading_state_ = state;
  switch (reading_state_) {
    case State::HEAD_PATTERN: {
      buffer_part_ = BufferPart(&buffer_, 0, sizeof(HEAD_PATTERN));
      break;
    }
    case State::DATA_SIZE: {
      buffer_part_ = BufferPart(&buffer_, current_data_idx_, sizeof(data_len_t));
      break;
    }
    case State::DATA: {
      pkt_data_len_ = ntohl((*buffer_part_.parseAs<data_len_t>()));
      buffer_part_ = BufferPart(&buffer_, current_data_idx_, pkt_data_len_);
      break;
    }
    case State::TAIL_PATTERN: {
      buffer_part_ = BufferPart(&buffer_, current_data_idx_, sizeof(TAIL_PATTERN));
      break;
    }
    case State::NONE: {
      buffer_part_ = BufferPart();
      break;
    }
  }
}

inline bool
Packet::verifyCurrentStateData(void) const
{
  switch (reading_state_) {
    case State::HEAD_PATTERN: return std::memcmp(&HEAD_PATTERN, buffer_part_.buffer(), std::min(sizeof(HEAD_PATTERN), buffer_part_.dataSize())) == 0;
    case State::DATA_SIZE: return (*buffer_part_.parseAs<data_len_t>()) <= MAX_DATA_LEN;
    case State::DATA: return true;
    case State::TAIL_PATTERN: return std::memcmp(&TAIL_PATTERN, buffer_part_.buffer(), std::min(sizeof(TAIL_PATTERN), buffer_part_.dataSize())) == 0;
    default:
      PKT_ASSERT(false && "invalid state");
  }
  return false;
}

inline std::size_t
Packet::dataPtrIndex(void) const
{
  return sizeof(HEAD_PATTERN) + sizeof(data_len_t);
}


inline Packet::Packet() :
  status_(Status::INCOMPLETE)
, pkt_data_len_(0)
, current_data_idx_(0)
{
  setupState(sizeof(HEAD_PATTERN) > 0 ? State::HEAD_PATTERN : State::DATA_SIZE);
}


inline Packet::Status
Packet::status(void) const
{
  return status_;
}

inline void
Packet::reset(void)
{
  status_ = Status::INCOMPLETE;
  pkt_data_len_ = 0;
  current_data_idx_ = 0;
  buffer_.clear();
  setupState(sizeof(HEAD_PATTERN) > 0 ? State::HEAD_PATTERN : State::DATA_SIZE);
}

inline std::size_t
Packet::appendData(const byte_t* data, const std::size_t len)
{
  PKT_ASSERT_PTR(data);
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

inline std::size_t
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

inline bool
Packet::serialize(const byte_t* data, const data_len_t len, std::ostream& out)
{
  PKT_ASSERT_PTR(data);
  if (sizeof(HEAD_PATTERN) > 0) {
    out.write(reinterpret_cast<const char*>(&HEAD_PATTERN), sizeof(HEAD_PATTERN));
  }
  // TODO: write the size properly here, little / big endian
  const data_len_t wire_len = htonl(len);
  out.write(reinterpret_cast<const char*>(&wire_len), sizeof(data_len_t));
  out.write(reinterpret_cast<const char*>(data), len);

  if (sizeof(TAIL_PATTERN) > 0) {
    out.write(reinterpret_cast<const char*>(&TAIL_PATTERN), sizeof(TAIL_PATTERN));
  }
  return true;
}



