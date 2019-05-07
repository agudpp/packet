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
    BufferPart(void);

    /**
     * @brief Construct it from the real buffer that must be allocated from start_idx till
     *        start_idx + size
     * @param real_buffer the real buffer that must be allocated
     * @param start_idx the starting index of the buffer part
     * @param size the size of the buffer part
     * @param auto_resize flag indicating if we should resize the real buffer or not
     */
    BufferPart(std::vector<byte_t>* real_buffer,
               const std::size_t start_idx,
               const std::size_t size,
               bool auto_resize = true) noexcept;

    /**
     * @brief How many bytes we still need to fill for completing the buffer
     * @return the number of bytes still available
     */
    inline std::size_t
    remainingSize(void) const;

    /**
     * @brief Returns the size of the data readed so far for this buffer
     * @return the size of the data readed so far for this buffer
     */
    inline std::size_t
    dataSize(void) const;

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
     * @return the amount of bytes added
     * @note that we will append at maximum min(remainingSize(), len) bytes
     */
    std::size_t
    append(const byte_t* data, const std::size_t len);
    std::size_t
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
     * @brief Returns the real buffer associated to this buffer part
     * @return the real buffer pointer associated
     */
    inline std::vector<byte_t>*
    realBuffer(void);

    /**
     * @brief Provides a way to notify the buffer that we added more data into the buffer
     *        writing it directly on it (useful when reading from sockets for example)
     * @param data_len_added the number of bytes added to the buffer
     * @return amount of bytes added
     */
    std::size_t
    updateDataOffset(const std::size_t data_len_added);

    /**
     * @brief Reinterpret the current buffer as an specific type.
     * @note we assert that the data size should be >= current type casted.
     */
    template<typename T>
    const T* parseAs(void) const
    {
      ASSERT(sizeof (T) <= dataSize());
      return reinterpret_cast<const T*>(buffer());
    }


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
