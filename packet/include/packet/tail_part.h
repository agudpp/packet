#ifndef PACKET_TAIL_PART_H_
#define PACKET_TAIL_PART_H_


#include <vector>
#include <cstdint>

#include <packet/buffer_part.h>
#include <packet/part_defs.h>
#include <packet/defs.h>


namespace packet {

template <typename EndingPattern>
class TailPart {
  public:
    TailPart(std::vector<byte_t>* real_buffer, std::size_t start_idx);

    inline PartStatus
    status(void) const;

    inline BufferPart&
    bufferPart(void);

    inline const BufferPart&
    bufferPart(void) const;

  private:

    inline bool
    isValid(void) const;

  private:
    BufferPart buffer_part_;
};



// TODO: move to a impl file

template <typename EndingPattern>
inline bool
TailPart<EndingPattern>::isValid(void) const
{
  ASSERT(buffer_part_.isFull());
  return EndingPattern::areEqual(buffer_part_.buffer());
}

template <typename EndingPattern>
TailPart<EndingPattern>::TailPart(std::vector<byte_t>* real_buffer, std::size_t start_idx) :
  buffer_part_(real_buffer, start_idx, EndingPattern::size())
{
}

template <typename EndingPattern>
inline PartStatus
TailPart<EndingPattern>::status(void) const
{
  return buffer_part_.isFull()
      ? (isValid() ? PartStatus::COMPLETE : PartStatus::INVALID)
      : PartStatus::INCOMPLETE;
}

template <typename EndingPattern>
inline BufferPart&
TailPart<EndingPattern>::bufferPart(void)
{
  return buffer_part_;
}

template <typename EndingPattern>
inline const BufferPart&
TailPart<EndingPattern>::bufferPart(void) const
{
  return buffer_part_;
}


}

#endif // PACKET_TAIL_PART_H_
