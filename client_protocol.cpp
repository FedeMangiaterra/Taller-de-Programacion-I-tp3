#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <string.h>
#include <arpa/inet.h>
#include "client_protocol.h"

Client_protocol::Client_protocol() {}

void Client_protocol::complete_message(std::vector<char>* message,
                    std::vector<std::string>& args){
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

void Client_protocol::get_text(Socket* socket, std::string& text) {
        std::vector<char> text_length_buffer(2);
        u_int16_t be_text_length;
        socket->receive(&text_length_buffer, 2);
        memcpy(&be_text_length, &text_length_buffer[0], 2);
        u_int16_t text_length = ntohs(be_text_length);
        std::vector<char> text_buffer(text_length);
        socket->receive(&text_buffer, text_length);
        text.assign(&text_buffer.at(0),
                    &text_buffer.at(text_length-1)+1);
    }
