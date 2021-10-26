#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <arpa/inet.h>
#include "server_message_queue.h"
#include "server_message_queue_map.h"
#include "server_operation.h"

Operation::Operation(Socket* socket) {
    std::vector<char> queue_length_buffer(2);
    u_int16_t be_queue_name_length;
    if (socket->receive(&queue_length_buffer, 2) == -1) return;
    memcpy(&be_queue_name_length, &queue_length_buffer[0], 2);
    u_int16_t queue_name_length = ntohs(be_queue_name_length);
    this->queue_name_length = queue_name_length;
    std::vector<char> queue_name_buffer(queue_name_length);
    if (socket->receive(&queue_name_buffer, queue_name_length) == -1) return;
    this->queue_name.assign(&queue_name_buffer.at(0),
                            &queue_name_buffer.at(queue_name_length-1)+1);
}

std::string& Operation::get_queue_name(){
    return this->queue_name;
}

Operation::~Operation() {} 

Define_operation::Define_operation(Socket* socket) :
        Operation(socket) {}

void Define_operation::execute(Message_queue_map* container,
                         Socket* socket) {
    std::string queue_name = this->get_queue_name();
    container->assign_if_absent(queue_name);
}

Push_operation::Push_operation(Socket* socket) :
        Operation(socket) {
    std::vector<char> text_length_buffer(2);
    u_int16_t be_text_length;
    if (socket->receive(&text_length_buffer, 2) == -1) return;
    memcpy(&be_text_length, &text_length_buffer[0], 2);
    u_int16_t text_length = ntohs(be_text_length);
    this->text_length = text_length;
    std::vector<char> text_buffer(text_length);
    if (socket->receive(&text_buffer, text_length) == -1) return;
    this->text.assign(&text_buffer.at(0),
                    &text_buffer.at(text_length-1)+1);
}

std::string Push_operation::get_text(){
    return this->text;
}

void Push_operation::execute(Message_queue_map* container,
                         Socket* socket) {
const std::string& queue_name = this->get_queue_name();
if (container->count(queue_name) != 0) {
        (*container)[queue_name]->push(this->get_text());
    }
}

Pop_operation::Pop_operation(Socket* socket) :
            Operation(socket) {}

void Pop_operation::execute(Message_queue_map* container,
                         Socket* socket) {
const std::string& queue_name = this->get_queue_name();
if (container->count(queue_name) != 0) {
        std::string text = (*container)[queue_name]->wait_front_pop();
        u_int16_t text_length = text.size();
        std::vector<char> message(text_length+2);
        u_int16_t be_text_length = htons(text_length);
        memcpy(&message[0], &be_text_length, 2);
        memcpy(&message[2], &text[0], text_length);
        socket->send_bytes(&message, message.size());
    }
}
