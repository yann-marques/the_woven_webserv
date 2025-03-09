#include "VServ.hpp"

// VServ::VServ();


VServ::VServ(WebServ *mainInstance, int port, const std::map< std::string, Rules* >& rules, int maxClients, std::set<std::string> argv, std::set<std::string> envp): _maxClients(maxClients) {
	// tmp
	_port = port;
//	_host = config.getHost();
// parse config ...
	_rules = rules;
	_mainInstance = mainInstance;
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
	//	_port = rhs.getPort();
	//	_host = rhs.getHost();
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
	_address.sin_addr.s_addr = htonl(INADDR_ANY);
	_address.sin_port = htons(_port);
}

// GETTERS

int	VServ::getFd() const {
	return (_fd);
}

// METHODS

std::vector<std::string> split (const std::string &s, char delim) {
	std::vector<std::string> result;
	std::stringstream ss(s);
	std::string item;

	while (getline(ss, item, delim)) {
		result.push_back(item);
	}

	return result;
}

void	VServ::setTargetRules(HttpRequest &req) {
	std::string httpHost = req.getHeader("Host");

	//std::cout << req.getRawHeaders() << std::endl;
	if (httpHost.empty()) {
		std::cout << std::endl << "Host not found on http header." << std::endl;
		httpHost = "localhost:";
		//throw ServerNameNotFound();
	}

	std::string serverName = split(httpHost, ':')[0];

	if (!_rules.count(serverName))
		throw ServerNameNotFound();
	Rules*	ptr = _rules[serverName];

	std::vector<std::string> vec = split(req.getPath(), '/');
	for (size_t i = 0, n = vec.size(); i < n; i++) {
		if (!vec[i].empty()) {
			std::map<std::string, Rules *> locations = ptr->getLocation();
			if (!locations.count("/" + vec[i])) //location not found
				break;
			else
				ptr = locations["/" + vec[i]];
		}
	}
	req.setRules(ptr);
}

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

std::string    ip_convert(uint32_t n) {
    std::ostringstream    oss;
    std::string    str;

    oss << (n >> 24 & 0xFF) << '.'
        << (n >> 16 & 0xFF) << '.'
        << (n >> 8 & 0xFF) << '.'
        << (n & 0xFF);
    str = oss.str();
    return (str);
}

int	VServ::clientAccept(void) {
	sockaddr_in clientAddress;
	socklen_t clientAddressLength = sizeof(clientAddress);
	int clientFd = accept(_fd, (struct sockaddr*)&clientAddress, &clientAddressLength);
	if (clientFd < 0)
		throw (AcceptException());
	
	std::cout << ip_convert(ntohl(clientAddress.sin_addr.s_addr)) << ' ';
	return (clientFd);
}

std::string	VServ::makeRootPath(HttpRequest &request) {
	std::string rqRootPath = request.getRootPath();
	std::string locationPath = request.getRules()->getLocationPath();
	std::string rqPath = request.getPath().substr(locationPath.size());

	if (!rqRootPath.empty())
		return (rqRootPath);
	if (rqPath == "/" || rqPath.empty())
		return(request.getRules()->getRoot()); /////
	else
		return (request.getRules()->getRoot() + rqPath); /////
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
		std::cout << "file is a cgi" << std::endl;
		cgiContent = handleCGI(fileData, request);
		return (cgiContent);
	}

    return (fileData);
}

bool isHttpRequestComplete(const std::string &rawRequest) {

	size_t headerEnd = rawRequest.find("\r\n\r\n");
	if (headerEnd == std::string::npos) {
		return false;  // Headers not fully received yet
	}

	size_t contentLengthPos = rawRequest.find("Content-Length: ");
	if (contentLengthPos != std::string::npos) {
		size_t start = contentLengthPos + 16;  // Skip "Content-Length: "
		size_t end = rawRequest.find("\r\n", start);
		if (end != std::string::npos) {
			int contentLength = atoi(rawRequest.substr(start, end - start).c_str());
			size_t bodyStart = headerEnd + 4;  // "\r\n\r\n"
			if (rawRequest.size() >= bodyStart + contentLength) {
				return true;  // Full request (headers + body) received
			}
			return false;  // Waiting for more body data
		}
	} /* else { //remove this block if bug for transfer-encoding: chuncked
		if (rawRequest.find("\r\n") == std::string::npos)
			return false;
	}
 */
	return true;
}

