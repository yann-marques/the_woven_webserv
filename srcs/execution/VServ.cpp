#include "VServ.hpp"

// VServ::VServ();


VServ::VServ(WebServ *mainInstance, std::string host, int port, const std::map< std::string, Rules* >& rules, int maxClients, std::set<std::string> argv, std::set<std::string> envp): _maxClients(maxClients) {
	// tmp
	_host = host;
	_port = port;
//	_host = config.getHost();
//	parse config ...
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
	if (_fd != -1)
		close(_fd);
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
	fcntl(_fd, F_SETFL, O_NONBLOCK);

	int opt = 1;
	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
		throw (SetSockOptException());

	if (bind(_fd, (struct sockaddr*)&_address, sizeof(_address)) == -1)
		throw (BindException());

	if (listen(_fd, _maxClients) == -1) //mettre dans le epoll
		throw (ListenException());
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
		return(request.getRules()->getRoot());
	else
		return (request.getRules()->getRoot() + rqPath);
}

std::string VServ::readFile(int fd) {
	ssize_t bytesRead;
	char tempBuffer[4096];
	std::string result;

	while ((bytesRead = read(fd, tempBuffer, sizeof(tempBuffer))) > 0) {
		result.append(tempBuffer, bytesRead);
	}
	if (bytesRead < 0 && result.empty()) {
		close(fd);
		throw OpenFileException();
	}	
	close(fd);
	return result;
}

std::string VServ::readRequest(HttpRequest &request) {
	std::string body;
	std::string cgiContent;
	int			fileFd;
	bool		isReqCGI = fileIsCGI(request);
	
	std::string rootPath = makeRootPath(request);
	if (_cachedPages.count(rootPath) > 0)
		body = _cachedPages[rootPath]; 
	else {
		fileFd = open(rootPath.c_str(), O_RDONLY);
		if (fileFd < 0)
			throw OpenFileException();
		body = readFile(fileFd);
		_cachedPages[rootPath] = body;
	}

	if (isReqCGI) {
		cgiContent = handleCGI(body, request);
		return (cgiContent);
	}

    return (body);
}

bool VServ::isEndedChunckReq(std::string rawRequest) {
	if (rawRequest.find("0\r\n\r\n") != std::string::npos) { //last chunked request
		return (true);
	}
	return (false);	
}

bool VServ::isHttpRequestComplete(const std::string &rawRequest) {

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
	} else {
		if (rawRequest.find("Transfer-Encoding: chunked") != std::string::npos
			&& !isEndedChunckReq(rawRequest))
			return false;
	}

	return true;
}

