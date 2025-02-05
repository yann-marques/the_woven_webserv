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

std::string	VServ::readRequest(const int fd) {
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

std::string	VServ::openFile(std::string &rootPath) {
	std::vector<char> buffer(4096);

	int fd = open(rootPath.c_str(), O_RDONLY);
	if (fd < 0)
		throw OpenFileException();

	ssize_t bytesRead = read(fd, buffer.data(), buffer.size());
	std::string fileData(buffer.begin(), buffer.begin() + bytesRead);
	close(fd);
	return (fileData);
}

void	VServ::processRequest(std::string rawRequest, int clientFd) {

	HttpRequest request(rawRequest);
	HttpRequest response;

	struct stat path_stat;

	//DEFINE IN THE CONFIG
	//...
	size_t client_max_body_size = 1000000; //in bytes ~1M
	std::vector<std::string> defaultPages;
	defaultPages.push_back("index");
	defaultPages.push_back("index.html");
	defaultPages.push_back("index.php");
	bool _autoIndex = true;
	//...
	//END DEFINE IN CONFIG


	response.setDefaultsHeaders();

	//DEFAULT CASE
	std::string rootPath;
	if (request.getPath() != "/")
		rootPath = _root + request.getPath();
	else
		rootPath = _root;

	std::cout << "ROOTPATH: " << rootPath << std::endl;

	try {

		std::string contentLengthStr = request.getHeader("Content-Length");
		if (!contentLengthStr.empty()) {
			size_t contentBytes;
			std::istringstream(contentLengthStr) >> contentBytes;
			if (contentBytes > client_max_body_size)
				throw EntityTooLarge();
		} else {
			std::string body = request.getBody();
			if (body.size() * sizeof(std::string::value_type) > client_max_body_size)
				throw EntityTooLarge(); 
		}

		if (stat(rootPath.c_str(), &path_stat) != 0)
			throw FileNotExist();

		if (S_ISREG(path_stat.st_mode)) { //ITS A FILE

			std::cout << "ITS A FILE" << std::endl;

			std::string file = openFile(rootPath);
			response.setBody(file);

		} else if (S_ISDIR(path_stat.st_mode)) { //ITS DIRECTORY

			std::cout << "ITS A DIRECTORY" << std::endl;

			DIR* dir = opendir(rootPath.c_str());
			if (!dir)
				throw OpenFolderException();


			if (_autoIndex) {

				struct dirent* entry;
				std::vector<std::string> fileNames;

				while ((entry = readdir(dir)) != NULL) {
					fileNames.push_back(entry->d_name);
				}
				closedir(dir);
				response.generateIndexFile(fileNames);

			} else {
				
				std::string file;

				for (std::vector<std::string>::iterator it = defaultPages.begin(); it != defaultPages.end(); it++) {
					std::string indexPath = rootPath + "/" + *it;
					if (stat(indexPath.c_str(), &path_stat) == 0 && S_ISREG(path_stat.st_mode)) {
						file = openFile(indexPath);
						break;
					}
				}

				if (file.empty())
					throw FileNotExist();
				
				response.setBody(file);
			}

		} else {
			response.setResponseCode(502);
		}
	} catch (FileNotExist &e) {
		response.makeError(HTTP_NOT_FOUND);
	} catch (OpenFileException &e) {
		response.makeError(HTTP_FORBIDDEN);
	} catch (OpenFolderException &e) {
		response.makeError(HTTP_FORBIDDEN);
	} catch (EntityTooLarge &e) {
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