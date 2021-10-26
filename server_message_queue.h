#ifndef Message_queue_H
#define Message_queue_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <thread>
#include <utility>
#include <string>
#include <atomic>
#include <condition_variable>
#include <deque>

class Message_queue{
    private:
    std::deque<std::string>* queue;
    std::mutex m;
    std::condition_variable queue_condition_variable;
    std::mutex condition_variable_m;

    public:
    Message_queue();

    /*Agrega el mensaje pasado por parametro al final
    de la cola. Esta operacion es atomica*/
    void push(std::string);

    /*Devuelve el mensaje que esta al principio
    de la cola. Esta operacion es atomica*/
    std::string front();

    /*Devuelve el mensaje que esta al principio
    de la cola y ademas lo saca de la misma, todo
    de forma atomica*/
    std::string front_and_pop();

    /*Si no hay nada en la cola, espera a que haya
    algo. Luego devuelve el mensaje que esta al principio
    de la cola y ademas lo saca de la misma de forma atomica*/
    std::string wait_front_pop();

    /*Devuelve la cantidad de mensajes presentes
    en la cola, de forma atomica.*/
    size_t size();

    /*Devuelve un booleano que indica si la cola esta
    vacia o no.*/
    bool empty();

    ~Message_queue();

    private:
    Message_queue(const Message_queue& other) = delete;
    Message_queue& operator=(const Message_queue &other) = delete;
};
#endif
