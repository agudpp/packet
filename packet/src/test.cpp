#include <iostream>
#include <sstream>
#include <cassert>
#include <vector>

#include <packet/defs.h>
#include <packet/packet.h>
#include <packet/pattern.h>


using namespace packet;


static std::vector<std::string>
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

static void
readPacketPart(const std::string& serializedData, Packet& pkt)
{
  std::size_t data_ptr = 0;
  while (pkt.status() == Packet::Status::INCOMPLETE && data_ptr < serializedData.size()) {
    PKT_ASSERT(pkt.remainingBytes() > 0);
    const std::size_t to_read = std::min(pkt.remainingBytes(), serializedData.size() - data_ptr);
    const std::size_t read = pkt.appendData(reinterpret_cast<const byte_t*>(serializedData.data() + data_ptr), to_read);

    data_ptr += read;
  }
}

static Packet
readPacket(const std::string& p)
{
  Packet pkt;
  readPacketPart(p, pkt);
  return pkt;
}


static std::string
serializePacketFromData(const std::string& data)
{
  Packet p;
  std::stringstream ss;
  assert(p.serialize(reinterpret_cast<const byte_t*>(data.data()), data.size(), ss));

  const std::string result = ss.str();
  const data_len_t* len_part = reinterpret_cast<const data_len_t*>(result.data() + 2);
  std::cout << "#### real len: " << data.size() << " converted: " << *len_part << "\n";

  return result;
}


void
testCreatePacket2()
{

  const std::string& data = "agustin";
  const std::string serialized = serializePacketFromData(data);
  std::cout << "Serialized data: " << serialized << "\n";

  Packet p = readPacket(serialized);
  std::cout << "packet status: " << int(p.status()) << "\n";
  assert(p.status() == Packet::Status::COMPLETE);
  assert(std::string((const char*)p.data(), p.dataLen()) == data);
}

void 
testPattern(void)
{
  const std::string& data = "agustin";
  const std::string serialized = serializePacketFromData(data);
  std::cout << "Serialized data: " << serialized << "\n";
  const Pattern head_pattern = std::vector<byte_t>{0xAF, 0xFA};
  std::cout << " pattern head == serialized? " << bool(head_pattern == reinterpret_cast<const byte_t*>(serialized.data())) << "\n";

}

void 
testSerializeLen(void)
{
  char buff[sizeof(data_len_t)];
  data_len_t len = 123;
  std::memcpy(buff, &len, sizeof(len));

  data_len_t len2 = *(reinterpret_cast<data_len_t*>(buff));
  std::cout << "len: " << len << " - len2: " << len2 << " - buff: " << buff <<"\n";
}


void
testMultiplePackets()
{
  const std::vector<std::string> packetContents {
    "this is a packet",
    "",
    "\n\t\b\n\n\n",
    std::string(4098, ' '),
  };
  for (const std::string& msg : packetContents) {
    const std::string serialized = serializePacketFromData(msg);
    const Packet pkt = readPacket(serialized);
    assert(pkt.status() == Packet::Status::COMPLETE);
    assert(std::string((const char*)pkt.data(), pkt.dataLen()) == msg);
  }
}

void
testPacketPartsWorks()
{
  const std::string pktMsg = "this is a example of message";
  const std::string serialized = serializePacketFromData(pktMsg);
  auto parts = splitStr(serialized, 2);
  assert(parts.size() > 0);

  Packet pkt;
  for (auto& part : parts) {
    readPacketPart(part, pkt);
  }
  assert(pkt.status() == Packet::Status::COMPLETE);
  assert(std::string((const char*)pkt.data(), pkt.dataLen()) == pktMsg);
}

int main(void)
{
  testCreatePacket2();
  testSerializeLen();
  testPattern();
  testMultiplePackets();
  testPacketPartsWorks();

  return 0;
}
