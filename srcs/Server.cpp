#include "Server.hpp"

Server::Server(void) {};

Server::~Server(void) {};

void setNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void    Server::start(void) {
    int serverFd;
    struct sockaddr_in serverAddress, clientAddress;

    serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd == -1) {
        std::cerr << "Failed to create socket." << std::endl;
        //todo: exception
    }

    // Rendre le socket non bloquant
    setNonBlocking(serverFd);

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(this->_port);
    if (bind(serverFd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Failed to bind socket." << std::endl;
        close(serverFd);
    }

    if (listen(serverFd, this->_maxClients) == -1) {
        std::cerr << "Failed to listen." << std::endl;
        close(serverFd);
        //todo: exception
    }

    std::cout << "Server started. Listening on port " << this->_port << std::endl;

    socklen_t clientAddressLength = sizeof(clientAddress);
    int clientFd = accept(serverFd, (struct sockaddr*)&clientAddress, &clientAddressLength);
    std::cout << "test" << std::endl;
    if (clientFd == -1) {
        std::cerr << "Failed to accept client connection." << std::endl;
    }

    char buffer[1000000];

    int bytes_read = read(clientFd, buffer, sizeof(buffer));
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        std::cout << "Received: " << buffer << std::endl;
    }

    close(serverFd);
    close(clientFd);
}