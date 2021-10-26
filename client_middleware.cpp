#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <stdbool.h>
#include <string>
#include <arpa/inet.h>
#include <vector>
#include "common_socket.h"
#include "client_middleware.h"

Client_middleware::Client_middleware(const char *host,
                    const char *service) {
    this->client_socket.establish_connection(host, service);
}

void Client_middleware::complete_message(std::vector<char>* message,
                    std::vector<std::string>& args) {
    size_t i;
    u_int16_t queue_name_length = args[1].length();
    u_int16_t be_queue_name_length = htons(queue_name_length);
    memcpy(&message->at(1), &be_queue_name_length, 2);
    for (i = 0; i < queue_name_length; i++) {
        message->push_back(args[1][i]);
    }
    if (args[0] == "define") {
        message->at(0) = 'd';
    } else if (args[0] == "push") {
        message->at(0) = 'u';
        message->resize(message->size() + 2);
        u_int16_t text_length = args[2].length();
        u_int16_t be_text_length = htons(text_length);
        memcpy(&message->at(queue_name_length+3), &be_text_length, 2);
        for (i = 0; i < text_length; i++) {
            message->push_back(args[2][i]);
        }
    } else if (args[0] == "pop") {
        message->at(0) = 'o';
    }
}

void Client_middleware::process_line(char* line, std::vector<char>* message) {
    std::vector<std::string> args;
    char* arg = strtok(line, " ");
    if (arg == nullptr) throw(std::runtime_error("Invalid operation"));
    args.push_back(arg);
    arg = strtok(NULL, " ");
    if (arg == nullptr) throw(std::runtime_error("Invalid operation"));
    args.push_back(arg);
    arg = strtok(NULL, "\n");
    if (arg != nullptr) {
        args.push_back(arg);
    }
    if (args[0] == "define" || args[0] == "push" || args[0] == "pop") {
        complete_message(message, args);
    } else {
        throw(std::runtime_error("Invalid operation"));
    }
}

int Client_middleware::get_text(){
    std::vector<char> text_length_buffer(2);
    u_int16_t be_text_length;
    std::string text;
    this->client_socket.receive(&text_length_buffer, 2);
    memcpy(&be_text_length, &text_length_buffer[0], 2);
    u_int16_t text_length = ntohs(be_text_length);
    std::vector<char> text_buffer(text_length);
    this->client_socket.receive(&text_buffer, text_length);
    text.assign(&text_buffer.at(0),
                &text_buffer.at(text_length-1)+1);
    std::cout << text << '\n';
    return 0;
}

int Client_middleware::run(){
    char* line = NULL;
    size_t size = 100;
    if (getline(&line, &size, stdin) < 0) {
        free(line);
        throw(std::runtime_error("Error at getline"));
    }
    size_t line_size = strlen(line);
    line[line_size-1] = '\0';
    while (strcmp(line, "exit") != 0) {
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
        free(line);
        line = NULL;
        if (getline(&line, &size, stdin) < 0) {
            free(line);
            throw(std::runtime_error("Error at getline"));
        }
        line_size = strlen(line);
        line[line_size-1] = '\0';
    }
    free(line);
    return 0;
}
