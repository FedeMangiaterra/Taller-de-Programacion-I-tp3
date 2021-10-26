#ifndef SERVER_ACCEPTING_THREAD_H
#define SERVER_ACCEPTING_THREAD_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <stdbool.h>
#include <string>
#include <arpa/inet.h>
#include <list>
#include <vector>
#include <atomic>
#include "server_operation.h"
#include "common_socket.h"
#include "common_thread.h"
#include "server_message_queue_map.h"
#include "server_client_thread.h"

class Accepting_thread : public Thread {
    private:
    Socket* host_socket;
    Message_queue_map container;
    std::atomic <bool>* stop;
    std::vector<Client_thread*> clients;

    public:
    Accepting_thread(Socket* socket, std::atomic <bool>* stop);

    /*Va aceptando clientes y creando hilos para realizar las operaciones.
    Tras aceptar un cliente, comprueba si hay alguno aceptado anteriormente
    que haya terminado, y lo elimina. El ciclo de aceptar clientes se detiene
    cuando se cierra el socket del servidor.*/
    virtual void run() override;
};
#endif
