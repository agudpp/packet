#include <iostream>
#include <sstream>

#include <packet/defs.h>
#include <packet/packet.h>
#include <packet/pattern.h>

#include <packet/test_defs.h>

using namespace packet;

static Packet
readPacket(const std::string& p)
{
  Packet pkt;
  std::size_t data_ptr = 0;
  while (pkt.status() == Packet::Status::INCOMPLETE && data_ptr < p.size()) {
    PKT_ASSERT(pkt.remainingBytes() > 0);
    const std::size_t to_read = std::min(pkt.remainingBytes(), p.size() - data_ptr);
    const std::size_t read = pkt.appendData(reinterpret_cast<const byte_t*>(p.data() + data_ptr), to_read);

    data_ptr += read;
  }
  return pkt;
}


static std::string
getSerializedPacket(const std::string& data)
{
  Packet p;
  std::stringstream ss;
  PKT_ASSERT(p.serialize(reinterpret_cast<const byte_t*>(data.data()), data.size(), ss));

  const std::string result = ss.str();
  const data_len_t* len_part = reinterpret_cast<const data_len_t*>(result.data() + 2);
  std::cout << "#### real len: " << data.size() << " converted: " << *len_part << "\n";

  return result;
}


//DEF_TEST(testInvalidPacket,
//{
//  Packet p = readPacket("agu");
//  EXPECT_EQ(p.status(), Packet::Status::INVALID);
//});


DEF_TEST(testCreatePacket2,
{
  const std::string& data = "agustin";
           const std::string serialized = getSerializedPacket(data);
           std::cout << "Serialized data: " << serialized << "\n";

  Packet p = readPacket(serialized);
           std::cout << "packet status: " << int(p.status()) << "\n";
  EXPECT_EQ(p.status(), Packet::Status::COMPLETE);

});

void testPattern(void)
{
  const std::string& data = "agustin";
  const std::string serialized = getSerializedPacket(data);
  std::cout << "Serialized data: " << serialized << "\n";
  const Pattern head_pattern = std::vector<byte_t>{0xAF, 0xFA};
  std::cout << " pattern head == serialized? " << bool(head_pattern == reinterpret_cast<const byte_t*>(serialized.data())) << "\n";

}

void testSerializeLen(void)
{
  char buff[sizeof(data_len_t)];
  data_len_t len = 123;
  std::memcpy(buff, &len, sizeof(len));

  data_len_t len2 = *(reinterpret_cast<data_len_t*>(buff));
  std::cout << "len: " << len << " - len2: " << len2 << " - buff: " << buff <<"\n";
}

int main(void)
{
  testSerializeLen();
  testPattern();
  RUN_ALL_TESTS;

  return 0;
}
