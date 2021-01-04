#include <iostream>
#include <sstream>
#include <cassert>
#include <vector>

#include <packet/defs.h>
#include <packet/packet.h>

// test
#include "test_helpers.hpp"


void
testPacketLengthCalculation()
{
    // serialize a pakcet with different configuration should work
    struct TestConfig : packet::ConfigT<
              packet::DefaultStartPattern,
              packet::DefaultEndPattern,
              std::uint32_t,
              10
              >{};
    using Packet = packet::PacketT<TestConfig>;
    const std::size_t expectedSize = (sizeof(char) * 2) +   // start and end default pattern
                                     sizeof(std::uint32_t) +
                                     10;
    TEST_ASSERT(Packet::PACKET_MAX_SIZE == expectedSize);
}


void
testSimplePackets()
{
    const std::vector<std::string> packetContents {
        "this is a packet",
        " ",
        "\n\t\b\nxyz\n\n",
        std::string(4098, ' '),
    };
    for (const std::string& msg : packetContents) {
        TEST_ASSERT(checkSerializeAndUnserialize<packet::DefaultPacket>(msg));
    }
}

void
testDifferentPacketConfigWorks()
{
    // serialize a pakcet with different configuration should work
    struct TestConfig : packet::ConfigT<
            packet::DefaultStartPattern,
            packet::DefaultEndPattern,
            std::uint32_t,
            10
            >{};
    using Packet = packet::PacketT<TestConfig>;

    TEST_ASSERT(checkSerializeAndUnserialize<Packet>("content"));
}

void 
testSerializationAPIsWork(void)
{
  const std::string packet_content_str = "example";
  const packet::byte_t* packet_content = reinterpret_cast<const packet::byte_t*>(packet_content_str.data());
  std::stringstream stream;
  std::vector<packet::byte_t> buffer;

  // serialize both apis
  TEST_ASSERT(packet::DefaultPacket::serialize(packet_content, packet_content_str.size(), stream));
  TEST_ASSERT(packet::DefaultPacket::serialize(packet_content, packet_content_str.size(), buffer));

  const std::string stream_str = stream.str();
  const std::string buff_str(buffer.begin(), buffer.end());
  TEST_ASSERT(stream_str == buff_str);
}


void
testPacketLimits()
{
    // Define a new packet configuration
    static constexpr int MAX_PKT_SIZE = 5;
    struct TestConfig : packet::ConfigT<
            packet::DefaultStartPattern,
            packet::DefaultEndPattern,
            std::uint32_t,
            MAX_PKT_SIZE
            >{};
    using Packet = packet::PacketT<TestConfig>;

    packet::byte_t buff[MAX_PKT_SIZE * 2];
    std::vector<packet::byte_t> result;

    // ensure we cannot
    TEST_ASSERT(Packet::serialize(buff, MAX_PKT_SIZE + 1, result) == false);
    TEST_ASSERT(Packet::serialize(buff, 0, result) == false);
}


void
testPacketPartsWorks()
{
  const std::string pkt_msg = "this is a example of message";
  const std::string serialized = serializePacketFromData<packet::DefaultPacket>(pkt_msg);
  auto parts = splitStr(serialized, 2);
  TEST_ASSERT(parts.size() > 0);

  packet::DefaultPacket pkt;
  for (auto& part : parts) {
    readPacketPart(part, pkt);
  }
  TEST_ASSERT(pkt.status() == packet::Status::COMPLETE);
  TEST_ASSERT(std::string((const char*)pkt.data(), pkt.dataLen()) == pkt_msg);
}

void
testInvalidPacketAreDetected()
{
    const std::string pkt_msg = "this is a example of message";
    const std::string serialized = serializePacketFromData<packet::DefaultPacket>(pkt_msg);
    {
        // invalid head
        std::string serialized_test = serialized;
        serialized_test[0] = packet::DefaultStartPattern::value[0] + 1;
        auto pkt = readPacket<packet::DefaultPacket>(serialized_test);
        TEST_ASSERT(pkt.status() == packet::Status::INVALID);
    }

    {
        // invalid tail
        std::string serialized_test = serialized;
        serialized_test[serialized_test.size() - 1] = packet::DefaultEndPattern::value[0] + 1;
        auto pkt = readPacket<packet::DefaultPacket>(serialized_test);
        TEST_ASSERT(pkt.status() == packet::Status::INVALID);
    }
}

int
main(void)
{
    testPacketLengthCalculation();
    testSimplePackets();
    testDifferentPacketConfigWorks();
    testSerializationAPIsWork();
    testPacketLimits();
    testPacketPartsWorks();
    testInvalidPacketAreDetected();
    return 0;
}
