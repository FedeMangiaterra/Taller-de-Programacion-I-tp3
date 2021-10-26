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
#include "server_message_queue_map.h"

Message_queue_map::Message_queue_map() {}

void Message_queue_map::assign(std::string key, Message_queue* value) {
    this->map[key] = value;
}

void Message_queue_map::assign_if_absent(std::string key) {
    std::unique_lock<std::mutex> lock(this->m);
    if (this->map.count(key) == 0) {
        //cppcheck-suppress stlFindInsert
        this->map[key] = new Message_queue();
        /*El cppcheck dice que no es necesario chequear antes de insertar,
        pero hago este chequeo porque no quiero que se cree una nueva cola
        en donde ya existe una*/
    }
}

Message_queue* Message_queue_map::operator[](std::string key) {
    std::unique_lock<std::mutex> lock(this->m);
    return this->map[key];
}

int Message_queue_map::count(const std::string &__x) {
    std::unique_lock<std::mutex> lock(this->m);
    return this->map.count(__x);
}

Message_queue_map::~Message_queue_map() {
    std::unordered_map<std::string, Message_queue*>::iterator it =
                                                this->map.begin();
    while (it != this->map.end()) {
        delete it->second;
        it = this->map.erase(it);
    }
}
