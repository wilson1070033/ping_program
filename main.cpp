#include "PingClient.hpp"
#include <iostream>
#include <cstring>
#include <cstdlib>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "USAGE: " << argv[0] 
                  << " <hostname or IP address> [count]" << std::endl;
        std::cout << std::endl;
        std::cout << "EXAMPLES:" << std::endl;
        std::cout << "  " << argv[0] << " google.com" << std::endl;
        std::cout << "  " << argv[0] << " 127.0.0.1 4" << std::endl;
        std::cout << "  " << argv[0] << " 8.8.8.8 10" << std::endl;
        return 1;
    }
    
    std::string destination = argv[1];
    int count = 4;
    
    if (argc >= 3) {
        count = atoi(argv[2]);
        if (count <= 0) {
            std::cerr << "ERROR: Count must be a positive integer" << std::endl;
            return 1;
        }
    }
    PingClient ping(destination, 2);
    
    if (!ping.initialize()) {
        return 1;
    }
    ping.run(count);
    return 0;
}