ssize_t	VServ::readSocketFD(int fd, std::string &buffer) {
	std::string&	str = _clientBuffers[fd];
	ssize_t		bytesRead;
	char 		tempBuffer[4096];

	buffer = "";	
	while (true) {
		bytesRead = read(fd, tempBuffer, sizeof(tempBuffer));
		
		if (bytesRead > 0) {
			str.append(tempBuffer, bytesRead);
		} else {
			if (bytesRead == 0) { //client close connection;
				_clientBuffers.erase(fd);
				_mainInstance->deleteFd(fd);
				return (bytesRead);
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


void	VServ::sendRequest(HttpRequest &response, int clientFd) {
	std::string	rawResponse = response.makeRawResponse();

	if (_debug)
		std::cout << "RESPONSE --------" << std::endl << "{" << rawResponse << "}" << std::endl << std::endl << std::endl << std::endl;

	ssize_t totalBytesSent = 0;
	ssize_t bytesSent = 0;
	const char* data = rawResponse.c_str();
	size_t dataSize = rawResponse.size();
	
	while (totalBytesSent < static_cast<ssize_t>(dataSize)) {
		bytesSent = send(clientFd, data + totalBytesSent, dataSize - totalBytesSent, 0);
		totalBytesSent += bytesSent;
	}
	
	if (totalBytesSent != static_cast<ssize_t>(dataSize))
		throw SendPartiallyException();
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
		if (!body.empty() && body.size() > client_max_body_size)
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

void	VServ::showDirectory(HttpRequest &request, HttpRequest &response) {
	struct dirent* entry;
	std::vector<std::string> filesName;

	std::string rootPath = makeRootPath(request);
	DIR* dir = opendir(rootPath.c_str());
	if (!dir)
		throw OpenFolderException();
	
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
		if (cgiPaths.find("." + segment) != cgiPaths.end()) {
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
	
	env.push_back(strdup("GATEWAY_INTERFACE=CGI/1.1"));

	env.push_back(strdup(path_info.c_str()));
	env.push_back(strdup(query_string.c_str()));
	env.push_back(NULL);
	return (env);
}

std::string	VServ::handleCGI(std::string &body, HttpRequest &request) {
	int					parentToChild[2], childToParent[2];
	pid_t				pid;
	std::vector<char*>	env;
	std::string			result;
	int 				status;
	ssize_t				bytesWritten = 0;
	ssize_t				totalBytesWritten = 0;
	const ssize_t 		chunckSize = 16384;  // 64 KB chunk size for cross platform pipe buff limit

	std::map< std::string, std::string > cgiPaths = request.getRules()->getCgiPath(); 
	std::string interpreter = cgiPaths[request.getCgiExt()];

	if (interpreter.empty()) throw InterpreterEmpty();

	if (pipe(parentToChild) < 0 || pipe(childToParent) < 0) throw PipeException();
	if ((pid = fork()) < 0) throw ForkException();

	fcntl(childToParent[1], F_SETFL, O_NONBLOCK);
	fcntl(parentToChild[0], F_SETFL, O_NONBLOCK);
	fcntl(parentToChild[1], F_SETFL, O_NONBLOCK);
	fcntl(childToParent[0], F_SETFL, O_NONBLOCK);	
	
	if (pid == 0) {
		close(childToParent[0]);
		close(parentToChild[1]);

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
		if (execve(interpreter.c_str(), argv, env.data()) < 0)
			throw ExecveException();
	} else {
		close(parentToChild[0]);
		close(childToParent[1]);

        int epollFd = epoll_create(2);
        if (epollFd == -1) {
            std::cerr << "epoll_create failed: " << strerror(errno) << std::endl;
            return "";
        }

		struct epoll_event eventWrite, eventRead;
		eventWrite.events = EPOLLOUT | EPOLLET; 
		eventWrite.data.fd = parentToChild[1];

		eventRead.events = EPOLLIN | EPOLLET;
		eventRead.data.fd = childToParent[0];

        if (epoll_ctl(epollFd, EPOLL_CTL_ADD, parentToChild[1], &eventWrite) == -1 ||
			epoll_ctl(epollFd, EPOLL_CTL_ADD, childToParent[0], &eventRead) == -1) {
				throw EpollCTLException();
        }

		ssize_t bodySize = static_cast<ssize_t>(body.size());
		const char *bodyCStr = body.c_str();
		bool endOfWritting = false, endOfReading = false;

		while (!(endOfWritting && endOfReading)) {
			struct epoll_event events[2];
            int nfds = epoll_wait(epollFd, events, 2, -1);
            if (nfds < 0)
                throw EpollWaitException();

			for (int i = 0; i < nfds; i++) {
				int fd = events[i].data.fd;
				if (fd == parentToChild[1] && !endOfWritting) {
					while (totalBytesWritten < bodySize) {
						ssize_t bytesToWrite = totalBytesWritten + chunckSize < bodySize ? chunckSize : bodySize - totalBytesWritten;
						bytesWritten = write(fd, bodyCStr + totalBytesWritten, bytesToWrite);
						if (bytesWritten > 0)
							totalBytesWritten += bytesWritten;
						else
							break ;
					}
					if (totalBytesWritten >= bodySize) {
						endOfWritting = true;
						close(fd);
					}
				}

				if (fd == childToParent[0] && !endOfReading) {
					char buffer[4096];
					ssize_t bytesRead;

					while ((bytesRead = read(childToParent[0], buffer, sizeof(buffer))) > 0) {
						result.append(buffer, bytesRead);
					}
					if (bytesRead == 0) {
						endOfReading = true;
						close(fd);
					}
				}
			}
		}

		waitpid(pid, &status, 0);
		close(parentToChild[1]);
		close(childToParent[0]);
		close(epollFd);
		if (WEXITSTATUS(status) != EXIT_SUCCESS)
			throw ChildProcessException();
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


void	VServ::uploadFile(HttpRequest request, std::string content) {
	std::string uploadFile = request.getRules()->getUpload();
	std::string locationPath = request.getRules()->getLocationPath();
	std::string rqPath = request.getPath().substr(locationPath.size());

	if (rqPath == "/" || rqPath.empty())
		throw NoUploadFileName();

	std::string separator = uploadFile[uploadFile.size() - 1] == '/' ? "" : "/";
	std::string filePath = uploadFile + separator + rqPath;		
	std::cout << filePath << std::endl;
	std::ofstream outFile(filePath.c_str());
    if (!outFile)
		throw CreateFileException(); 
    outFile << content;
    outFile.close();	
}

void	VServ::processRequest(std::string rawRequest, int &clientFd) {
	struct stat path_stat;	
	HttpRequest request;
	HttpRequest response;	

	try {
		request = HttpRequest(HTTP_REQUEST, rawRequest);
		setTargetRules(request);
		
		std::string reqMethod = request.getMethod();
		response.setMethod(reqMethod);
		
		if (!_debug)
			request.log();
		
		checkAllowedMethod(request);
		handleBigRequest(request);

		std::string rootPath = makeRootPath(request);
		if (_debug)
			std::cout << rootPath << std::endl;

		if (reqMethod == GET || reqMethod == HEAD) {
			if (stat(rootPath.c_str(), &path_stat) != 0)
				throw FileNotExist();
			
			if (S_ISREG(path_stat.st_mode)) {

				std::string rawResponse = readRequest(request);
				response = HttpRequest(HTTP_RESPONSE, rawResponse);	

			} else if (S_ISDIR(path_stat.st_mode)) {

				if (request.getRules()->getAutoIndex()) {
					showDirectory(request, response);
				} else {
					std::string rawResponse = readDefaultPages(request);
					response = HttpRequest(HTTP_RESPONSE, rawResponse);	
				}

			} else {
				response.setResponseCode(HTTP_BAD_GATEWAY);
			}
		}

		if (reqMethod == POST) {
			std::string requestBody = request.getBody();
			if (fileIsCGI(request)) {
				std::string cgiContent = handleCGI(requestBody, request);
				response = HttpRequest(HTTP_RESPONSE, cgiContent);
			} else {
				response = HttpRequest(HTTP_RESPONSE, requestBody);
				if (!request.getRules()->getUpload().empty())
					uploadFile(request, requestBody);
			}
		}

		if (reqMethod == DELETE) {
			std::string emptyContent;
			response = HttpRequest(HTTP_RESPONSE, emptyContent);

			if (remove(rootPath.c_str()) == 0) {
				response.setResponseCode(204);	
			} else 
				throw FileNotExist();

		}


	} catch (FileNotExist& e) {
		response.makeError(HTTP_NOT_FOUND, request);
	} catch (OpenFileException& e) {
		response.makeError(HTTP_FORBIDDEN, request);
	} catch (OpenFolderException& e) {
		response.makeError(HTTP_FORBIDDEN, request);
	} catch (EntityTooLarge& e) {
		response.makeError(HTTP_PAYLOAD_TOO_LARGE, request);
	} catch (RecvException& e) {
		response.makeError(HTTP_INTERNAL_SERVER_ERROR, request);
	} catch (ServerNameNotFound& e) {
		response.makeError(HTTP_INTERNAL_SERVER_ERROR, request);
	} catch (InterpreterEmpty& e) {
		response.makeError(HTTP_INTERNAL_SERVER_ERROR, request);
	} catch (ExecveException& e) {
		response.makeError(HTTP_INTERNAL_SERVER_ERROR, request);
	} catch (MethodNotAllowed& e) {
		response.makeError(HTTP_METHOD_NOT_ALLOWED, request);
	} catch (HttpRequest::MalformedHttpHeader& e) {
		response.makeError(HTTP_BAD_REQUEST, request);
	} catch (CreateFileException& e) {
		response.makeError(HTTP_INTERNAL_SERVER_ERROR, request);
	} catch (NoUploadFileName& e) {
		response.makeError(HTTP_BAD_REQUEST, request);
	} catch (ChildProcessException& e) {
		response.makeError(HTTP_INTERNAL_SERVER_ERROR, request);
	} catch (PipeException& e) {
		response.makeError(HTTP_INTERNAL_SERVER_ERROR, request);
	} catch (ForkException& e) {
		response.makeError(HTTP_INTERNAL_SERVER_ERROR, request);
	} catch (ExtensionNotFound& e) {
		response.makeError(HTTP_INTERNAL_SERVER_ERROR, request);
	} catch (EpollWaitException& e) {
		response.makeError(HTTP_INTERNAL_SERVER_ERROR, request);
	} catch (EpollCTLException& e) {
		response.makeError(HTTP_INTERNAL_SERVER_ERROR, request);
	}

	sendRequest(response, clientFd);

	std::string connectionType = request.getHeader("Connection");
	if (!connectionType.empty() || (connectionType.find("close") != std::string::npos))  {
		_mainInstance->deleteFd(clientFd);
	}
}
