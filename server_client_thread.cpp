#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <stdbool.h>
#include <string>
#include <arpa/inet.h>
#include <vector>
#include <utility>
#include <atomic>
#include "server_operation.h"
#include "common_socket.h"
#include "common_thread.h"
#include "server_message_queue_map.h"
#include "server_client_thread.h"

Client_thread::Client_thread(Socket socket,
            Message_queue_map* container) : client_socket(std::move(socket)),
                                        container(container),
                                        stop(false) {}

Operation* Client_thread::create_operation(Socket* client){
    std::vector<char> op(1);
    client->receive(&op, 1);
    if (op.at(0) == 'd') {
        return new Define_operation(client);
    } else if (op.at(0) == 'o'){
        return new Pop_operation(client);
    } else {
        return new Push_operation(client);
    }
}

bool Client_thread::has_finished() {
    return this->stop;
}

void Client_thread::end() {
    this->client_socket.stop();
}

void Client_thread::run() {
    while (this->stop == false) {
        try {
            Operation* op = create_operation(&this->client_socket);
            op->execute(this->container, &this->client_socket);
            delete(op);
        } catch(Socket_exception &err) {
            this->stop = true;
        }
    }
}
