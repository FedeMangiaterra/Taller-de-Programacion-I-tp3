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

Operation::Operation(const std::string& queue_name) : queue_name(queue_name) {}

std::string& Operation::get_queue_name(){
    return this->queue_name;
}

Operation::~Operation() {} 

Define_operation::Define_operation(const std::string& queue_name) :
        Operation(queue_name) {}

void Define_operation::execute(Message_queue_map* container,
                         Socket* socket) {
    std::string queue_name = this->get_queue_name();
    container->assign_if_absent(queue_name);
}

Push_operation::Push_operation(const std::string& queue_name,
                                const std::string& text) :
        Operation(queue_name), text(text) {}

std::string Push_operation::get_text(){
    return this->text;
}

void Push_operation::execute(Message_queue_map* container,
                         Socket* socket) {
const std::string& queue_name = this->get_queue_name();
//Se asume que si se hace push la cola ya existe
(*container)[queue_name]->push(this->get_text()); 
}

Pop_operation::Pop_operation(const std::string& queue_name) :
        Operation(queue_name) {}

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
