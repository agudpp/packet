


template<typename Cfg>
inline void
PacketT<Cfg>::newDataAdded(void)
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

template<typename Cfg>
inline bool
PacketT<Cfg>::canAddMoreData(void) const
{
  return reading_state_ != State::NONE;
}

template<typename Cfg>
inline typename PacketT<Cfg>::State
PacketT<Cfg>::nextState(void) const
{
  switch (reading_state_) {
    case State::HEAD_PATTERN: return State::DATA_SIZE;
    case State::DATA_SIZE: return State::DATA;
    case State::DATA: return (TAIL_PATTERN_SIZE > 0 ? State::TAIL_PATTERN : State::NONE);
    case State::TAIL_PATTERN: return State::NONE;
    default:
      PKT_ASSERT(false && "invalid state");
  }
  return State::NONE;
}

template<typename Cfg>
inline void
PacketT<Cfg>::setupState(const State state)
{
  reading_state_ = state;
  switch (reading_state_) {
    case State::HEAD_PATTERN: {
      buffer_part_ = BufferPart(&buffer_, 0, HEAD_PATTERN_SIZE);
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
      buffer_part_ = BufferPart(&buffer_, current_data_idx_, TAIL_PATTERN_SIZE);
      break;
    }
    case State::NONE: {
      buffer_part_ = BufferPart();
      break;
    }
  }
}

template<typename Cfg>
inline bool
PacketT<Cfg>::verifyCurrentStateData(void) const
{
  switch (reading_state_) {
    case State::HEAD_PATTERN: return std::memcmp(Cfg::HEAD_PATTERN, buffer_part_.buffer(), std::min(std::size_t(HEAD_PATTERN_SIZE), buffer_part_.dataSize())) == 0;
    case State::DATA_SIZE: {
      const data_len_t len = ntohl(*buffer_part_.parseAs<data_len_t>());
      return len <= Cfg::MAX_DATA_LEN;
    }
    case State::DATA: return true;
    case State::TAIL_PATTERN: return std::memcmp(Cfg::TAIL_PATTERN, buffer_part_.buffer(), std::min(std::size_t(TAIL_PATTERN_SIZE), buffer_part_.dataSize())) == 0;
    default:
      PKT_ASSERT(false && "invalid state");
  }
  return false;
}

template<typename Cfg>
inline std::size_t
PacketT<Cfg>::dataPtrIndex(void) const
{
  return HEAD_PATTERN_SIZE + sizeof(data_len_t);
}


template<typename Cfg>
inline PacketT<Cfg>::PacketT() :
  status_(Status::INCOMPLETE)
, pkt_data_len_(0)
, current_data_idx_(0)
{
  setupState(HEAD_PATTERN_SIZE > 0 ? State::HEAD_PATTERN : State::DATA_SIZE);
}


template<typename Cfg>
inline Status
PacketT<Cfg>::status(void) const
{
  return status_;
}

template<typename Cfg>
inline void
PacketT<Cfg>::reset(void)
{
  status_ = Status::INCOMPLETE;
  pkt_data_len_ = 0;
  current_data_idx_ = 0;
  buffer_.clear();
  setupState(HEAD_PATTERN_SIZE > 0 ? State::HEAD_PATTERN : State::DATA_SIZE);
}

template<typename Cfg>
inline std::size_t
PacketT<Cfg>::appendData(const byte_t* data, const std::size_t len)
{
  PKT_ASSERT_PTR(data);
  const std::size_t result = buffer_part_.append(data, len);
  newDataAdded();
  return result;
}

template<typename Cfg>
inline byte_t*
PacketT<Cfg>::remainingBuffer(void)
{
  return buffer_part_.remainingBuffer();
}

template<typename Cfg>
inline std::size_t
PacketT<Cfg>::remainingBytes(void) const
{
  return buffer_part_.remainingSize();
}

template<typename Cfg>
inline std::size_t
PacketT<Cfg>::updateDataOffset(const std::size_t data_len_added)
{
  const std::size_t result = buffer_part_.updateDataOffset(data_len_added);
  newDataAdded();
  return result;
}

template<typename Cfg>
inline std::size_t
PacketT<Cfg>::dataLen(void) const
{
  return pkt_data_len_;
}

// returns the data ptr of size dataLen()

template<typename Cfg>
inline const byte_t*
PacketT<Cfg>::data(void) const
{
  return dataLen() == 0 ? nullptr : &(buffer_[dataPtrIndex()]);
}

template<typename Cfg>
inline bool
PacketT<Cfg>::serialize(const byte_t* packet_content, const data_len_t len, std::ostream& out)
{
  if (packet_content == nullptr || len == 0 || len > Cfg::MAX_DATA_LEN) {
      return false;
  }
  if (HEAD_PATTERN_SIZE > 0) {
    out.write(Cfg::HEAD_PATTERN, HEAD_PATTERN_SIZE);
  }

  const data_len_t wire_len = htonl(len);
  out.write(reinterpret_cast<const char*>(&wire_len), sizeof(data_len_t));
  out.write(reinterpret_cast<const char*>(packet_content), len);

  if (TAIL_PATTERN_SIZE > 0) {
    out.write(Cfg::TAIL_PATTERN, HEAD_PATTERN_SIZE);
  }
  return true;
}

template<typename Cfg>
inline bool
PacketT<Cfg>::serialize(const byte_t* packet_content, const data_len_t len, std::vector<byte_t>& out)
{
    out.clear();
    if (packet_content == nullptr || len == 0 || len > Cfg::MAX_DATA_LEN) {
        return false;
    }
    const std::size_t total_size = HEAD_PATTERN_SIZE + TAIL_PATTERN_SIZE + sizeof(data_len_t) + len;
    out.resize(total_size);
    BufferPart buffer(&out, 0, total_size, true);

    if (HEAD_PATTERN_SIZE > 0) {
      buffer.append(reinterpret_cast<const byte_t*>(Cfg::HEAD_PATTERN), HEAD_PATTERN_SIZE);
    }

    const data_len_t wire_len = htonl(len);
    buffer.append(reinterpret_cast<const byte_t*>(&wire_len), sizeof(data_len_t));
    buffer.append(packet_content, len);

    if (TAIL_PATTERN_SIZE > 0) {
      buffer.append(reinterpret_cast<const byte_t*>(Cfg::TAIL_PATTERN), HEAD_PATTERN_SIZE);
    }
    return true;
}



