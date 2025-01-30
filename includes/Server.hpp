#pragma once

#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>

class Server {
    private:
        static int const _port = 8080;
        static int const _maxClients = 1000;

    public:
        Server(void);
        Server(const int &_port);
        ~Server(void);

        //METHODS
        void start(void);
};