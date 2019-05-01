#ifndef PACKET_BUFFER_PART_H_
#define PACKET_BUFFER_PART_H_

#include <cstdint>
#include <vector>
#include <cstring>
#include <algorithm>

#include <packet/defs.h>

#include <packet/debug_helper.h>


namespace packet {

/**
 * @brief Provides an interface of a buffer using just a part of a real one
 */
class BufferPart {
  public:
    /**
     * @brief Construct it from the real buffer that must be allocated from start_idx till
     *        start_idx + size
     * @param real_buffer the real buffer that must be allocated
     * @param start_idx the starting index of the buffer part
     * @param size the size of the buffer part
     */
    BufferPart(std::vector<byte_t>* real_buffer,
               const std::size_t start_idx,
               const std::size_t size) noexcept;

    /**
     * @brief How many bytes we still need to fill for completing the buffer
     * @return the number of bytes still available
     */
    inline std::size_t
    remainingSize(void) const;

    /**
     * @brief Returns the full size of the buffer
     * @return the full size of the buffer
     */
    inline std::size_t
    fullSize(void) const;

    /**
     * @brief Check if the buffer is full or not
     * @return true if it is, false otherwise
     */
    inline bool
    isFull(void) const;

    /**
     * @brief Append family methods will be used to append more data to the buffer.
     * @param data the data to be appended
     * @param len the len of the data to be appended
     * @return true if we appended all requested, false otherwise
     * @note that we will append at maximum min(remainingSize(), len) bytes
     */
    bool
    append(const byte_t* data, const std::size_t len);
    bool
    append(const std::vector<byte_t>& data);

    /**
     * @brief Returns the buffer / remaining buffer data pointers
     * @return the buffer pointer (check fullSize() / remainingSize()) for the associated sizes
     */
    inline const byte_t*
    buffer(void) const;
    inline byte_t*
    buffer(void);

    inline byte_t*
    remainingBuffer(void);

    /**
     * @brief Provides a way to notify the buffer that we added more data into the buffer
     *        writing it directly on it (useful when reading from sockets for example)
     * @param data_len_added the number of bytes added to the buffer
     * @return true if correct, false otherwise
     */
    bool
    updateDataOffset(const std::size_t data_len_added);


  private:
    std::vector<byte_t>* real_buffer_;
    std::size_t start_idx_;
    std::size_t size_;
    std::size_t data_idx_;
};



// TODO: move all this to the buffer_impl


#include <packet/buffer_part_impl.h>
}

#endif // PACKET_BUFFER_PART_H_
