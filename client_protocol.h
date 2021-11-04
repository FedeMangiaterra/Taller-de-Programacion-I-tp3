#ifndef CLIENT_PROTOCOL_H
#define CLIENT_PROTOCOL_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include "common_socket.h"

class Client_protocol {    
    public:
    Client_protocol();

    void complete_message(std::vector<char>* message,
                    std::vector<std::string>& args);

    void get_text(Socket* socket, std::string& text);
};

#endif
