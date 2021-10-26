#ifndef COMMON_SOCKET_H
#define COMMON_SOCKET_H

#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include <string>
#include <exception>
#include <vector>

class Socket_exception : public std::exception {
    private:
    std::string function;

    public:
    explicit Socket_exception(const std::string& function);
    
    const char* what();

    std::string get_function();
};

class Socket {
    private:
    int fd;

    public:
    Socket();

    Socket(Socket&& other);

    /*Asigna el socket a determinado host y servicio, y se lo marca como
    encargado de recibir intentos de conexion entrantes por parte de los
    clientes*/
    int bind_and_listen(const char *host,
                            const char *service);

    /*Asigna al socket pasado por parametro como el socket de la conexion
    con el peer*/
    int accept_peer(Socket* peer);

    /*Establece la conexion con el host y servicio indicados*/
    int establish_connection(const char *host, const char *service);

    /*Envia la cantidad de bytes indicada por el parametro length, 
    con varios envios si es un tamanio muy grande*/
    ssize_t send_bytes(std::vector<char>* buffer, ssize_t length);

    /*Recibe la cantidad de bytes indicada por el parametro length,
    con varios recv si es un tamanio muy grande. Se queda bloqueado
    hasta que eso se cumpla que se recibieron length bytes*/
    ssize_t receive(std::vector<char>* buffer, ssize_t length);

    /*Hace un shutodown y un close del socket*/
    void stop();

    Socket(const Socket&) = delete;

    Socket& operator=(const Socket&) = delete;

    ~Socket();
};

#endif
