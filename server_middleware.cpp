#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <stdbool.h>
#include <string>
#include <arpa/inet.h>
#include <vector>
#include <atomic>
#include <unordered_map>
#include "server_message_queue.h"
#include "server_operation.h"
#include "common_socket.h"
#include "common_thread.h"
#include "server_message_queue_map.h"
#include "server_client_thread.h"
#include "server_accepting_thread.h"
#include "server_middleware.h"

Server_middleware::Server_middleware(const char *service) : host_socket(),
                                            stop(false) {
    this->host_socket.bind_and_listen(0, service);
}

int Server_middleware::run() {
    Thread* accepting_thread = new Accepting_thread(&this->host_socket,
                                                        &this->stop);
    accepting_thread->start();
    char* input = NULL;
    size_t size = 1;
    while (this->stop == false){
        if (getline(&input, &size, stdin) < 0) return 2;
        if (input[0] == 'q') {
            this->stop = true;
            this->host_socket.stop();
            accepting_thread->join();
        }
        free(input);
        input = NULL;
    }
    delete(accepting_thread);
    return 0; 
}

Server_middleware::~Server_middleware() {}

