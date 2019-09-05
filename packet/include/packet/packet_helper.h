#ifndef PACKET_PACKET_HELPER_H
#define PACKET_PACKET_HELPER_H

#include <string>

#include <packet/packet.h>


namespace packet {


static Packet
readPacket(const std::string& serialized_pkt)
{
  Packet pkt;
  std::size_t data_ptr = 0;
  while (pkt.status() == Packet::Status::INCOMPLETE && data_ptr < serialized_pkt.size()) {
    ASSERT(pkt.remainingBytes() > 0);
    const std::size_t to_read = std::min(pkt.remainingBytes(), serialized_pkt.size() - data_ptr);
    const std::size_t read = pkt.appendData(reinterpret_cast<const byte_t*>(serialized_pkt.data() + data_ptr), to_read);

    data_ptr += read;
  }
  return pkt;
}



}


#endif // PACKET_PACKET_HELPER_H
