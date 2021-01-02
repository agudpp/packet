# Description
Simple header only c++ library to pack / unpack data into "packages" so can be sent over the
wire (or store it in a serialized stream).

The goal of `packet` is to provide efficient and generic package handling with a simple interface.

I personally used this library for sending different binary serialized messages over TCP
(like `protobuf` messages).

## Layout

The packets have the following shape:
`[ head_pattern | pkt_content_len | content | tail_pattern ]`

- head_pattern: (optional) a user defined pattern to detect early wrong or invalid messages over the wire
- pkt_content_len: field indicating the size of the content buffer
- content: the data / content itself
- tail_pattern: (optional) ensure that the packet size was correct and respects the protocol.


## Features

- Serialization of a packet data from its content
- Support of head / tail patterns for early error detections.
- Dynamic size and data types.

## Building

I compiled this using g++ but should work on almost any compiler that supports c++14.

```bash
# clone the repo into
git clone https://github.com/agudpp/packet.git
# compile the debug version
cd packet && \
    mkdir -p build && \
    cd build &&
    cmake .. &&
    cmake --build . --config Debug -- -j 8
# run the tests
./packet
```


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
byte_t buff[READ_BUFF_SIZE];
packet::DefaultPacket pkt;
// ..
while (pkt.status() == packet::Status::INCOMPLETE) {
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
if (packet.status() == packet::Status::INVALID) {
  return Error::INVALID_PACKET;
}

// extract the final packet content data
const std::string pkt_content(reinterpret_cast<const char*>(packet.data()), packet.dataLen());

```

Serialization example

```cpp

const std::string packet_content_str = "example of a message content";
const packet::byte_t* packet_content = reinterpret_cast<const packet::byte_t*>(packet_content_str.data());


// packet can be serialized into a ostream or buffer
std::stringstream stream;
std::vector<packet::byte_t> buffer;

packet::DefaultPacket::serialize(packet_content, packet_content_str.size(), stream);
packet::DefaultPacket::serialize(packet_content, packet_content_str.size(), buffer);

```


For more usage cases check [tests](src/test.cpp).


