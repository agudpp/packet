#ifndef PACKET_PATTERN_SECTION_HANDLER_H_
#define PACKET_PATTERN_SECTION_HANDLER_H_

#include <cstring>
#include <vector>
#include <cstdint>

#include <packet/buffer_part.h>
#include <packet/section_handler_base.h>
#include <packet/defs.h>
#include <packet/debug_helper.h>
#include <packet/pattern.h>


namespace packet {



class PatternSectionHandler : public SectionHandlerBase {
  public:
    PatternSectionHandler(std::vector<byte_t>* buffer,
                          const std::size_t section_buffer_start_idx,
                          const Pattern& pattern);

    virtual std::size_t
    bytesRead(void) const override;

  protected:

    virtual void
    allDataRead(void) override;

    // Serialize methods
    virtual std::vector<byte_t>
    serializeForDataSize(const byte_t* data, const std::size_t len) const override;



  private:
    Pattern pattern_;
    BufferPart buffer_part_;
};



// TODO move to impl file

void
PatternSectionHandler::allDataRead(void)
{
  if (pattern_ == bufferData()) {
    setStatus(SectionHandlerStatus::COMPLETE);
  } else {
    setStatus(SectionHandlerStatus::INVALID);
  }
}

// Serialize methods
std::vector<byte_t>
PatternSectionHandler::serializeForDataSize(const byte_t* data, const std::size_t len) const
{
  std::vector<byte_t> result(pattern_.size());
  pattern_.copyTo(result.data());
  return result;
}


PatternSectionHandler::PatternSectionHandler(std::vector<byte_t>* buffer,
                                             const std::size_t section_buffer_start_idx,
                                             const Pattern& pattern) :
  SectionHandlerBase(buffer, section_buffer_start_idx)
, pattern_(pattern)
{
  configureNewBuffer(section_buffer_start_idx, pattern.size());
}

std::size_t
PatternSectionHandler::bytesRead(void) const
{
  return bytesReadCurrentBuffer();
}


}

#endif // PACKET_PATTERN_SECTION_HANDLER_H_
