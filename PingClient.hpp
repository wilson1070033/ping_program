#ifndef PING_CLIENT_HPP
#define PING_CLIENT_HPP

#include "PingStatistics.hpp"
#include "ICMPPacket.hpp"
#include <string>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/time.h>

class PingClient {
private:
    int sockfd;
    struct sockaddr_in destAddr;
    std::string hostname;
    std::string ipAddress;
    int timeout;
    PingStatistics stats;

    bool createSocket();
    bool resolveHost(const std::string& host);
    double calculateRTT(const struct timeval& start, const struct timeval& end);
    bool sendPacket(const ICMPPacket& packet);
    bool receiveReply(int expectedSeq, struct timeval sendTime, double& rtt);

public:
    PingClient(const std::string& host, int timeoutSec = 2);
    ~PingClient();

    bool initialize();
    void run(int count = 4);
    const PingStatistics& getStatistics() const;
};

#endif
