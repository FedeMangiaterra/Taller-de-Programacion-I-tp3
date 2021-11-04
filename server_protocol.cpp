#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <arpa/inet.h>
#include "server_operation.h"
#include "common_socket.h"
#include "server_protocol.h"

Server_protocol::Server_protocol() {}

Define_operation* Server_protocol::create_define_operation(Socket* client) {
    std::vector<char> queue_length_buffer(2);
    u_int16_t be_queue_name_length;
    client->receive(&queue_length_buffer, 2);
    memcpy(&be_queue_name_length, &queue_length_buffer[0], 2);
    u_int16_t queue_name_length = ntohs(be_queue_name_length);
    std::vector<char> queue_name_buffer(queue_name_length);
    client->receive(&queue_name_buffer, queue_name_length);
    std::string queue_name;
    queue_name.assign(&queue_name_buffer.at(0),
                    &queue_name_buffer.at(queue_name_length-1)+1);
    return new Define_operation(queue_name);
}

Push_operation* Server_protocol::create_push_operation(Socket* client) {
    std::vector<char> queue_length_buffer(2);
    u_int16_t be_queue_name_length;
    client->receive(&queue_length_buffer, 2);
    memcpy(&be_queue_name_length, &queue_length_buffer[0], 2);
    u_int16_t queue_name_length = ntohs(be_queue_name_length);
    std::vector<char> queue_name_buffer(queue_name_length);
    client->receive(&queue_name_buffer, queue_name_length);
    std::string queue_name;
    queue_name.assign(&queue_name_buffer.at(0),
                    &queue_name_buffer.at(queue_name_length-1)+1);
    std::vector<char> text_length_buffer(2);
    u_int16_t be_text_length;
    client->receive(&text_length_buffer, 2);
    memcpy(&be_text_length, &text_length_buffer[0], 2);
    u_int16_t text_length = ntohs(be_text_length);
    std::vector<char> text_buffer(text_length);
    client->receive(&text_buffer, text_length);
    std::string text;
    text.assign(&text_buffer.at(0),
                    &text_buffer.at(text_length-1)+1);
    return new Push_operation(queue_name, text);
}

Pop_operation* Server_protocol::create_pop_operation(Socket* client) {
    std::vector<char> queue_length_buffer(2);
    u_int16_t be_queue_name_length;
    client->receive(&queue_length_buffer, 2);
    memcpy(&be_queue_name_length, &queue_length_buffer[0], 2);
    u_int16_t queue_name_length = ntohs(be_queue_name_length);
    std::vector<char> queue_name_buffer(queue_name_length);
    client->receive(&queue_name_buffer, queue_name_length);
    std::string queue_name;
    queue_name.assign(&queue_name_buffer.at(0),
                    &queue_name_buffer.at(queue_name_length-1)+1);
    return new Pop_operation(queue_name);
}

Operation* Server_protocol::create_operation(Socket* client) {
    std::vector<char> op(1);
    client->receive(&op, 1);
    if (op.at(0) == 'd') {
        return this->create_define_operation(client);
    } else if (op.at(0) == 'o'){
        return this->create_pop_operation(client);
    } else {
        return this->create_push_operation(client);
    }
}
