#include "utils.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <sys/time.h>
#include <unistd.h>

void printSeparator(char c, int width) {
    std::cout << std::string(width, c) << std::endl;
}

void printHeader(const std::string& title) {
    printSeparator('=', 80);
    std::cout << "  " << title << std::endl;
    printSeparator('=', 80);
}

void printSection(const std::string& title) {
    printSeparator('-', 80);
    std::cout << "  " << title << std::endl;
    printSeparator('-', 80);
}

void printInfo(const std::string& label, const std::string& value, int labelWidth) {
    std::cout << "  " << std::left << std::setw(labelWidth) << label << ": " << value << std::endl;
}

void printInfo(const std::string& label, int value, int labelWidth) {
    std::cout << "  " << std::left << std::setw(labelWidth) << label << ": " << value << std::endl;
}

void printInfo(const std::string& label, double value, int labelWidth, int precision) {
    std::cout << "  " << std::left << std::setw(labelWidth) << label << ": " 
              << std::fixed << std::setprecision(precision) << value << std::endl;
}

std::string getTimestamp() {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    std::stringstream ss;
    ss << tv.tv_sec << "." << std::setfill('0') << std::setw(6) << tv.tv_usec << std::setfill(' '); // Reset fill
    return ss.str();
}

std::string getIcmpTypeName(int type) {
    switch(type) {
        case 0: return "Echo Reply";
        case 3: return "Destination Unreachable";
        case 5: return "Redirect";
        case 8: return "Echo Request";
        case 11: return "Time Exceeded";
        default: return "Unknown";
    }
}