ssize_t	VServ::readSocketFD(int fd, std::string &buffer) {
	std::string&	str = _clientBuffers[fd];
	ssize_t		bytesRead;
	char 		tempBuffer[1024];

	buffer = "";	
	while (true) {
		bytesRead = read(fd, tempBuffer, sizeof(tempBuffer));
		
		if (bytesRead > 0) {
			str.append(tempBuffer, bytesRead);
			//std::cout << "Lopp: [" << str << "]\n\n\n" << std::endl;
		} else {
			if (bytesRead == 0) { //client close connection;
				_clientBuffers.erase(fd);
				_mainInstance->deleteFd(fd);
			}
			break;
		}
	}

	std::string finalStr = std::string(str);
	if (isHttpRequestComplete(finalStr)) {
		_clientBuffers.erase(fd);
		buffer = finalStr;
	}

	return bytesRead;
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
		std::cout << "RESPONSE --------" << std::endl << "{" << rawResponse << "}" << std::endl << std::endl << std::endl << std::endl;

	ssize_t bytesSent = send(clientFd, rawResponse.c_str(), rawResponse.size(), 0);
	if (bytesSent == -1) {
		throw SendException();
	} else if (bytesSent < static_cast<ssize_t>(rawResponse.size())) {
    	throw SendPartiallyException();
	}
}

void	VServ::handleBigRequest(HttpRequest &request) {
	size_t client_max_body_size = request.getRules()->getMaxBodyBytes();

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
	std::vector<std::string> defaultPages = request.getRules()->getDefaultPages();
	bool	defaultPageIsFind = false;

	std::string file;
	for (std::vector<std::string>::iterator it = defaultPages.begin(); it != defaultPages.end(); it++) {
		std::string rootPath = makeRootPath(request);
		std::string indexPath;
		if (rootPath[rootPath.size() - 1] != '/')
			indexPath = rootPath + "/" + (*it);
		else
			indexPath = rootPath + (*it);	

		std::cout << "Index path: " << indexPath << std::endl;
		struct stat path_stat;
		if (stat(indexPath.c_str(), &path_stat) == 0 && S_ISREG(path_stat.st_mode)) {
			defaultPageIsFind = true;
			request.setRootPath(indexPath);
			file = readRequest(request);
			break;
		}
	}
	if (!defaultPageIsFind)
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

	std::set<std::string> cgiPaths = request.getRules()->getCgiKeys();
	while (std::getline(stream, segment, '.')) {
		if (cgiPaths.find("." + segment) != cgiPaths.end())
		{
			request.setCgiExt("." + segment);
			return (true);		
		}
	}
	request.setCgiExt("");
	return (false);	
}

std::vector<char*>	VServ::makeEnvp(HttpRequest &request) {
	size_t startPos;
	size_t endPos;
	std::vector<char*> env;

	std::string ext = request.getCgiExt();
	std::string path = request.getPath();

	if ((startPos = path.find(ext)) != std::string::npos) {
		startPos += ext.size();
		if ((endPos = path.find('?')) == std::string::npos)
			endPos = path.size();
	} else
		throw ExtensionNotFound();

	std::string afterExt = path.substr(startPos, endPos - startPos);
	std::string path_info;
	afterExt[0] != '/' ? path_info = "PATH_INFO=/" + afterExt : path_info = "PATH_INFO=" + afterExt;

	std::cerr << path_info << std::endl;
	std::string query_string = "QUERY_STRING=" + path.substr(endPos);
	
    // Add standard CGI environment variables
    env.push_back( strdup(("REQUEST_METHOD=" + request.getMethod()).c_str()) );
	env.push_back( strdup(("SERVER_PROTOCOL=" + request.getVersion()).c_str()) );
	std::string rootPath = makeRootPath(request);
    env.push_back( strdup(("SCRIPT_FILENAME=" + rootPath).c_str()) );

	//ADD REQUEST HEADERS
	std::map<std::string, std::string> headers = request.getHeaders();
	for (std::map<std::string, std::string>::iterator header = headers.begin(); header != headers.end(); header++) {
        std::string envVar = "HTTP_" + header->first;
        for (std::string::iterator it = envVar.begin(); it != envVar.end(); it++) *it = toupper(*it);
        std::replace(envVar.begin(), envVar.end(), '-', '_');
        std::string envValue = envVar + "=" + header->second;
        env.push_back(strdup(envValue.c_str()));
    }
	
	//https://stackoverflow.com/questions/24378472/what-is-php-serverredirect-status
	env.push_back(strdup("REDIRECT_STATUS=CGI"));
	
	env.push_back(strdup(path_info.c_str()));
	env.push_back(strdup(query_string.c_str()));
	env.push_back(NULL);
	return (env);
}

