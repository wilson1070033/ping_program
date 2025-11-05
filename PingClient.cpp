#include "PingClient.hpp"
#include "utils.hpp"
#include <unistd.h>
#include <sys/time.h>
#include <netdb.h>
#include <cerrno>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <cstring> 

PingClient::PingClient(const std::string& host, int timeoutSec) 
    : sockfd(-1), hostname(host), timeout(timeoutSec) {
    memset(&destAddr, 0, sizeof(destAddr));
}

PingClient::~PingClient() {
    if (sockfd >= 0) {
        std::cout << std::endl << "  Closing socket (fd: " << sockfd << ")..." << std::endl;
        close(sockfd);
        std::cout << "  [SUCCESS] Socket closed" << std::endl;
    }
}

bool PingClient::createSocket() {
    printSection("SOCKET CREATION");
    
    std::cout << "  Creating raw socket..." << std::endl;
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    
    if (sockfd < 0) {
        std::cout << "  [FAILED] Cannot create socket" << std::endl;
        std::cout << "  Error Code: " << errno << std::endl;
        std::cout << "  Error Message: " << strerror(errno) << std::endl;
        std::cout << "  Note: Root privileges required for raw sockets" << std::endl;
        return false;
    }
    
    std::cout << "  [SUCCESS] Socket created" << std::endl;
    printInfo("Socket File Descriptor", sockfd);
    printInfo("Socket Type", "SOCK_RAW");
    printInfo("Protocol", "IPPROTO_ICMP");
    
    std::cout << std::endl << "  Configuring socket options..." << std::endl;
    struct timeval tv;
    tv.tv_sec = timeout;
    tv.tv_usec = 0;
    
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        std::cout << "  [WARNING] Cannot set socket timeout: " << strerror(errno) << std::endl;
    } else {
        std::cout << "  [SUCCESS] Socket timeout configured" << std::endl;
        printInfo("Receive Timeout", std::to_string(timeout) + " seconds");
    }
    
    return true;
}

bool PingClient::resolveHost(const std::string& host) {
    printSection("HOSTNAME RESOLUTION");
    
    printInfo("Target Hostname", host);
    std::cout << "  Querying DNS..." << std::endl;
    
    struct hostent* hostInfo = gethostbyname(host.c_str());
    
    if (hostInfo == nullptr) {
        std::cout << "  [FAILED] Cannot resolve hostname" << std::endl;
        std::cout << "  h_errno: " << h_errno << " (" << hstrerror(h_errno) << ")" << std::endl;
        return false;
    }
    
    destAddr.sin_family = AF_INET;
    destAddr.sin_addr = *((struct in_addr*)hostInfo->h_addr);
    ipAddress = inet_ntoa(destAddr.sin_addr);
    
    std::cout << "  [SUCCESS] Hostname resolved" << std::endl;
    printInfo("IP Address", ipAddress);
    printInfo("Address Family", "AF_INET (IPv4)");
    
    return true;
}

double PingClient::calculateRTT(const struct timeval& start, const struct timeval& end) {
    return (end.tv_sec - start.tv_sec) * 1000.0 +
           (end.tv_usec - start.tv_usec) / 1000.0;
}

bool PingClient::sendPacket(const ICMPPacket& packet) {
    std::cout << "  Sending ICMP packet..." << std::endl;
    printInfo("Destination IP", ipAddress);
    printInfo("Packet Size", (int)packet.getSize());
    
    int sent = sendto(sockfd, packet.getData(), packet.getSize(), 0,
                      (struct sockaddr*)&destAddr, sizeof(destAddr));
    
    if (sent < 0) {
        std::cout << "  [FAILED] Send operation failed" << std::endl;
        std::cout << "  Error: " << strerror(errno) << std::endl;
        return false;
    }
    
    std::cout << "  [SUCCESS] Packet sent" << std::endl;
    printInfo("Bytes Sent", sent);
    printInfo("Send Timestamp", getTimestamp());
    stats.addTransmitted();
    return true;
}

