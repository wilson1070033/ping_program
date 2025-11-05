#ifndef ICMP_PACKET_HPP
#define ICMP_PACKET_HPP

#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <unistd.h>
#include <cstddef> // for size_t

class ICMPPacket {
private:
    struct Packet {
        struct icmphdr header;
        char data[56];
    } packet;

    static unsigned short calculateChecksum(void* buffer, int length);

public:
    ICMPPacket();

    void prepare(int sequenceNumber);
    const void* getData() const;
    size_t getSize() const;
    int getId() const;
};

#endif
