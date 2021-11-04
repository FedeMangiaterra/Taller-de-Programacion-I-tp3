#ifndef CLIENT_MIDDLEWARE_H
#define CLIENT_MIDDLEWARE_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include "common_socket.h"

class Client_middleware{
    private:
    Socket client_socket;

    Client_middleware(const Client_middleware&) = delete;

    Client_middleware& operator=(const Client_middleware&) = delete;

    void process_line(std::string line, std::vector<char>* message);

    int get_text();
    
    public:
    Client_middleware(const char *host,
                    const char *service);

    /*Se conecta al servidor y va recibiendo las operaciones a enviar
    por entrada estandar. La ejecucion se detiene cuando el servidor cierra
    el socket, cuando se ingresa 'exit' por entrada estandar, o cuando se
    ingresa una operacion no valida*/
    int run();
};

#endif
