#ifndef PACKET_HEADER_PART_H_
#define PACKET_HEADER_PART_H_

#include <cstring>
#include <vector>
#include <cstdint>

#include <packet/buffer_part.h>
#include <packet/part_defs.h>
#include <packet/defs.h>
#include <packet/debug_helper.h>


namespace packet {


class HeaderPart : public PartBase {
  public:
    HeaderPart(std::vector<byte_t>* real_buffer);


    virtual bool
    configure(const PartData& previous_part_data) = 0;

    // called when this is complete only
    virtual PartData
    partData(void) const = 0;

    virtual PartStatus
    status(void) const = 0;

    // Serialize methods
    virtual std::vector<byte_t>
    serializeForDataSize(const data_len_t len) const = 0;

  private:

    inline data_len_t
    getLen(void) const;

    inline bool
    isValid(void) const;



  private:
    BufferPart buffer_part_;
};



// TODO: move to impl file

template<typename StartingPattern, data_len_t MAX_DATA_SIZE>
inline data_len_t
HeaderPart<StartingPattern, MAX_DATA_SIZE>::getLen(void) const
{
  ASSERT(buffer_part_.isFull());
  // TODO: here we assume no little / big endian encoding, list of bytes can be casted to data_len_t
  // directly.
  const byte_t* data_len_addr = buffer_part_.buffer() + StartingPattern::size();
  return *reinterpret_cast<const data_len_t*>(data_len_addr);
}

template<typename StartingPattern, data_len_t MAX_DATA_SIZE>
inline bool
HeaderPart<StartingPattern, MAX_DATA_SIZE>::isValid(void) const
{
  ASSERT(buffer_part_.isFull());
  return StartingPattern::areEqual(buffer_part_.buffer()) && (getLen() <= MAX_DATA_SIZE);
}

template<typename StartingPattern, data_len_t MAX_DATA_SIZE>
HeaderPart<StartingPattern, MAX_DATA_SIZE>::HeaderPart(std::vector<byte_t>* real_buffer) :
  buffer_part_(real_buffer, 0, HeaderPart<StartingPattern, MAX_DATA_SIZE>::size())
{
}

template<typename StartingPattern, data_len_t MAX_DATA_SIZE>
inline std::size_t
HeaderPart<StartingPattern, MAX_DATA_SIZE>::size(void)
{
  return StartingPattern::size() + sizeof(data_len_t);
}

template<typename StartingPattern, data_len_t MAX_DATA_SIZE>
inline PartStatus
HeaderPart<StartingPattern, MAX_DATA_SIZE>::status(void) const
{
  return (!buffer_part_.isFull())
      ? PartStatus::INCOMPLETE
      : (isValid() ? PartStatus::COMPLETE : PartStatus::INVALID);
}

template<typename StartingPattern, data_len_t MAX_DATA_SIZE>
inline std::size_t
HeaderPart<StartingPattern, MAX_DATA_SIZE>::maxDataLen(void) const
{
  return MAX_DATA_SIZE;
}

template<typename StartingPattern, data_len_t MAX_DATA_SIZE>
inline std::size_t
HeaderPart<StartingPattern, MAX_DATA_SIZE>::dataLen(void) const
{
  return getLen();
}

template<typename StartingPattern, data_len_t MAX_DATA_SIZE>
inline BufferPart&
HeaderPart<StartingPattern, MAX_DATA_SIZE>::bufferPart(void)
{
  return buffer_part_;
}

template<typename StartingPattern, data_len_t MAX_DATA_SIZE>
inline const BufferPart&
HeaderPart<StartingPattern, MAX_DATA_SIZE>::bufferPart(void) const
{
  return buffer_part_;
}

template<typename StartingPattern, data_len_t MAX_DATA_SIZE>
inline std::vector<byte_t>
HeaderPart<StartingPattern, MAX_DATA_SIZE>::serializeForDataSize(const data_len_t len)
{
  ASSERT(len <= MAX_DATA_SIZE);
  std::vector<byte_t> result;
  result.resize(size());
  StartingPattern::copyTo(result.data());
  std::memcpy(result.data() + StartingPattern::size(),
              reinterpret_cast<const byte_t*>(&len),
              sizeof(data_len_t));
  return result;
}

}

#endif // PACKET_HEADER_PART_H_
