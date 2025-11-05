#ifndef PING_STATISTICS_HPP
#define PING_STATISTICS_HPP

#include <vector>
#include <string>
#include <sys/time.h>

class PingStatistics {
private:
    int transmitted;
    int received;
    int errors;
    double totalTime;
    double minTime;
    double maxTime;
    std::vector<double> rttValues;
    struct timeval startTime;
    struct timeval endTime;

    double getAverageTime() const;
    double calculateStdDev() const;

public:
    PingStatistics();
    
    void addTransmitted();
    void addReceived(double rtt);
    void addError();

    int getTransmitted() const;
    int getReceived() const;
    int getErrors() const;
    int getPacketLoss() const;
    
    void printDetailedStatistics(const std::string& host) const;
};

#endif
