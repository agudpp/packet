#ifndef PACKET_DATA_SECTION_HANDLER_H_
#define PACKET_DATA_SECTION_HANDLER_H_

#include <cstring>
#include <vector>
#include <cstdint>

#include <packet/buffer_part.h>
#include <packet/section_handler_base.h>
#include <packet/defs.h>
#include <packet/debug_helper.h>


namespace packet {


class DataSectionHandler : public SectionHandlerBase {
  public:
    DataSectionHandler(const data_len_t max_data_len);

    virtual BufferPart*
    bufferPart(void) = 0;

    // return true on success
    virtual bool
    configure(const ConfigData& data) = 0;

    virtual std::size_t
    bytesReaded(void) const = 0;

    virtual SectionHandlerStatus
    status(void) const = 0;

    // Serialize methods
    virtual std::vector<byte_t>
    serializeForDataSize(const data_len_t len) const = 0;

  private:

    enum class ReadingStep {
      DATA_LEN,
      DATA,
    };

  private:

    inline data_len_t
    getLen(void) const;

    inline bool
    isValid(void) const;



  private:
    BufferPart buffer_part_;
    data_len_t max_data_len_;
    ReadingStep reading_step_;
};



// TODO: move to impl file

DataSectionHandler::DataSectionHandler(const data_len_t max_data_len) :
  max_data_len_(max_data_len)
, reading_step_(ReadingStep::DATA_LEN)
{
}

BufferPart*
DataSectionHandler::bufferPart(void)
{
  return buffer_part_.realBuffer() == nullptr ? nullptr : &buffer_part_;
}

// return true on success
bool
DataSectionHandler::configure(const ConfigData& data)
{
  ASSERT_PTR(data.real_buffer);
  buffer_part_ = BufferPart(data.real_buffer, data.start_buff_pos, sizeof(data_len_t));
  return true;
}

std::size_t
DataSectionHandler::bytesReaded(void) const
{

}

SectionHandlerStatus
DataSectionHandler::status(void) const
{

}

// Serialize methods
std::vector<byte_t>
DataSectionHandler::serializeForDataSize(const data_len_t len) const
{

}

}

#endif // PACKET_DATA_SECTION_HANDLER_H_
