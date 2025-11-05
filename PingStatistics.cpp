#include "PingStatistics.hpp"
#include "utils.hpp"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <sys/time.h>

PingStatistics::PingStatistics() 
    : transmitted(0), received(0), errors(0), totalTime(0.0), 
      minTime(999999.0), maxTime(0.0) {
    gettimeofday(&startTime, nullptr);
}

void PingStatistics::addTransmitted() {
    transmitted++;
    std::cout << "  [TX] Packet transmitted (total: " << transmitted << ")" << std::endl;
}

void PingStatistics::addReceived(double rtt) {
    received++;
    totalTime += rtt;
    rttValues.push_back(rtt);
    
    if (rtt < minTime) minTime = rtt;
    if (rtt > maxTime) maxTime = rtt;
    
    std::cout << "  [RX] Packet received successfully" << std::endl;
    std::cout << "  [STAT] Total received: " << received << "/" << transmitted 
              << " (" << std::fixed << std::setprecision(1) 
              << (received * 100.0 / transmitted) << "%)" << std::endl;
}

void PingStatistics::addError() {
    errors++;
    std::cout << "  [ERROR] Packet processing error (total errors: " << errors << ")" << std::endl;
}

int PingStatistics::getTransmitted() const { return transmitted; }
int PingStatistics::getReceived() const { return received; }
int PingStatistics::getErrors() const { return errors; }

double PingStatistics::getAverageTime() const { 
    return received > 0 ? totalTime / received : 0.0; 
}

double PingStatistics::getMinTime() const { return minTime; }
double PingStatistics::getMaxTime() const { return maxTime; }

int PingStatistics::getPacketLoss() const {
    return transmitted > 0 ? 
           ((transmitted - received) * 100 / transmitted) : 0;
}

double PingStatistics::calculateStdDev() const {
    if (rttValues.size() < 2) return 0.0;
    
    double avg = getAverageTime();
    double sumSquares = 0.0;
    
    for (double rtt : rttValues) {
        double diff = rtt - avg;
        sumSquares += diff * diff;
    }
    
    return std::sqrt(sumSquares / rttValues.size());
}

void PingStatistics::printDetailedStatistics(const std::string& host) const {
    gettimeofday((struct timeval*)&endTime, nullptr);
    double totalDuration = (endTime.tv_sec - startTime.tv_sec) + 
                           (endTime.tv_usec - startTime.tv_usec) / 1000000.0;
    
    std::cout << std::endl;
    printHeader("PING STATISTICS");
    
    printInfo("Target Host", host);
    printInfo("Test Duration", totalDuration, 25, 3);
    
    printSeparator('-', 80);
    std::cout << "  PACKET STATISTICS" << std::endl;
    printSeparator('-', 80);
    
    printInfo("Packets Transmitted", transmitted);
    printInfo("Packets Received", received);
    printInfo("Packets Lost", transmitted - received);
    printInfo("Packet Loss Rate", std::to_string(getPacketLoss()) + " %");
    printInfo("Errors", errors);
    
    if (received > 0) {
        printSeparator('-', 80);
        std::cout << "  ROUND-TRIP TIME (RTT) STATISTICS" << std::endl;
        printSeparator('-', 80);
        
        printInfo("Minimum RTT", minTime, 25, 3);
        printInfo("Average RTT", getAverageTime(), 25, 3);
        printInfo("Maximum RTT", maxTime, 25, 3);
        printInfo("Standard Deviation", calculateStdDev(), 25, 3);
        printInfo("Range (max - min)", maxTime - minTime, 25, 3);
        
        printSeparator('-', 80);
        std::cout << "  RTT DISTRIBUTION" << std::endl;
        printSeparator('-', 80);
        
        std::cout << "  Individual RTT values (ms):" << std::endl;
        for (size_t i = 0; i < rttValues.size(); i++) {
            std::cout << "    Packet " << (i + 1) << ": " 
                      << std::fixed << std::setprecision(3) << rttValues[i] << " ms" << std::endl;
        }
    }
    
    printSeparator('=', 80);
}
