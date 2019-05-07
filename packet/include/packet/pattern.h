#ifndef PACKET_PATTERN_H_
#define PACKET_PATTERN_H_

#include <cstring>
#include <cstdint>
#include <vector>

#include <packet/defs.h>

namespace packet {


/**
 * @brief Here we will define the interface of what a pattern should be
 *
 *  struct PatternInterface {
 *    static inline std::size_t size(void);
 *    static inline bool areEqual(const byte_t* data);
 *    static inline void copyTo(byte_t* dest)
 *  };
 */
class Pattern {
  public:
    Pattern(const std::vector<byte_t>& pattern);

    inline std::size_t
    size(void) const;

    inline bool
    operator==(const byte_t* data) const;
    inline bool
    operator==(const std::vector<byte_t>& data) const;

    inline void
    copyTo(byte_t* dest) const;

    inline const byte_t*
    data(void) const;

  private:
    std::vector<byte_t> pattern_data_;
};



// TODO move to impl file
Pattern::Pattern(const std::vector<byte_t>& pattern) :
  pattern_data_(pattern)
{
}

inline std::size_t
Pattern::size(void) const
{
  return pattern_data_.size();
}

inline bool
Pattern::operator==(const byte_t* data) const
{
  return std::memcmp(data, pattern_data_.data(), size()) == 0;
}

inline bool
Pattern::operator==(const std::vector<byte_t>& data) const
{
  return (*this) == data.data();
}

inline void
Pattern::copyTo(byte_t* dest) const
{
  std::memcpy(dest, pattern_data_.data(), pattern_data_.size());
}

inline const byte_t*
Pattern::data(void) const
{
  return pattern_data_.data();
}

}

#endif // PACKET_PATTERN_H_
