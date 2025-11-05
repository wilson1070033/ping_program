#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>

void printSeparator(char c = '=', int width = 80);
void printHeader(const std::string& title);
void printSection(const std::string& title);
void printInfo(const std::string& label, const std::string& value, int labelWidth = 25);
void printInfo(const std::string& label, int value, int labelWidth = 25);
void printInfo(const std::string& label, double value, int labelWidth = 25, int precision = 3);
std::string getTimestamp();
std::string getIcmpTypeName(int type);

#endif
