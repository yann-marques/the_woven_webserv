#pragma once

#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <csignal>
#include <cstdlib>
#include <vector>

class Server {
    private:
        //PRIVATE ATTRIBUTS
        static volatile sig_atomic_t _stopListenEvents;
        int _serverFd;
        int _epollFd;
        int const _port;
        static int const _maxClients = 1000;
        static int const _maxEvents = 1000;

        //PRIVATE METHODS:
        void listenEvents(void);
        void readRequest(const int fd);
        void parseRequest(const std::string &request);
        static void handleSignal(int signal);

    public:
        Server(void);
        Server(const int &_port);
        ~Server(void);

        //PUBLIC METHODS
        void start(void);
};