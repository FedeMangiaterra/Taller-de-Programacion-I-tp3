#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <stdbool.h>
#include <string>
#include <sstream>
#include <arpa/inet.h>
#include <vector>
#include "common_socket.h"
#include "client_protocol.h"
#include "client_middleware.h"

Client_middleware::Client_middleware(const char *host,
                    const char *service) {
    this->client_socket.establish_connection(host, service);
}

void Client_middleware::process_line(std::string line, std::vector<char>* message) {
    std::vector<std::string> args;
    std::istringstream stringstream(line);
    std::string arg;
    std::getline(stringstream, arg, ' ');
    args.push_back(arg);
    if (std::getline(stringstream, arg, ' ')) {
        args.push_back(arg);
    } else {
        throw(std::runtime_error("Invalid operation"));
    }
    if (std::getline(stringstream, arg, '\n')) {
        args.push_back(arg);
    }
    if (args[0] == "define" || args[0] == "push" || args[0] == "pop") {
        Client_protocol protocol;
        protocol.complete_message(message, args);
    } else {
        throw(std::runtime_error("Invalid operation"));
    }
}

int Client_middleware::get_text(){
    Client_protocol protocol;
    std::string text;
    protocol.get_text(&this->client_socket, text);
    std::cout << text << '\n';
    return 0;
}

int Client_middleware::run(){
    std::string line;
    std::getline(std::cin, line);
    while (line != "exit") {
        std::vector<char> message(3);
        process_line(line, &message);
        try {
            this->client_socket.send_bytes(&message, message.size());
            if (message.at(0) == 'o') {
                this->get_text();
            }
        } catch (Socket_exception &err) {
            break; //El socket esta cerrado
        }
        std::getline(std::cin, line);
    }
    return 0;
}
