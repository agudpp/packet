
BufferPart::BufferPart(void) :
  real_buffer_(nullptr)
, start_idx_(0)
, size_(0)
, data_idx_(0)
{}


BufferPart::BufferPart(std::vector<byte_t>* real_buffer,
                       const std::size_t start_idx,
                       const std::size_t size,
                       bool auto_resize) noexcept :
  real_buffer_(real_buffer)
, start_idx_(start_idx)
, size_(size)
, data_idx_(start_idx)
{
  PKT_ASSERT_PTR(real_buffer_);
  if (auto_resize) {
    real_buffer_->resize(std::max(real_buffer_->size(), (start_idx_ + size_)));
  }
  PKT_ASSERT(real_buffer_->size() >= (start_idx_ + size_));
}


inline std::size_t
BufferPart::remainingSize(void) const
{
  return (start_idx_ + size_) - data_idx_;
}

inline std::size_t
BufferPart::fullSize(void) const
{
  return size_;
}

inline std::size_t
BufferPart::dataSize(void) const
{
  return data_idx_ - start_idx_;
}

inline bool
BufferPart::isFull(void) const
{
  return data_idx_ >= (start_idx_ + size_);
}

std::size_t
BufferPart::append(const byte_t* data, const std::size_t len)
{
  const std::size_t to_copy = std::min(remainingSize(), len);
  std::memcpy(remainingBuffer(), data, to_copy);
  data_idx_ += to_copy;
  return to_copy;
}

std::size_t
BufferPart::append(const std::vector<byte_t>& data)
{
  return append(data.data(), data.size());
}

inline const byte_t*
BufferPart::buffer(void) const
{
  return real_buffer_->data() + start_idx_;
}

inline byte_t*
BufferPart::buffer(void)
{
  return real_buffer_->data() + start_idx_;
}

inline byte_t*
BufferPart::remainingBuffer(void)
{
  return isFull() ? nullptr : (real_buffer_->data() + data_idx_);
}

inline std::vector<byte_t>*
BufferPart::realBuffer(void)
{
  return real_buffer_;
}

std::size_t
BufferPart::updateDataOffset(const std::size_t data_len_added)
{
  const std::size_t to_add = std::min(data_len_added, remainingSize());
  data_idx_ += data_len_added;
  return to_add;
}
