#include "VServ.hpp"

// VServ::VServ();

VServ::VServ(VServConfig config, int maxClients): _maxClients(maxClients) {
	// tmp
	_port = config.getPort();
	_host = config.getHost();
	// parse config ...
	setAddress();
	socketInit();
}

// VServ::VServ(const VServ& rhs);

VServ&	VServ::operator=(const VServ& rhs) {
	_port = rhs.getPort();
	_host = rhs.getHost();
	_fd = rhs.getFd();
	setAddress();
//	std::cout << "address port = " << _address.sin_port << std::endl;
	return (*this);
}

VServ::~VServ() {
//	if (_fd != -1)
//		close(_fd);
}

// SETTERS

void	VServ::setAddress() {
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = INADDR_ANY;
	_address.sin_port = htons(_port);
}

// GETTERS

int	VServ::getPort() const {
	return (_port);
}

int	VServ::getHost() const {
	return (_host);
}

int	VServ::getFd() const {
	return (_fd);
}

// METHODS

void	VServ::socketInit() {
	_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_fd == -1)
		throw (SocketException());
	fcntl(_fd, F_SETFL, O_NONBLOCK); // setNonBlocking

	int opt = 1; // member attribute ? need later ?
	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
		throw (SetSockOptException());

	if (bind(_fd, (struct sockaddr*)&_address, sizeof(_address)) == -1)
		throw (BindException());

	if (listen(_fd, _maxClients) == -1)
		throw (ListenException());

	// catch in WebServ constructor
}

int	VServ::clientAccept(void) {
	sockaddr_in clientAddress;
	socklen_t clientAddressLength = sizeof(clientAddress);
	int clientFd = accept(_fd, (struct sockaddr*)&clientAddress, &clientAddressLength);
	if (clientFd == -1)
		throw (AcceptException());
	return (clientFd);
}

std::string	VServ::readRequest(const int fd)
{
	std::vector<char> buffer(4096);
	int bytes_read = recv(fd, buffer.data(), buffer.size(), MSG_DONTWAIT);

	if (bytes_read > 0) {
		std::string rawRequest(buffer.begin(), buffer.begin() + bytes_read);
		return (rawRequest);
	} else if (bytes_read == 0){
		return "";
	} else {
		throw RecvException();
	}
}

void	VServ::processRequest(std::string rawRequest, int clientFd) {
	HttpRequest rq(rawRequest);

	/* std::cout << rawRequest << std::endl;
	std::cout << rq.getMethod() << std::endl;
	std::cout << rq.getPath() << std::endl;
	std::cout << rq.getVersion() << std::endl;
	std::cout << rq.getHeader("Host") << std::endl; */

	rq.makeReponse("Je suis la reponse. Je suis construit depuis HttpRequest::makeReponse et call dans VServ::processRequest");
	std::string rawResponse = rq.makeRawResponse();

	std::cout << rawResponse << std::endl;

	ssize_t bytesSent = send(clientFd, rawResponse.c_str(), rawResponse.size(), 0);
	if (bytesSent == -1) {
		throw SendException();
	} else if (bytesSent < static_cast<ssize_t>(rawResponse.size())) {
    	throw SendPartiallyException();
	} else { //request finialized
		close(clientFd);
	}
}

//EXCEPTION

const char*	VServ::SocketException::what() const throw() {
	return ("Failed to create socket.");
}

const char*	VServ::SetSockOptException::what() const throw() {
	return ("Failed to set socket opt.");
}

const char*	VServ::BindException::what() const throw() {
	return ("Failed to bind socket.");
}

const char*	VServ::ListenException::what() const throw() {
	return ("Failed to listen.");
}

const char*	VServ::AcceptException::what() const throw() {
	return ("Failed accept connection on socket.");
}

const char*	VServ::RecvException::what() const throw() {
	return ("Failed to read the request in the buffer.");
}

const char*	VServ::SendException::what() const throw() {
	return ("Failed to send the request to the clientfd");
}

const char*	VServ::SendPartiallyException::what() const throw() {
	return ("Failed to send entire request to the client");
}

std::ostream&	operator<<(std::ostream& os, const VServ& vs) {
	os	<< "----------- VSERV -----------" << std::endl
		<< "\tport:\t" << vs.getPort() << std::endl
		<< "\thost:\t" << vs.getHost() << std::endl
		<< "\tfd:\t" << vs.getFd() << std::endl;
	// ...
	return (os);
}