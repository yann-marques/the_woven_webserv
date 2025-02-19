#include "VServ.hpp"

// VServ::VServ();

VServ::VServ(VServConfig config, int maxClients, std::set<std::string> argv, std::set<std::string> envp): _maxClients(maxClients), _root("www") {
	// tmp
	_port = config.getPort();
	_host = config.getHost();
	// parse config ...
	setAddress();
	socketInit();
	//
	this->_argv = argv;
	this->_envp = envp;
	this->_debug = false;
	if (argv.find("--debug=yes") != argv.end())
		this->_debug = true;
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

std::string	VServ::makeRootPath(HttpRequest &request) {
	std::string rqRootPath = request.getRootPath();
	std::string rqPath = request.getPath();

	if (!rqRootPath.empty())
		return (rqRootPath);
	if (rqPath != "/") {
		return (_root + getPagePath(request));
	}
	else
		return(_root);
}


std::string VServ::readRequest(HttpRequest &request) {
    std::vector<char> buffer;
    ssize_t bytesRead;
	std::string cgiContent;
    char tempBuffer[4096];

	std::string rootPath = makeRootPath(request);
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
	std::string fileData = std::string(buffer.begin(), buffer.end());

	if (fileIsCGI(request)) {
		cgiContent = handleCGI(fileData, request);
		return (cgiContent);
	}

    return (fileData);
}


std::string	VServ::readSocketFD(const int fd) {
	std::vector<char> buffer;
	ssize_t bytesRead;
	char tempBuffer[4096];

	while ((bytesRead = recv(fd, tempBuffer, sizeof(tempBuffer), MSG_DONTWAIT)) > 0) {
		buffer.insert(buffer.end(), tempBuffer, tempBuffer + bytesRead);
	}

	if (bytesRead < 0 && buffer.empty()) {
	    close(fd);
        throw RecvException();
    }

	if (_debug)
		std::cout << "RESPONSE ----- " << std::endl << buffer.data() << std::endl;

	return std::string(buffer.begin(), buffer.end());
}

std::string VServ::readFile(int fd) {
	std::vector<char> buffer;
	ssize_t bytesRead;
	char tempBuffer[4096];

	while ((bytesRead = read(fd, tempBuffer, sizeof(tempBuffer)))) {
		buffer.insert(buffer.end(), tempBuffer, tempBuffer + bytesRead);
	}

	if (bytesRead < 0 && buffer.empty()) {
	    close(fd);
        throw RecvException();
    }

	return std::string(buffer.begin(), buffer.end());
}

void	VServ::sendRequest(HttpRequest &response, int clientFd) {
	std::string	rawResponse = response.makeRawResponse();

	if (_debug)
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

std::string	VServ::readDefaultPages(HttpRequest &request) 
{

	//IN CONFIG !
	std::vector<std::string> defaultPages;
	defaultPages.push_back("index");
	defaultPages.push_back("index.html");
	defaultPages.push_back("index.php");
	//END IN CONFIG !

	std::string file;
	for (std::vector<std::string>::iterator it = defaultPages.begin(); it != defaultPages.end(); it++) {
		std::string indexPath = makeRootPath(request) + "/" + *it;
		struct stat path_stat;
		if (stat(indexPath.c_str(), &path_stat) == 0 && S_ISREG(path_stat.st_mode)) {
			request.setRootPath(indexPath);
			file = readRequest(request);
			break;
		}
	}
	if (file.empty())
		throw FileNotExist();
	return (file);
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

bool	VServ::fileIsCGI(HttpRequest &request) {
	std::istringstream stream(request.getPath());
	std::string segment;

	while (std::getline(stream, segment, '/')) {
		if (segment.find(".php") != std::string::npos)
			return (true);		
	}
	return (false);	
}

std::string VServ::getPagePath(HttpRequest &request) {
	std::size_t	startDelPos;
	std::string	path;
	std::vector<std::string> delimiters;

	delimiters.push_back(".html");
	delimiters.push_back(".php");
	delimiters.push_back(".py");
	delimiters.push_back(".pl");
	delimiters.push_back("?");
	delimiters.push_back("#");

	path = request.getPath();
	for (std::size_t i = 0, n = delimiters.size(); i < n; i++) {
		std::string del = delimiters[i];
		if ((startDelPos = path.find(del)) != std::string::npos) {
			if (del == "?" || del == "#")
				return (path.substr(0, startDelPos - del.size()));
			else	
				return (path.substr(0, startDelPos + del.size()));
		}
	}
	return (path);
}

const char**	VServ::makeEnvp(HttpRequest &request) {
	size_t startPos;
	size_t endPos;

	std::string ext = ".php"; //tmp, getfrom config

	std::string path = request.getPath();
	if ((startPos = path.find(ext)) != std::string::npos) {
		startPos += ext.size();
		if ((endPos = path.find('?')) == std::string::npos)
			endPos = path.size();
	} else
		throw ExtensionNotFound();

	std::string path_info = "PATH_INFO=" + path.substr(startPos, endPos - startPos);
	std::string query_string = "QUERY_STRING=" + path.substr(endPos); 
	
	const char** exec_envp = new const char*[3];
	exec_envp[0] = path_info.c_str();
	exec_envp[1] = query_string.c_str();
	exec_envp[2] = NULL;

	if (_debug) {
		std::cout << exec_envp[0] << std::endl;
		std::cout << exec_envp[1] << std::endl;
	}

	return (exec_envp);
}



std::string	VServ::handleCGI(std::string &fileData, HttpRequest &request) {
	int			parentToChild[2], childToParent[2];
	pid_t		pid;
	const char	**envp;
	std::string	result;
	int 		status;

	//TMP
	std::string interpreter = "/usr/bin/php-cgi";

	if (pipe(parentToChild) < 0 || pipe(childToParent) < 0)
		throw PipeException();
	if ((pid = fork()) < 0)
		throw ForkException();

	if (pid == 0) {
		close(childToParent[0]);
		close(parentToChild[1]);

		fcntl(childToParent[1], F_SETFL, O_NONBLOCK);
		
		if (dup2(childToParent[1], STDOUT_FILENO) < 0)
			std::cerr << "dup2 failed" << std::endl;
		close(childToParent[1]);

		if (dup2(parentToChild[0], STDIN_FILENO) < 0)
			std::cerr << "dup2 failed" << std::endl;
		close(parentToChild[0]);
		
		char* argv[] = {
			const_cast<char*>(interpreter.c_str()),
			NULL
		};

		envp = makeEnvp(request);
		if (execve(interpreter.c_str(), argv, const_cast<char *const *>(envp)) < 0) {
			std::cout << strerror( errno ) << std::endl;
			//return ("OHHHH ?");
		}
		
	} else {
		close(parentToChild[0]);
		close(childToParent[1]);

		if (write(parentToChild[1], fileData.c_str(), fileData.size() + 1) < 0)
			std::cerr << "Fail to write into parentToChild pipe" << std::endl;
		close(parentToChild[1]);

		waitpid(pid, &status, 0);
		if (WIFEXITED(status)) {
			result = readFile(childToParent[0]);
		} else {
			result = "";
		}
		close(childToParent[0]);
	}
	
	return (result);
}

void	VServ::processRequest(std::string rawRequest, int clientFd) {
	HttpRequest request(rawRequest);
	HttpRequest response;
	struct stat path_stat;


	//DEFINE IN THE CONFIG
	//...
	bool _autoIndex = false;
	//...
	//END DEFINE IN CONFIG

	try {
		handleBigRequest(request);

		std::string rootPath = makeRootPath(request);
		if (_debug)
			std::cout << rootPath << std::endl;

		if (stat(rootPath.c_str(), &path_stat) != 0)
			throw FileNotExist();

		if (S_ISREG(path_stat.st_mode)) {

			std::string rawResponse = readRequest(request);
			response = HttpRequest(rawResponse);			

		} else if (S_ISDIR(path_stat.st_mode)) {

			DIR* dir = opendir(rootPath.c_str());
			if (!dir)
				throw OpenFolderException();

			if (_autoIndex) {
				showDirectory(dir, response);
			} else {
				std::string rawResponse = readDefaultPages(request);
				response = HttpRequest(rawResponse);			
			}

		} else {
			response.setResponseCode(HTTP_BAD_GATEWAY);
		}

	} catch (FileNotExist& e) {
		std::cerr << e.what() << std::endl;
		response.makeError(HTTP_NOT_FOUND);
	} catch (OpenFileException& e) {
		std::cerr << e.what() << std::endl;
		response.makeError(HTTP_FORBIDDEN);
	} catch (OpenFolderException& e) {
		std::cerr << e.what() << std::endl;
		response.makeError(HTTP_FORBIDDEN);
	} catch (EntityTooLarge& e) {
		std::cerr << e.what() << std::endl;
		response.makeError(HTTP_PAYLOAD_TOO_LARGE);
	} catch (RecvException& e) {
		std::cerr << e.what() << std::endl;
		response.makeError(HTTP_INTERNAL_SERVER_ERROR);
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

const char*	VServ::ExtensionNotFound::what() const throw() {
	return ("Error, extension for the cgi is not found on request path");
}

const char*	VServ::PipeException::what() const throw() {
	return ("Error, the pipe function make an excetion");
}

const char*	VServ::ForkException::what() const throw() {
	return ("Error, the fork function make an exception");
}

std::ostream&	operator<<(std::ostream& os, const VServ& vs) {
	os	<< "----------- VSERV -----------" << std::endl
		<< "\tport:\t" << vs.getPort() << std::endl
		<< "\thost:\t" << vs.getHost() << std::endl
		<< "\tfd:\t" << vs.getFd() << std::endl;
	// ...
	return (os);
}