#ifndef PACKET_TEST_HELPERS_HPP
#define PACKET_TEST_HELPERS_HPP

// std
#include <vector>
#include <string>
#include <iostream>
// packet
#include <packet/packet.h>


// assert for testing
#define TEST_ASSERT(x)   {\
    const bool condition = (x);\
    if(!condition){\
        std::cerr << "[" << __FILE__ << ", " << __FUNCTION__ << ", " << __LINE__ << "] "\
                   "TEST_ASSERT failed " #x "\n";\
        assert(false);\
    }\
}



/**
 * @brief splitStr
 * @param str
 * @param partSize
 * @return
 */
static inline std::vector<std::string>
splitStr(const std::string& str, unsigned int partSize)
{
  std::vector<std::string> result;
  unsigned int start = 0;
  while (start < str.size()) {
    result.push_back(str.substr(start, partSize));
    start += partSize;
  }
  return result;
}

/**
 * @brief Reads a packet part from a buffer containing the serialiazed data
 * @param serializedData    The serialized data
 * @param pkt               The packet to be filled in with the data
 */
template<typename PacketType>
static inline void
readPacketPart(const std::string& serializedData, PacketType& pkt)
{
  std::size_t data_ptr = 0;
  while (pkt.status() == packet::Status::INCOMPLETE && data_ptr < serializedData.size()) {
    TEST_ASSERT(pkt.remainingBytes() > 0);
    const std::size_t to_read = std::min(pkt.remainingBytes(), serializedData.size() - data_ptr);
    const std::size_t read = pkt.appendData(reinterpret_cast<const packet::byte_t*>(serializedData.data() + data_ptr), to_read);

    data_ptr += read;
  }
}

/**
 * @brief Read a full packet from a serialized data buffer
 * @param p The buffer
 * @return the packet read
 */
template<typename PacketType>
static inline PacketType
readPacket(const std::string& p)
{
  PacketType pkt;
  readPacketPart(p, pkt);
  return pkt;
}

/**
 * @brief Generates a serialized packet buffer from packet data
 * @param data  The packet dat we want to pack into a serialized packet
 * @return the serialized packet
 */
template<typename PacketType>
static inline std::string
serializePacketFromData(const std::string& data)
{
  PacketType p;
  std::stringstream ss;
  TEST_ASSERT(p.serialize(reinterpret_cast<const packet::byte_t*>(data.data()), data.size(), ss));

  const std::string result = ss.str();
  return result;
}


/**
 * @berif Check that we can serialize and unserialize content for a given packet type and
 *        should work
 */
template <typename PacketType>
static bool
checkSerializeAndUnserialize(const std::string& packetContent)
{
    const std::string serialized = serializePacketFromData<PacketType>(packetContent);
    std::cout << "Serialized data: " << serialized << "\n";

    const PacketType p = readPacket<PacketType>(serialized);
    std::cout << "packet status: " << int(p.status()) << "\n";
    return (p.status() == packet::Status::COMPLETE) &&
           std::string((const char*)p.data(), p.dataLen()) == packetContent;
}



#endif // PACKET_TEST_HELPERS_HPP
