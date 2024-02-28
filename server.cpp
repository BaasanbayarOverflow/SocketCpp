#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <fstream>


void *handler(void *arg) {
    int client = *((int*)arg);
    char buffer[128];
    int bytes;

    std::ofstream file("log.txt", std::ios::app);
    if (!file.is_open()) {
        std::cerr << "Couldn't open the file!" << std::endl;
        return 0;
    }

    while (1) {
        bytes = recv(client, buffer, sizeof(buffer), 0);
        if (bytes <= 0) {
            break;
        }

        buffer[bytes] = '\0';
        std::cout << buffer << std::endl;
        file << buffer << "\n";
    }

    file.close();

    close(client);
    delete (int*)arg;
    pthread_exit(NULL);
}


int main(int argc, char *argv[]) {
    std::cout << "Hello, server" << std::endl;

    if (argc != 2) {
        std::cerr << "Please enter port number only!" << std::endl;
        return 0;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        std::cerr << "Couldn't create socket!" << std::endl;
        return 0;
    } 

    int port = atoi(argv[1]);
    if (port == 0) {
        std::cerr << "Port number incorrect!"; 
        return 0;
    } 

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = port;
    if (bind(sock, (sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        std::cerr << "Couldn't bind socker" << std::endl;
        close(sock);
        return 0;
    }

    if (listen(sock, 5) == -1) {
        std::cerr << "Couldn't listen incoming packets!" << std::endl;
        close(sock);
        return 0;
    }

    std::cout << "Server is listening on #" << port << std::endl; 


    while (1) {
        sockaddr_in client_addr;
        socklen_t client_address = sizeof(client_addr);
        int *client = new int;
        *client = accept(sock, (sockaddr*)&client_addr, &client_address);
        if (*client == -1) {
            std::cerr << "Connection error!" << std::endl;
            delete client;
            continue;
        }

        pthread_t thread;
        if (pthread_create(&thread, NULL, handler, (void*)client) != 0) {
            std::cerr << "Couldn't create thread!" << std::endl;
            delete client;
            continue;
        }
    }

    close(sock);
    return 0;
}