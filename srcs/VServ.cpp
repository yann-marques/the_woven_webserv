#include "VServ.hpp"

// VServ::VServ();

VServ::VServ(VServConfig config, int maxClients): _maxClients(maxClients), _root("www") {
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

	int opt = 1;
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

std::string VServ::openFile(std::string &rootPath) {
    std::vector<char> buffer;
    ssize_t bytesRead;
    char tempBuffer[4096];

    int fd = open(rootPath.c_str(), O_RDONLY);
    if (fd < 0)
        throw OpenFileException();

    while ((bytesRead = read(fd, tempBuffer, sizeof(tempBuffer))) > 0) {
        buffer.insert(buffer.end(), tempBuffer, tempBuffer + bytesRead);
    }
	
	if (bytesRead < 0 && buffer.empty()) {
        close(fd);
        throw OpenFileException();
    }

    close(fd);
    return std::string(buffer.begin(), buffer.end());
}


std::string	VServ::readRequest(const int fd) {
	std::vector<char> buffer;
	ssize_t bytesRead;
	char tempBuffer[4096];

	while ((bytesRead = recv(fd, tempBuffer, sizeof(tempBuffer), MSG_DONTWAIT)) > 0) {
		buffer.insert(buffer.end(), tempBuffer, tempBuffer + bytesRead);
	}

	if (bytesRead == 0) {
		return ("");
	}
	if (bytesRead < 0 && buffer.empty()) {
	    close(fd);
        throw RecvException();
    }

	return std::string(buffer.begin(), buffer.end());
}

void	VServ::sendRequest(HttpRequest &request, int clientFd) {
	std::string	rawResponse = request.makeRawResponse();

	std::cout << "RESPONSE --------" << std::endl << rawResponse << std::endl;

	ssize_t bytesSent = send(clientFd, rawResponse.c_str(), rawResponse.size(), 0);
	if (bytesSent == -1) {
		throw SendException();
	} else if (bytesSent < static_cast<ssize_t>(rawResponse.size())) {
    	throw SendPartiallyException();
	}
}

void	VServ::handleBigRequest(HttpRequest &request) {
	size_t client_max_body_size = 1000000; //in bytes ~1M IN CONFIG

	std::string contentLengthStr = request.getHeader("Content-Length");
	if (!contentLengthStr.empty()) {
		size_t contentBytes;
		std::istringstream(contentLengthStr) >> contentBytes;
		if (contentBytes > client_max_body_size)
			throw EntityTooLarge();
	} else {
		std::string body = request.getBody();
		if (!body.empty() && (body.size() * sizeof(std::string::value_type) > client_max_body_size))
			throw EntityTooLarge(); 
	}
}

void	VServ::openDefaultPages(std::string &rootPath, HttpRequest &response) {

	//IN CONFIG !
	std::vector<std::string> defaultPages;
	defaultPages.push_back("index");
	defaultPages.push_back("index.html");
	defaultPages.push_back("index.php");
	//END IN CONFIG !

	std::string file;
	for (std::vector<std::string>::iterator it = defaultPages.begin(); it != defaultPages.end(); it++) {
		std::string indexPath = rootPath + "/" + *it;
		struct stat path_stat;
		if (stat(indexPath.c_str(), &path_stat) == 0 && S_ISREG(path_stat.st_mode)) {
			std::cout << "test" << std::endl; 
			file = openFile(indexPath);
			std::cout << file << std::endl;
			break;
		}
	}
	if (file.empty())
		throw FileNotExist();
	response.setBody(file);
}

void	VServ::showDirectory(DIR* dir, HttpRequest &response) {
	struct dirent* entry;
	std::vector<std::string> filesName;

	while ((entry = readdir(dir)) != NULL) {
		filesName.push_back(entry->d_name);
	}
	closedir(dir);
	response.generateIndexFile(filesName);
}

void	VServ::processRequest(std::string rawRequest, int clientFd) {

	HttpRequest request(rawRequest);
	HttpRequest response;
	struct stat path_stat;

	response.setDefaultsHeaders();

	//DEFINE IN THE CONFIG
	//...
	bool _autoIndex = false;
	//...
	//END DEFINE IN CONFIG

	std::string rootPath;
	if (request.getPath() != "/")
		rootPath = _root + request.getPath();
	else
		rootPath = _root;

	try {
		handleBigRequest(request);

		if (stat(rootPath.c_str(), &path_stat) != 0)
			throw FileNotExist();

		if (S_ISREG(path_stat.st_mode)) {

			std::cout << "ITS A FILE" << std::endl;
			std::string file = openFile(rootPath);
			response.setBody(file);

		} else if (S_ISDIR(path_stat.st_mode)) {

			DIR* dir = opendir(rootPath.c_str());
			if (!dir)
				throw OpenFolderException();

			_autoIndex ? showDirectory(dir, response) : openDefaultPages(rootPath, response);

		} else {
			response.setResponseCode(502);
		}

	} catch (FileNotExist& e) {
		response.makeError(HTTP_NOT_FOUND);
	} catch (OpenFileException& e) {
		response.makeError(HTTP_FORBIDDEN);
	} catch (OpenFolderException& e) {
		response.makeError(HTTP_FORBIDDEN);
	} catch (EntityTooLarge& e) {
		response.makeError(HTTP_PAYLOAD_TOO_LARGE);
	}

	sendRequest(response, clientFd);
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

const char*	VServ::ReadFileException::what() const throw() {
	return ("Fail read the file");
}

const char*	VServ::FileNotExist::what() const throw() {
	return ("Fail to get infos about the file");
}

const char*	VServ::OpenFileException::what() const throw() {
	return ("Error to opening the root file");
}

const char*	VServ::OpenFolderException::what() const throw() {
	return ("Error to opening the folder");
}

const char*	VServ::EntityTooLarge::what() const throw() {
	return ("Error, the entity is too lage. Change client_max_body_size in config");
}


std::ostream&	operator<<(std::ostream& os, const VServ& vs) {
	os	<< "----------- VSERV -----------" << std::endl
		<< "\tport:\t" << vs.getPort() << std::endl
		<< "\thost:\t" << vs.getHost() << std::endl
		<< "\tfd:\t" << vs.getFd() << std::endl;
	// ...
	return (os);
}