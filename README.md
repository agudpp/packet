# Description
Simple header only c++ library to pack / unpack data into "packages" so can be sent over the
wire (or store it in a serialized stream).

The goal of `packet` is to provide efficient and generic package handling with a simple interface.

I personally used this library for sending different binary serialized messages over TCP
(like `protobuf` messages).

## Layout

The packets have the following shape:
`[ head_pattern | pkt_content_len | content | tail_pattern ]`

- head_pattern: a user defined pattern to detect early wrong or invalid messages over the wire
- pkt_content_len: field indicating the size of the content buffer
- content: the data / content itself
- tail_pattern: ensure that the packet size was correct and respects the protocol.


## Features

- Serialization of a packet data from its content
- Support of head / tail patterns for early error detections.
- Dynamic size and data types.


## Usage

You can copy the `include` folder directly into your project or just use `git submodule`
and add the include folder to the `packet` project.

You can define or configure your packet based on your requirements, or use the default one
`packet::DefaultPacket` which uses the [default configuration](include/packet/defs.h).

The packets are intended to be read from a stream (or socket) with the goal of detecting
potential errors (invalid packets) as soon as possible. In the [tests](src/test_helpers.hpp)
you will find more examples on how to read packets.
The following pseudocode shows how to read from a `client` (TCP socket for example).

```cpp
// this is an example for a DefaultPacket and there is a client class
Client client;
packet::DefaultPacket pkt;
// ..
while (pkt.status() == packet::DefaultPacket::Status::INCOMPLETE) {
    const std::size_t to_read = std::min(pkt.remainingBytes(), READ_BUFF_SIZE);
    const int client_readed = client.readData(buff, to_read);
    if (client_readed <= 0) {
      return Error::INVALID_CONNECTION;
    }
    // append the data to the packet
    const std::size_t packet_read = packet.appendData(reinterpret_cast<const packet::byte_t*>(buff),
                                                      std::size_t(client_readed));
    if (packet_read == 0) {
      return Error::INVALID_PACKET;
    }
}

// check that the packet was successfully read
if (packet.status() == packet::DefaultPacket::Status::INVALID) {
  return Error::INVALID_PACKET;
}

// extract the final packet content data
const std::string pkt_content(reinterpret_cast<const char*>(packet.data()), packet.dataLen());

```


For more usage cases check [tests](src/test.cpp).
