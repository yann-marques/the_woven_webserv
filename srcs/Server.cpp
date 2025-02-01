#include "Server.hpp"

Server::Server(void) : _port(8080) {};

Server::Server(const int &port) : _port(port) {};

Server::~Server(void) {};

void setNonBlocking(int fd) {
	fcntl(fd, F_SETFL, O_NONBLOCK);
}

volatile sig_atomic_t Server::_stopListenEvents = 0;

void Server::handleSignal(int signal) {
	if (signal == SIGINT)
	{
		std::cout << "\nSignal received, stopping the events loop...\n";
		_stopListenEvents = 1;
		// throw exception ?
	}
}

void Server::parseRequest(const std::string &requestData) {
	(void) requestData;
	//flemme pour ajd
}

void Server::readRequest(const int fd)
{
	std::vector<char> buffer(1024);
	int bytes_read = recv(fd, buffer.data(), buffer.size(), MSG_DONTWAIT);

	if (bytes_read > 0) {
		std::string http_request(buffer.begin(), buffer.begin() + bytes_read);
		std::cout << http_request << std::endl;

		this->parseRequest(http_request);
	} else if (bytes_read == 0){ //Empty FD, so the client closed request.
		close(fd);
		std::cout << "HTTP Request stop. FD Client closed!!" << std::endl;
	} else {
		std::cout << "Error: recv" << std::endl;
		//todo: exception
	}
}

void Server::listenEvents(void) {

	struct sockaddr_in clientAddress;
	struct epoll_event eventClient, events[this->_maxEvents]; // member attributes

	while (!this->_stopListenEvents) // try - si handleSignal throw une exception, sera-t-elle catch ici ? OUI
	{
		int numEvents = epoll_wait(this->_epollFd, events, this->_maxEvents, -1);
		if (numEvents == -1)
		{
			std::cerr << "Failed to wait for events." << std::endl;
			break;
		}

		for (int i = 0; i < numEvents; ++i)
		{
			if (events[i].data.fd == this->_serverFd) //EVENTS ON SERVERFD
			{
				// Accept new client connection
				socklen_t clientAddressLength = sizeof(clientAddress);
				int clientFd = accept(this->_serverFd, (struct sockaddr*)&clientAddress, &clientAddressLength);
				if (clientFd == -1)
				{
					std::cerr << "Failed to accept client connection." << std::endl;
					//todo: exception
				}

				setNonBlocking(clientFd);

				// Add client socket to epoll
				eventClient.events = EPOLLIN | EPOLLET;
				eventClient.data.fd = clientFd;
				if (epoll_ctl(this->_epollFd, EPOLL_CTL_ADD, clientFd, &eventClient) == -1) {
					std::cerr << "Failed to add client socket to epoll instance." << std::endl;
					close(clientFd);
					continue;
					//todo: exception
				}

				std::cout << "Event number " << i << " executed. Server FD: " << events[i].data.fd << std::endl;
				std::cout << "Client FD created. FD: " << clientFd << std::endl << std::endl;

			} else //NON SERVER-FD EVENTS (CLIENT FDS)
			{
				this->readRequest(events[i].data.fd);
				std::cout << "Event number " << i << " executed: FD: " << events[i].data.fd << std::endl << std::endl;
			}
		}
	}
}

void    Server::start(void) {
	struct sockaddr_in serverAddress;	// member attribute // vector ?
	struct epoll_event eventServer;		// same ? // vector

	_stopListenEvents = 0;
	signal(SIGINT, this->handleSignal);

////////////////// SET SERVER(S) SOCKET

	this->_serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_serverFd == -1) {
		std::cerr << "Failed to create socket." << std::endl;
		//todo: exception
	}

	setNonBlocking(this->_serverFd);

	//Allows the socket to reuse the address immediately after the program terminates.
	//Fix our bug
	int opt = 1;
	if (setsockopt(this->_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
		std::cerr << "Failed to set server fs opt" << std::endl;
	}

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(this->_port);
	if (bind(this->_serverFd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
		std::cerr << "Failed to bind socket." << std::endl;
		close(this->_serverFd);
		//todo: exception
	}

	if (listen(this->_serverFd, this->_maxClients) == -1) {
		std::cerr << "Failed to listen." << std::endl;
		close(this->_serverFd);
		//todo: exception
	}

/////////////////// set EPOLL

	this->_epollFd = epoll_create(this->_maxClients + 1); // arg deprecated ?
	if (this->_epollFd == -1) {
		std::cerr << "Failed to create epoll instance." << std::endl;
		close(this->_serverFd);
		//todo: exception
	}

/////////////////// EPOLL server

	eventServer.events = EPOLLIN;
	eventServer.data.fd = this->_serverFd;
	if (epoll_ctl(this->_epollFd, EPOLL_CTL_ADD, this->_serverFd, &eventServer) == -1) {
		std::cerr << "Failed to add server socket to epoll instance." << std::endl;
		close(this->_serverFd);
		close(this->_epollFd);
		//todo: exception
	}

	std::cout << "Server started. Listening on port " << this->_port << std::endl;
	this->listenEvents();

	close(this->_epollFd);
	close(this->_serverFd);
}