std::string	VServ::handleCGI(std::string &fileData, HttpRequest &request) {
	int					parentToChild[2], childToParent[2];
	pid_t				pid;
	std::vector<char*>	env;
	std::string			result;
	int 				status;

	std::map< std::string, std::string > cgiPaths = request.getRules()->getCgiPath(); 
	std::string interpreter = cgiPaths[request.getCgiExt()];
	std::cout << interpreter << std::endl;
	if (interpreter.empty())
		throw InterpreterEmpty();

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

		env = makeEnvp(request);
		if (execve(interpreter.c_str(), argv, env.data()) < 0) {
			throw ExecveException();
		}
		
	} else {
		close(parentToChild[0]);
		close(childToParent[1]);

		if (write(parentToChild[1], fileData.c_str(), fileData.size() + 1) < 0)
			std::cerr << "Fail to write into parentToChild pipe" << std::endl;
		close(parentToChild[1]);

		waitpid(pid, &status, 0);
		if (WIFEXITED(status)) {
			std::cout << status << std::endl;
			result = readFile(childToParent[0]);
			std::cout << "Result CGI [" << result << "]" << std::endl;
		} else {
			result = "";
		}
		close(childToParent[0]);
	}
	
	return (result);
}

void	VServ::checkAllowedMethod(HttpRequest& request) {
	std::string method = request.getMethod();
	std::transform(method.begin(), method.end(), method.begin(), ::toupper);

	std::vector<std::string> allowedMethods = request.getRules()->getAllowedMethods();
	for (size_t i = 0, n = allowedMethods.size(); i < n; i++) {
		std::transform(allowedMethods[i].begin(), allowedMethods[i].end(), allowedMethods[i].begin(), ::toupper);
		if (allowedMethods[i] == method)
			return ;
	}
	throw MethodNotAllowed();
} 

void	VServ::processRequest(std::string rawRequest, int &clientFd) {
	struct stat path_stat;	
	HttpRequest request;
	HttpRequest response;
	
	try {
		request = HttpRequest(HTTP_REQUEST, rawRequest);

		std::string reqMethod = request.getMethod();
		response.setMethod(reqMethod);
	
		if (!_debug)
			request.log();
		
		setTargetRules(request);
		checkAllowedMethod(request);
		handleBigRequest(request);

		std::string rootPath = makeRootPath(request);
		if (_debug)
			std::cout << rootPath << std::endl;

		if (stat(rootPath.c_str(), &path_stat) != 0)
			throw FileNotExist();
		
		if (S_ISREG(path_stat.st_mode)) {

			std::string rawResponse = readRequest(request);
			response = HttpRequest(HTTP_RESPONSE, rawResponse);	
			response.setMethod(reqMethod);

		} else if (S_ISDIR(path_stat.st_mode)) {

			DIR* dir = opendir(rootPath.c_str());
			if (!dir)
				throw OpenFolderException();

			if (request.getRules()->getAutoIndex()) {
				showDirectory(dir, response);
			} else {
				std::string rawResponse = readDefaultPages(request);
				response = HttpRequest(HTTP_RESPONSE, rawResponse);	
				response.setMethod(reqMethod);
			}

		} else {
			response.setResponseCode(HTTP_BAD_GATEWAY);
		}

	} catch (FileNotExist& e) {
		response.makeError(HTTP_NOT_FOUND);
	} catch (OpenFileException& e) {
		response.makeError(HTTP_FORBIDDEN);
	} catch (OpenFolderException& e) {
		response.makeError(HTTP_FORBIDDEN);
	} catch (EntityTooLarge& e) {
		response.makeError(HTTP_PAYLOAD_TOO_LARGE);
	} catch (RecvException& e) {
		response.makeError(HTTP_INTERNAL_SERVER_ERROR);
	} catch (ServerNameNotFound& e) {
		std::cout << "Server name not found" << std::endl;
		return ; //abort. send nothing
	} catch (InterpreterEmpty& e) {
		response.makeError(HTTP_INTERNAL_SERVER_ERROR);
	} catch (ExecveException& e) {
		response.makeError(HTTP_INTERNAL_SERVER_ERROR);
	} catch (MethodNotAllowed& e) {
		response.makeError(HTTP_METHOD_NOT_ALLOWED);
	} catch (HttpRequest::MalformedHttpHeader& e) {
		response.makeError(HTTP_BAD_REQUEST);
	}

	sendRequest(response, clientFd);

	std::string connectionType = request.getHeader("Connection");
	if (connectionType.empty() || (connectionType.find("keep-alive") == std::string::npos)) {
		std::cout << "The request has been closed." << std::endl;	
		_mainInstance->deleteFd(clientFd);
	}
}
