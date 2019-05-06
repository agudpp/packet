#ifndef PACKET_DATA_READER_H_
#define PACKET_DATA_READER_H_

#include <vector>

#include <packet/buffer_part.h>

namespace packet {

class DataReader {
  public:
    DataReader(const BufferPart& buffer_part);

    inline SectionHandlerStatus
    status(void) const;

    std::size_t
    appendData(const byte_t* data, const std::size_t len);

    inline byte_t*
    remainingBuffer(void);

    std::size_t
    updateDataOffset(const std::size_t data_len_added);


  private:
    BufferPart buffer_;
};

}

#endif // PACKET_DATA_READER_H_
