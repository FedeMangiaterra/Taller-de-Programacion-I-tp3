#ifndef SERVER_MESSAGE_QUEUE_MAP
#define SERVER_MESSAGE_QUEUE_MAP

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <stdbool.h>
#include <string>
#include <thread>
#include <vector>
#include <unordered_map>
#include "server_message_queue.h"

class Message_queue_map {
    private:
    std::unordered_map<std::string, Message_queue*> map;
    std::mutex m;

    Message_queue_map(const Message_queue_map&) = delete;

    Message_queue_map& operator=(const Message_queue_map&) = delete;

    public:
    Message_queue_map();

    /*Se guarda en el mapa un valor en la clave indicada, exista o no*/
    void assign(std::string key, Message_queue* value);

    /*Se crea un nuevo par clave-valor, solo si la clave no estaba
    ya presente en el mapa. Esta operacion es atomica*/
    void assign_if_absent(std::string key); 

    /*Devuelve el puntero a Message_queue de la clave indicada*/
    Message_queue* operator[](std::string key);

    /*Devuelve 0 si la clave no se encuentra en el mapa, 1 en caso contrario*/
    int count(const std::string &__x);

    ~Message_queue_map();
};

#endif
