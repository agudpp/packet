
BufferPart::BufferPart(void) :
  real_buffer_(nullptr)
, start_idx_(0)
, size_(0)
, data_idx_(0)
{}


BufferPart::BufferPart(std::vector<byte_t>* real_buffer,
                       const std::size_t start_idx,
                       const std::size_t size) noexcept :
  real_buffer_(real_buffer)
, start_idx_(start_idx)
, size_(size)
, data_idx_(start_idx)
{
  ASSERT_PTR(real_buffer_);
  ASSERT(real_buffer_->size() >= (start_idx_ + size_));
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

inline bool
BufferPart::isFull(void) const
{
  return data_idx_ >= (start_idx_ + size_);
}

bool
BufferPart::append(const byte_t* data, const std::size_t len)
{
  const std::size_t to_copy = std::min(remainingSize(), len);
  std::memcpy(static_cast<void*>(remainingBuffer()),
              static_cast<const void*>(data),
              to_copy);
  return to_copy == len;
}

bool
BufferPart::append(const std::vector<byte_t>& data)
{
  return append(data.data(), data.size());
}

inline const byte_t*
BufferPart::buffer(void) const
{
  return &((*real_buffer_)[start_idx_]);
}

inline byte_t*
BufferPart::buffer(void)
{
  return &((*real_buffer_)[start_idx_]);
}

inline byte_t*
BufferPart::remainingBuffer(void)
{
  return isFull() ? nullptr : &((*real_buffer_)[start_idx_ + data_idx_]);
}

bool
BufferPart::updateDataOffset(const std::size_t data_len_added)
{
  const std::size_t to_add = std::min(data_len_added, remainingSize());
  data_idx_ += data_len_added;
  return to_add == data_len_added;
}
