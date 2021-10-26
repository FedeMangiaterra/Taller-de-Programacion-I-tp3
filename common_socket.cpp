#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <netdb.h>
#include <unistd.h>
#include <vector>
#include <exception>
#include "common_socket.h"

#define MAX_QUEUE 15 //El valor que se va a usar en listen()
#define SUCCESS 0
#define ERROR 1

Socket_exception::Socket_exception(const std::string& function) :
                                    function(function) {}
    
const char* Socket_exception::what() {
    return "Socket error at "; 
}

std::string Socket_exception::get_function() {
    return this->function;
}

Socket::Socket() {
    this->fd = socket(AF_INET, SOCK_STREAM, 6);
    if (this->fd == -1) {
        throw(Socket_exception("Socket constructor"));
    }
}

Socket::Socket(Socket&& other) {
    this->fd = other.fd;
    //Deja un fd invalido, luego en la destruccion de other no se llama a close
    other.fd = -1; 
}

int Socket::bind_and_listen(const char *host,
                            const char *service) {
    struct addrinfo hints;
    struct addrinfo *ptr, *res;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;       
    hints.ai_socktype = SOCK_STREAM; 
    hints.ai_flags = AI_PASSIVE;
    if (getaddrinfo(host, service, &hints, &res) < 0) {
        throw(Socket_exception("getaddrinfo() at bind_and_listen()"));
    }
    ptr = res;
    int val = 1;
    if (setsockopt(this->fd,
                    SOL_SOCKET,
                    SO_REUSEADDR,
                    &val,
                    sizeof(val)) == -1) {
        throw(Socket_exception("setsockopt() at bind_and_listen()"));
    }
    if (bind(this->fd, ptr->ai_addr, ptr->ai_addrlen) < 0) {
        freeaddrinfo(res);
        throw(Socket_exception("bind() at bind_and_listen()"));
    }
    freeaddrinfo(res);
    listen(this->fd, MAX_QUEUE);
    return SUCCESS;
}

int Socket::establish_connection(const char *host, const char *service) {
    struct addrinfo hints;
    struct addrinfo *ptr, *res;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;       
    hints.ai_socktype = SOCK_STREAM; 
    hints.ai_flags = 0;
    if (getaddrinfo(host, service, &hints, &res) < 0) {
        throw(Socket_exception("getaddrinfo() at establish_connection()"));
    }
    ptr = res;
    if (connect(this->fd, ptr->ai_addr, ptr->ai_addrlen) < 0) {
            freeaddrinfo(res);
            throw(Socket_exception("connect() at establish_connection()"));
    }
    freeaddrinfo(res);
    return SUCCESS;
}

int Socket::accept_peer(Socket* peer) {
    close(peer->fd);
    peer->fd = accept(this->fd, NULL, NULL);
    if (peer->fd == -1) {
        throw(Socket_exception("accept()"));
    }
    return SUCCESS;
}

ssize_t Socket::send_bytes(std::vector<char>* buffer, ssize_t length) {
    ssize_t total_bytes_sent = 0;
    while (total_bytes_sent < length) {
        int bytes_sent = send(this->fd,
                             &buffer->at(total_bytes_sent),
                            length - total_bytes_sent,
                            MSG_NOSIGNAL);
        if (bytes_sent == 0) {
            throw(Socket_exception("send()"));
        } else if (bytes_sent == -1) {
            throw(Socket_exception("send()"));
        } else {
            total_bytes_sent += bytes_sent;
        }
    }
    return total_bytes_sent;
}

ssize_t Socket::receive(std::vector<char>* buffer, ssize_t length) {
    ssize_t total_bytes_received = 0;
    while (total_bytes_received < length) {
        int bytes_received = recv(this->fd,
                                 &buffer->at(total_bytes_received),
                                length - total_bytes_received,
                                0);
        if (bytes_received == 0) {
            throw(Socket_exception("receive()"));
        } else if (bytes_received == -1) {
            throw(Socket_exception("receive()"));
        } else {
            total_bytes_received += bytes_received;
        }
    }
    return total_bytes_received;
}

void Socket::stop() {
    shutdown(this->fd, SHUT_RDWR);
    close(this->fd);
}

Socket::~Socket() {
    if (this->fd != -1) {
        close(this->fd);
    }
}
