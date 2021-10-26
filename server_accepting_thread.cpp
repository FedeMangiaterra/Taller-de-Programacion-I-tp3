#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <vector>
#include <list>
#include <atomic>
#include <algorithm>
#include <utility>
#include <exception>
#include "server_message_queue.h"
#include "server_operation.h"
#include "common_socket.h"
#include "common_thread.h"
#include "server_client_thread.h"
#include "server_accepting_thread.h"

Accepting_thread::Accepting_thread(Socket* socket, std::atomic <bool>* stop) :
                    host_socket(socket), stop(stop) {}

void Accepting_thread::remove_if_finished() {
    std::vector<Client_thread*>::iterator it;
    it = this->clients.begin();
    while (it != this->clients.end()){
        if ((Client_thread*)(*it)->has_finished()) {
            (*it)->join();
            delete ((Client_thread*)(*it));
            it = this->clients.erase(it);
        } else {
            ++it;
        }
    }
}

void Accepting_thread::stop_all_clients() {
    size_t i;
    for (i = 0; i < this->clients.size(); i++){
        this->clients[i]->end();
        this->clients[i]->join();
        delete(this->clients[i]);
    }
}

void Accepting_thread::run() {
    while (*this->stop == false) {
        Socket client;
        try {
            this->host_socket->accept_peer(&client);
        } catch(Socket_exception &err) {
            break;
        }
        Client_thread* client_thread = new Client_thread(std::move(client),
                                                    &this->container);
        this->clients.push_back(client_thread);
        client_thread->start();
        this->remove_if_finished();
    }
    this->stop_all_clients();
}
