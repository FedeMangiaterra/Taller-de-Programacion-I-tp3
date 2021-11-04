#ifndef SERVER_PROTOCOL_H
#define SERVER_PROTOCOL_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "common_socket.h"
#include "server_operation.h"

class Server_protocol {    
    public:
    Server_protocol();

    Define_operation* create_define_operation(Socket* client);

    Push_operation* create_push_operation(Socket* client);

    Pop_operation* create_pop_operation(Socket* client);

    Operation* create_operation(Socket* client);
};

#endif
