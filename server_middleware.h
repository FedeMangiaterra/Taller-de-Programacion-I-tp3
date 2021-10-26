#ifndef SERVER_MIDDLEWARE_H
#define SERVER_MIDDLEWARE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <atomic>
#include "server_operation.h"
#include "common_socket.h"
#include "server_message_queue_map.h"

class Server_middleware {
    private:
    Socket host_socket;
    std::atomic <bool> stop;

    Server_middleware(const Server_middleware&) = delete;

    Server_middleware& operator=(const Server_middleware&) = delete;

    public:
    explicit Server_middleware(const char *service);

    /*Mientras espera a que se ingrese la letra q por consola, lanza un
    hilo que se encarga de ir aceptando clientes, para los cuales se lanzan
    mas hilos(uno por cliente) en los que se realizan las operaciones que
    se indiquen*/
    int run();

    ~Server_middleware();
};

#endif
