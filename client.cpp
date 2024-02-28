#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <ctime>
#include <chrono>
#include <unistd.h>
#include <iomanip>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


const std::string ADDRESS = "127.0.0.1";

int main(int argc, char *argv[]) {
    if (argc < 4) {
        std::cerr << "Please enter all required information!" << std::endl;
        std::cerr << "1. Client name" << std::endl;
        std::cerr << "2. Port number" << std::endl;
        std::cerr << "3. Duration" << std::endl;
        return 0;
    }

    std::string c_name = argv[1];
    int port = atoi(argv[2]);
    int duration = atoi(argv[3]);

    int client = socket(AF_INET, SOCK_STREAM, 0);
    if (client == -1) {
        std::cerr << "Couldn't create socket!" << std::endl;
        return 0;
    }

    sockaddr_in sock_addr;
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = port;
    inet_pton(AF_INET, "127.0.0.1", &sock_addr.sin_addr);
    if (connect(client, (sockaddr*)&sock_addr, sizeof(sock_addr)) == -1) {
        std::cerr << "Connection problem!" << std::endl;
        close(client);
        return 0;
    }

    while (1) {
        sleep(duration);

        auto now = std::chrono::system_clock::now();
        std::time_t c_time = std::chrono::system_clock::to_time_t(now);
        struct tm *time = std::localtime(&c_time);

        //std::cout << c_time << std::endl;

        std::stringstream sstream;
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;   
        sstream << std::put_time(time, "%Y-%m-%d %H:%M:%S") << "." << std::setfill('0') << std::setw(3) << milliseconds.count();
        std::string c_time_str = sstream.str();

        std::size_t pos = c_time_str.find('\n');
        if (pos != std::string::npos) {
            c_time_str.erase(pos, 1);
        }   

        std::string tmp = "[" + c_time_str + "]" + c_name;
        const char *message = tmp.c_str();

        send(client, message, strlen(message), 0);
    }

    std::cout << "Cliend stopped!" << std::endl;
    close(client);

    return 0;
}