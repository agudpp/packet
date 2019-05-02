#ifndef PACKET_PART_DEFS_H_
#define PACKET_PART_DEFS_H_

#include <vector>
#include <cstdint>

#include <packet/buffer_part.h>
#include <packet/defs.h>


namespace packet {

enum class PartStatus {
  COMPLETE,
  INCOMPLETE,
  INVALID,
};

/**
 * @brief Contains information to configure a Part based on previos part
 */
struct PartData {
    PartData(std::vector<byte_t>* the_real_buffer,
             std::size_t the_start_idx = 0,
             std::size_t the_len = 0,
             std::size_t the_next_len = 0) :
      real_buffer(the_real_buffer)
    , start_idx(the_start_idx)
    , len(the_len)
    , next_len(the_next_len)
    {}

    std::vector<byte_t>* real_buffer;
    std::size_t start_idx;
    std::size_t len;
    std::size_t next_len;
};


class PartBase {
  public:
    virtual ~PartBase(void) {}

    inline BufferPart&
    bufferPart(void);

    inline const BufferPart&
    bufferPart(void) const;

    // return true on success
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

  protected:
    BufferPart buffer_part_;
};



// TODO move to impl
inline BufferPart&
PartBase::bufferPart(void)
{
  return buffer_part_;
}

inline const BufferPart&
PartBase::bufferPart(void) const
{
  return buffer_part_;
}

}

#endif // PACKET_PART_DEFS_H_
