#ifndef PACKET_DATA_PART_H_
#define PACKET_DATA_PART_H_

#include <vector>
#include <cstdint>

#include <packet/buffer_part.h>
#include <packet/part_defs.h>
#include <packet/defs.h>


namespace packet {

/**
 * @brief This class will hold the data of the
 */
class DataPart {
  public:
    DataPart(std::vector<byte_t>* real_buffer, std::size_t start_idx, std::size_t data_len);

    inline PartStatus
    status(void) const;

    inline BufferPart&
    bufferPart(void);
    inline const BufferPart&
    bufferPart(void) const;



  private:
    BufferPart buffer_part_;
};



// TODO: move to a impl file

DataPart::DataPart(std::vector<byte_t>* real_buffer, std::size_t start_idx, std::size_t data_len) :
  buffer_part_(real_buffer, start_idx, data_len)
{}

inline PartStatus
DataPart::status(void) const
{
  // cannot be invalid... is just data
  return buffer_part_.isFull() ? PartStatus::COMPLETE : PartStatus::INCOMPLETE;
}

inline BufferPart&
DataPart::bufferPart(void)
{
  return buffer_part_;
}

inline const BufferPart&
DataPart::bufferPart(void) const
{
  return buffer_part_;
}


}

#endif // PACKET_DATA_PART_H_
