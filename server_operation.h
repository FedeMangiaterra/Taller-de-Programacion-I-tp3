#ifndef SERVER_OPERATION_H
#define SERVER_OPERATION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <arpa/inet.h>
#include "common_socket.h"
#include "server_message_queue.h"
#include "server_message_queue_map.h"

class Operation {
    private:
    u_int16_t queue_name_length;
    std::string queue_name;
    
    public:
    explicit Operation(Socket* socket);

    /*Devuelve una referencia al string que contiene el nombre de la cola
    sobre la cual se realiza la operacion*/
    std::string& get_queue_name();

    virtual void execute(Message_queue_map* container,
                         Socket* socket) = 0;
    virtual ~Operation(); 
};

class Define_operation :public Operation {
    public:
    explicit Define_operation(Socket* socket);

    /*Comprueba si ya existe una cola con el nombre indicado. En
    caso contrario, la crea. Este metodo es atomico*/
    virtual void execute(Message_queue_map* container,
                         Socket* socket) override;
};

class Push_operation :public Operation {
    private:
    u_int16_t text_length;
    std::string text;

    public:
    explicit Push_operation(Socket* socket);

    /*Devuelve el mensaje que se debe guardar en la cola*/
    std::string get_text();

    /*Si la cola indicada existe, se guarda el mensaje en ella.
    Si no, no hace nada.*/
    virtual void execute(Message_queue_map* container,
                         Socket* socket) override;
};

class Pop_operation :public Operation {
    public:
    explicit Pop_operation(Socket* socket);

    /*Si la cola indicada existe, se espera a que tenga algun mensaje
    dentro y luego este es enviado al cliente. Si la cola no existe
    no hace nada*/
    virtual void execute(Message_queue_map* container,
                         Socket* socket) override;
};

#endif
