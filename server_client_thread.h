#ifndef SERVER_CLIENT_THREAD_H
#define SERVER_CLIENT_THREAD_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <atomic>
#include "server_operation.h"
#include "common_socket.h"
#include "common_thread.h"
#include "server_message_queue_map.h"

class Client_thread : public Thread {
    private:
    Socket client_socket;
    Message_queue_map* container;
    std::atomic <bool> stop;

    Operation* create_operation(Socket* client);

    public:
    Client_thread(Socket socket,
                Message_queue_map* container);

    /*Devuelve un booleano que indica si la ejecucion del
    hilo de detuvo, esto es que el cliente cerro la conexion*/
    bool has_finished(); 

    /*Cierra el socket de la conexion entre el servidor y el cliente*/
    void end();

    /*Recibe operaciones a realizar por parte del cliente, y las ejecuta.*/
    virtual void run() override;
};

#endif
