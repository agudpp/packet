#ifndef PACKET_SECTION_HANDLER_BASE_H_
#define PACKET_SECTION_HANDLER_BASE_H_

#include <vector>
#include <cstdint>

#include <packet/buffer_part.h>
#include <packet/defs.h>


namespace packet {

enum class SectionHandlerStatus {
  COMPLETE,
  INCOMPLETE,
  INVALID,
};


class SectionHandlerBase {
  public:
    SectionHandlerBase(std::vector<byte_t>* buffer,
                       const std::size_t section_buffer_start_idx);
    virtual ~SectionHandlerBase(void) {}

    virtual std::size_t
    bytesRead(void) const = 0;

    inline SectionHandlerStatus
    status(void) const;

    std::size_t
    appendData(const byte_t* data, const std::size_t len);

    inline byte_t*
    remainingBuffer(void);

    std::size_t
    updateDataOffset(const std::size_t data_len_added);


  protected:

    inline std::size_t
    startSectionBufferIdx(void) const;

    void
    configureNewBuffer(const std::size_t start_idx, const std::size_t len);

    inline void
    setStatus(const SectionHandlerStatus status);

    inline std::size_t
    bytesReadCurrentBuffer(void) const;

    inline byte_t*
    bufferData(void);

    template<typename T>
    inline T*
    castToPtr(void) const
    {
      ASSERT(buffer_part_.dataSize() >= sizeof (T));
      return reinterpret_cast<T*>(buffer_part_.buffer());
    }

    virtual void
    newDataAvailable(const std::size_t new_data_size) {}

    virtual void
    allDataRead(void) {}

    // Serialize methods
    virtual std::vector<byte_t>
    serializeForDataSize(const byte_t* data, const std::size_t len) const = 0;


  private:
    std::vector<byte_t>* buffer_;
    std::size_t section_start_idx_;
    BufferPart buffer_part_;
    SectionHandlerStatus status_;
};


// TODO move this to impl file

inline std::size_t
SectionHandlerBase::startSectionBufferIdx(void) const
{
  return section_start_idx_;
}

void
SectionHandlerBase::configureNewBuffer(const std::size_t start_idx, const std::size_t len)
{
  ASSERT_PTR(buffer_);
  buffer_part_ = BufferPart(buffer_, start_idx, len);
}

inline void
SectionHandlerBase::setStatus(const SectionHandlerStatus status)
{
  status_ = status;
}

inline std::size_t
SectionHandlerBase::bytesReadCurrentBuffer(void) const
{
  return buffer_part_.dataSize();
}

inline byte_t*
SectionHandlerBase::bufferData(void)
{
  return buffer_part_.buffer();
}


SectionHandlerBase::SectionHandlerBase(std::vector<byte_t>* buffer,
                                       const std::size_t section_buffer_start_idx) :
  buffer_(buffer)
, section_start_idx_(section_buffer_start_idx)
, status_(SectionHandlerStatus::INCOMPLETE)
{
}


inline SectionHandlerStatus
SectionHandlerBase::status(void) const
{
  return status_;
}

std::size_t
SectionHandlerBase::appendData(const byte_t* data, const std::size_t len)
{
  const std::size_t appended_len = buffer_part_.append(data, len);
  if (buffer_part_.isFull()) {
    allDataRead();
  } else {
    newDataAvailable(appended_len);
  }
  return appended_len;
}

inline byte_t*
SectionHandlerBase::remainingBuffer(void)
{
  return buffer_part_.remainingBuffer();
}

std::size_t
SectionHandlerBase::updateDataOffset(const std::size_t data_len_added)
{
  const std::size_t appended_len = buffer_part_.updateDataOffset(data_len_added);
  if (buffer_part_.isFull()) {
    allDataRead();
  } else {
    newDataAvailable(appended_len);
  }
  return appended_len;
}


}

#endif // PACKET_PART_DEFS_H_
