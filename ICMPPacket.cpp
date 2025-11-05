#include "ICMPPacket.hpp"
#include "utils.hpp"
#include <cstring>
#include <iostream>
#include <iomanip>


unsigned short ICMPPacket::calculateChecksum(void* buffer, int length) {
    unsigned short* buf = (unsigned short*)buffer;
    unsigned int sum = 0;
    
    for (int i = 0; i < length / 2; i++) {
        sum += buf[i];
    }
    
    if (length % 2) {
        sum += *((unsigned char*)buf + length - 1);
    }
    
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    
    return (unsigned short)(~sum);
}

ICMPPacket::ICMPPacket() {
    memset(&packet, 0, sizeof(packet));
}

void ICMPPacket::prepare(int sequenceNumber) {
    packet.header.type = ICMP_ECHO;
    packet.header.code = 0;
    packet.header.un.echo.id = getpid();
    packet.header.un.echo.sequence = sequenceNumber;
    
    for (int i = 0; i < 56; i++) {
        packet.data[i] = i + 0x20;
    }
    
    packet.header.checksum = 0;
    packet.header.checksum = calculateChecksum(&packet, sizeof(packet));
    
    printInfo("ICMP Type", "Echo Request (8)");
    printInfo("ICMP Code", 0);
    printInfo("Process ID", getpid());
    printInfo("Sequence Number", sequenceNumber);
    
    std::cout << "  Checksum                 : 0x" << std::hex << std::setw(4) 
              << std::setfill('0') << packet.header.checksum << std::dec 
              << std::setfill(' ') << std::endl; // <-- Reset fill char
    
    printInfo("Payload Size", 56);
    printInfo("Total Packet Size", (int)sizeof(packet));
}

const void* ICMPPacket::getData() const { return &packet; }
size_t ICMPPacket::getSize() const { return sizeof(packet); }
int ICMPPacket::getId() const { return packet.header.un.echo.id; }