bool PingClient::receiveReply(int expectedSeq, struct timeval sendTime, double& rtt) {
    const int MAX_RETRIES = 10;
    char buffer[1024];
    struct sockaddr_in fromAddr;
    socklen_t fromLen = sizeof(fromAddr);
    
    std::cout << std::endl << "  Waiting for reply..." << std::endl;
    printInfo("Expected Sequence", expectedSeq);
    printInfo("Max Receive Attempts", MAX_RETRIES);
    
    for (int retry = 0; retry < MAX_RETRIES; retry++) {
        std::cout << std::endl << "  [Attempt " << (retry + 1) << "/" << MAX_RETRIES << "]" << std::endl;
        
        int receivedBytes = recvfrom(sockfd, buffer, sizeof(buffer), 0,
                                     (struct sockaddr*)&fromAddr, &fromLen);
        
        if (receivedBytes < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                std::cout << "  [TIMEOUT] No packet received within timeout period" << std::endl;
                printInfo("Timeout Duration", std::to_string(timeout) + " seconds");
            } else {
                std::cout << "  [ERROR] Receive error: " << strerror(errno) << std::endl;
            }
            return false;
        }
        
        struct timeval recvTime;
        gettimeofday(&recvTime, nullptr);
        
        std::cout << "  [RECEIVED] Packet received" << std::endl;
        printInfo("Bytes Received", receivedBytes);
        printInfo("Source Address", inet_ntoa(fromAddr.sin_addr));
        printInfo("Receive Timestamp", getTimestamp());
        
        struct iphdr* ipHeader = (struct iphdr*)buffer;
        int ipHeaderLen = ipHeader->ihl * 4;
        struct icmphdr* icmpReply = (struct icmphdr*)(buffer + ipHeaderLen);
        
        std::cout << std::endl << "  IP HEADER ANALYSIS:" << std::endl;
        printInfo("IP Version", (int)ipHeader->version);
        printInfo("Header Length", std::to_string(ipHeaderLen) + " bytes");
        printInfo("Time To Live (TTL)", (int)ipHeader->ttl);
        printInfo("Protocol", (int)ipHeader->protocol);
        printInfo("Total Length", ntohs(ipHeader->tot_len));
        
        std::cout << std::endl << "  ICMP HEADER ANALYSIS:" << std::endl;
        printInfo("ICMP Type", std::to_string((int)icmpReply->type) + " (" + getIcmpTypeName(icmpReply->type) + ")");
        printInfo("ICMP Code", (int)icmpReply->code);
        printInfo("ICMP ID", icmpReply->un.echo.id);
        printInfo("ICMP Sequence", icmpReply->un.echo.sequence);
        
        std::cout << "  Checksum                 : 0x" << std::hex << std::setw(4) 
                  << std::setfill('0') << icmpReply->checksum << std::dec 
                  << std::setfill(' ') << std::endl; // <-- Reset fill char
        
        if (icmpReply->type == ICMP_ECHO) {
            std::cout << "  [SKIP] Echo Request detected (our own packet on loopback)" << std::endl;
            std::cout << "  Continuing to next packet..." << std::endl;
            continue;
        }
        
        std::cout << std::endl << "  PACKET VERIFICATION:" << std::endl;
        bool typeMatch = (icmpReply->type == ICMP_ECHOREPLY);
        bool idMatch = (icmpReply->un.echo.id == getpid());
        bool seqMatch = (icmpReply->un.echo.sequence == expectedSeq);
        
        printInfo("Type Match", typeMatch ? "YES (0 - Echo Reply)" : "NO");
        printInfo("ID Match", idMatch ? "YES" : "NO");
        printInfo("Sequence Match", seqMatch ? "YES" : "NO");
        
        if (typeMatch && idMatch && seqMatch) {
            rtt = calculateRTT(sendTime, recvTime);
            
            std::cout << std::endl << "  [SUCCESS] Valid Echo Reply received" << std::endl;
            printSeparator('-', 80);
            
            int dataSize = receivedBytes - ipHeaderLen;
            std::cout << "  REPLY SUMMARY: " << dataSize << " bytes from " 
                      << inet_ntoa(fromAddr.sin_addr)
                      << ": icmp_seq=" << expectedSeq
                      << " ttl=" << (int)ipHeader->ttl
                      << " time=" << std::fixed << std::setprecision(3) << rtt << " ms" << std::endl;
            
            printSeparator('-', 80);
            return true;
        } else {
            std::cout << "  [MISMATCH] Packet verification failed" << std::endl;
            if (!typeMatch) std::cout << "    - Wrong ICMP type" << std::endl;
            if (!idMatch) std::cout << "    - Wrong process ID" << std::endl;
            if (!seqMatch) std::cout << "    - Wrong sequence number" << std::endl;
            std::cout << "  Continuing to next packet..." << std::endl;
        }
    }
    
    std::cout << std::endl << "  [FAILED] No matching reply received after " 
              << MAX_RETRIES << " attempts" << std::endl;
    return false;
}


bool PingClient::initialize() {
    printHeader("ICMP PING - VERBOSE MODE");
    
    std::cout << std::endl;
    printInfo("Program Version", "1.0");
    printInfo("Target Host", hostname);
    printInfo("Process ID", getpid());
    printInfo("Timeout", std::to_string(timeout) + " seconds");
    
    std::cout << std::endl;
    
    if (!createSocket()) {
        return false;
    }
    
    std::cout << std::endl;
    
    if (!resolveHost(hostname)) {
        close(sockfd);
        sockfd = -1;
        return false;
    }
    
    return true;
}

void PingClient::run(int count) {
    std::cout << std::endl;
    printHeader("STARTING PING SEQUENCE");
    
    std::cout << std::endl << "PING " << hostname << " (" << ipAddress 
              << ") 56 bytes of data" << std::endl;
    printInfo("Total Packets to Send", count);
    printInfo("Interval Between Packets", "1 second");
    
    for (int seq = 1; seq <= count; seq++) {
        std::cout << std::endl;
        printSeparator('=', 80);
        std::cout << "  PACKET " << seq << " OF " << count << std::endl;
        printSeparator('=', 80);
        
        std::cout << std::endl;
        printSection("PACKET PREPARATION");
        
        ICMPPacket packet;
        packet.prepare(seq);
        
        std::cout << std::endl;
        printSection("PACKET TRANSMISSION");
        
        struct timeval sendTime;
        gettimeofday(&sendTime, nullptr);
        
        if (!sendPacket(packet)) {
            stats.addError();
            sleep(1);
            continue;
        }
        
        std::cout << std::endl;
        printSection("PACKET RECEPTION");
        
        double rtt;
        if (receiveReply(seq, sendTime, rtt)) {
            stats.addReceived(rtt);
        } else {
            stats.addError();
        }
        
        if (seq < count) {
            std::cout << std::endl << "  Waiting 1 second before next packet..." << std::endl;
            sleep(1);
        }
    }
    
    std::cout << std::endl;
    stats.printDetailedStatistics(hostname);
}

const PingStatistics& PingClient::getStatistics() const {
    return stats;
}
