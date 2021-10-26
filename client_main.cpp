#include <stdio.h>
#include <stdlib.h>
#include <exception>
#include <iostream>
#include "client_middleware.h"

#define SUCCESS 0
#define INVALID_ARGUMENT 1
#define ERROR 2

int main(int argc, char **argv) {
    try {
        if (argc < 2) throw(std::invalid_argument("Missing host argument"));
        if (argc < 3) throw(std::invalid_argument("Missing service argument")); 
        Client_middleware client(argv[1], argv[2]);
        client.run();
        return SUCCESS;
    } catch(std::invalid_argument &error) {
        return INVALID_ARGUMENT;
    } catch(Socket_exception &err) {
        std::cout << err.what() << err.get_function() << '\n';
        return ERROR;
    } catch (...) {
        return ERROR;
    }
}
