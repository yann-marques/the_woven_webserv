/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VServ.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: locharve <locharve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 14:10:53 by locharve          #+#    #+#             */
/*   Updated: 2025/03/31 14:10:53 by locharve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "VServ.hpp"

std::ofstream&	operator<<(std::ofstream& ofs, const t_binary& rhs) {
	for (size_t i = 0, n = rhs.size(); i < n; i++)
		ofs << rhs[i];
	return (ofs);
}

VServ::VServ(): _maxClients(1000) {
	_fd = -1;
}

uint32_t	ip_to_uint32_t(const char*	ipStr) {
	uint32_t	result = 0;
	size_t		i = 0;

	while (ipStr && ipStr[i]) {
		result += std::atoi(&ipStr[i]);
		while (ipStr[i] && isdigit(ipStr[i]))
			i++;
		if (ipStr[i]) {
			result = (result << 8);
			i++;
		}
	}
	return (result);
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

VServ::VServ(WebServ *mainInstance, std::string host, int port, const std::map< std::string, Rules* >& rules, int maxClients, std::set<std::string> argv, std::set<std::string> envp): _maxClients(maxClients) {
	_host = host;
	_port = port;
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

VServ::VServ(const VServ& rhs): _maxClients(rhs._maxClients) {
	*this = rhs;
}

VServ&	VServ::operator=(const VServ& rhs) {
	_fd = rhs.getFd();
	_host = rhs._host;
	_port = rhs._port;
	setAddress();
	return (*this);
}

VServ::~VServ() {
	if (_fd != -1)
		close(_fd);
}

// SETTERS

void	VServ::setAddress() {
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = htonl(ip_to_uint32_t(_host.c_str()));
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
	if (httpHost.empty())
		throw(BadRequestException());
	std::string serverName = split(httpHost, ':')[0];

	if (!_rules.count(serverName))
		throw (ServerNameNotFoundException());
	Rules*	ptr = _rules[serverName];
	_rules.count(serverName) == 0 ? ptr = _rules.begin()->second : ptr = _rules[serverName];
	 
	std::vector<std::string> vec = split(req.getPath(), '/');
	for (size_t i = 0, n = vec.size(); i < n; i++) {
		if (!vec[i].empty()) {
			std::map<std::string, Rules *> locations = ptr->getLocation();
			if (!locations.count("/" + vec[i]))
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
	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
		close(_fd);
		throw (SetSockOptException());
	}

	if (bind(_fd, (struct sockaddr*)&_address, sizeof(_address)) == -1) {
		close(_fd);
		throw (BindException());
	}

	if (listen(_fd, _maxClients) == -1) {
		close(_fd);
		throw (ListenException());
	}
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

void	VServ::eraseClient(int fd) {
	_clientRequestBuffer.erase(fd);
	_clientResponseBuffer.erase(fd);
	_clientRequests.erase(fd);
	_clientResponses.erase(fd);
	_clientHeaderEndPos.erase(fd);
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

t_binary VServ::readFile(std::string rootPath) {
	std::string result;
	std::string line;

	std::ifstream inputFile(rootPath.c_str(), std::ios_base::binary);
	
    inputFile.seekg(0, std::ios::end);
    std::streamsize length = inputFile.tellg();
    inputFile.seekg(0, std::ios::beg);

	if (length <= 0)
		return std::vector<unsigned char>();
 
	t_binary buffer(length);
	if (inputFile.fail()) {
		throw FileNotExist();
	} else {
		if (!inputFile.read(reinterpret_cast<char*>(buffer.data()), length))
			throw ReadFileException();
	}	

	return buffer;
}

void	VServ::readRequest(HttpRequest &request) {
	t_binary	body;
	
	std::string rootPath = makeRootPath(request);
	if (_cachedPages.count(rootPath) > 0)
		body = _cachedPages[rootPath]; 
	else {
		body = readFile(rootPath);
		_cachedPages[rootPath] = body;
	}

	request.setBody(body);
	request.setBodySize(body.size());
	int	clientFd = request.getClientFD();
	_clientRequests[clientFd] = request;
}

bool VServ::isEndedChunckReq(t_binary& clientBuffer) {
	std::string lastChunckSequence = "0\r\n\r\n";
	if (clientBuffer.size() < 10) return false;
	if (std::search(clientBuffer.end() - 10, clientBuffer.end(), lastChunckSequence.begin(), lastChunckSequence.end()) != clientBuffer.end()) { //last chunked request
		return (true);
	}
	return (false);	
}

bool VServ::isHttpRequestComplete(t_binary &clientBuffer, int clientFd) {
    static const std::string headerEndSeq = "\r\n\r\n";
    static const std::string contentLengthKey = "Content-Length: ";
    static const std::string transferEncodingKey = "Transfer-Encoding: chunked";

	if (_clientHeaderEndPos.count(clientFd) == 0) {
		t_binary::iterator it = std::search(clientBuffer.begin(), clientBuffer.end(), headerEndSeq.begin(), headerEndSeq.end());
		if (it == clientBuffer.end())
			return false;
		_clientHeaderEndPos[clientFd] = std::distance(clientBuffer.begin(), it) + 4; // "\r\n\r\n" length
	}

    // Search for "Content-Length" within headers (not entire buffer)
	size_t headerEnd = _clientHeaderEndPos[clientFd];
    t_binary::iterator headerEndIt = clientBuffer.begin() + headerEnd;
    t_binary::iterator contentLengthIt = std::search(clientBuffer.begin(), headerEndIt, contentLengthKey.begin(), contentLengthKey.end());

    if (contentLengthIt != headerEndIt) {  // Found "Content-Length"
        contentLengthIt += contentLengthKey.size();
        t_binary::iterator endPosIt = std::find(contentLengthIt, headerEndIt, '\r'); // Find end of line
        if (endPosIt != headerEndIt) {
            int contentLength = atoi(std::string(contentLengthIt, endPosIt).c_str());
            return clientBuffer.size() >= headerEnd + contentLength;
        }
    }

    // Check for "Transfer-Encoding: chunked"
    if (std::search(clientBuffer.begin(), headerEndIt, transferEncodingKey.begin(), transferEncodingKey.end()) != headerEndIt) {
		return isEndedChunckReq(clientBuffer); 
    }

    return true;
}


bool	VServ::readSocketFD(int fd) {
	t_binary&		clientBuffer = _clientRequestBuffer[fd];
	ssize_t			bytesRead;
	const ssize_t 	chunckSize = 65536;  // 64 KiB chunk size for (cross platform) pipe buff limit
	t_binary		tempBuffer(chunckSize);

	_mainInstance->epollCtlMod(fd, EPOLLIN);
	bytesRead = read(fd, tempBuffer.data(), chunckSize);
	if (bytesRead > 0) {
		clientBuffer.insert(clientBuffer.end(), tempBuffer.begin(), tempBuffer.begin() + bytesRead);
	} else {
		if (bytesRead == 0) {
			_mainInstance->deleteFd(fd);
			eraseClient(fd);
			return false;
		}
	}

	if (isHttpRequestComplete(clientBuffer, fd)) {
		return true;
	}

	return false;
}


bool	VServ::sendRequest(HttpRequest &response, int clientFd) {
	t_binary	rawResponse = response.makeRawResponse();
	
	ssize_t bytesSent = 0;
	size_t dataSize = rawResponse.size();

	bytesSent = send(clientFd, rawResponse.data() + _totalBytesSent[clientFd], dataSize - _totalBytesSent[clientFd], MSG_NOSIGNAL);
	if (bytesSent < 0) {
		_totalBytesSent[clientFd] = 0; 
		throw SigPipe();
	}

	_totalBytesSent[clientFd] += bytesSent;
	
	if (bytesSent == 0) {
		_totalBytesSent[clientFd] = 0;
		return (true);
	}
	return (false);
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
		t_binary body = request.getBody();
		if (!body.empty() && body.size() > client_max_body_size)
			throw EntityTooLarge(); 
	}
}

void	VServ::readDefaultPages(HttpRequest &request) 
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
			readRequest(request);
			break;
		}
	}
	if (!defaultPageIsFind)
		throw FileNotExist();
}

void	VServ::showDirectory(HttpRequest &request) {
	struct dirent* entry;
	std::vector<std::string> filesName;

	std::string rootPath = makeRootPath(request);
	DIR* dir = opendir(rootPath.c_str());
	if (!dir)
		throw OpenFolderException();
	
	while ((entry = readdir(dir)) != NULL) {
		std::string	reqPath = request.getPath();
		if (reqPath[reqPath.size() - 1] != '/')
			reqPath += '/';
		std::string	fileName = entry->d_name;
		if (entry->d_type == DT_DIR && fileName != "." && fileName != "..")
			fileName += '/';
		filesName.push_back(reqPath + fileName);
	}
	closedir(dir);
	request.generateIndexFile(filesName);
}

bool	VServ::isCGI(HttpRequest &request) {
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
	std::multimap<std::string, std::string> headers = request.getHeaders();
	for (std::map<std::string, std::string>::iterator header = headers.begin(); header != headers.end(); header++) {
        std::string envVar = "HTTP_" + header->first;
        for (std::string::iterator it = envVar.begin(); it != envVar.end(); it++) *it = toupper(*it);
        std::replace(envVar.begin(), envVar.end(), '-', '_');
        std::string envValue = envVar + "=" + header->second;
        env.push_back(strdup(envValue.c_str()));
    }
	
	env.push_back(strdup("REDIRECT_STATUS=CGI"));
	env.push_back(strdup("GATEWAY_INTERFACE=CGI/1.1"));

	env.push_back(strdup(path_info.c_str()));
	env.push_back(strdup(query_string.c_str()));
	env.push_back(NULL);
	return (env);
}

void	VServ::executeCGI(HttpRequest &request) {
	int					parentToChild[2], childToParent[2];
	pid_t				pid;
	std::vector<char*>	env;

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
		if (execve(interpreter.c_str(), argv, env.data()) < 0) {
			close(_fd);
			close(_mainInstance->getEpollFd());
			close(request.getClientFD());
			exit(EXIT_FAILURE);
		}
	} else {
		close(childToParent[1]);
		close(parentToChild[0]);

		_mainInstance->setVServ(parentToChild[1], this);
		_mainInstance->setVServ(childToParent[0], this);
		_mainInstance->setFdType(parentToChild[1], CGI_FD);
		_mainInstance->setFdType(childToParent[0], CGI_FD);
		_clientFdsPipeCGI[parentToChild[1]] = request.getClientFD();
		_clientFdsPipeCGI[childToParent[0]] = request.getClientFD();
		_mainInstance->epollCtlAdd(parentToChild[1], EPOLLOUT);
		_mainInstance->epollCtlAdd(childToParent[0], EPOLLIN); 
	}
}


void	VServ::talkToCgi(epoll_event event) {
	const ssize_t 		chunckSize = 65536;  // 64 KiB chunk size for (cross platform) pipe buff limit
	int 				fd = event.data.fd;
	int					clientFd = _clientFdsPipeCGI[fd];
	HttpRequest&		request = _clientRequests[clientFd];			
	const t_binary&		requestBody = request.getBody();
	std::size_t			bodySize = request.getBodySize();
	t_binary& 			clientResponseBuffer = _clientResponseBuffer[clientFd];
	t_binary  			readingBuffer(chunckSize);

	if (event.events & EPOLLOUT) {
		ssize_t bytesToWrite = _cgiBytesWriting[fd] + chunckSize < bodySize ? chunckSize : bodySize - _cgiBytesWriting[fd];
		ssize_t bytesWritten = write(fd, requestBody.data() + _cgiBytesWriting[fd], bytesToWrite);
		if (bytesWritten > 0) {
			_cgiBytesWriting[fd] += bytesWritten;
		} else {
			_cgiBytesWriting[fd] = 0;
			_clientFdsPipeCGI.erase(fd);
			_mainInstance->deleteFd(fd);
		}
	}	

	if (event.events & EPOLLIN) {
		ssize_t bytesRead = read(fd, readingBuffer.data(), chunckSize);
		if (bytesRead > 0) {
			clientResponseBuffer.insert(clientResponseBuffer.end(), readingBuffer.begin(), readingBuffer.begin() + bytesRead);
		}
	}
	
	if (!(event.events & EPOLLIN) && !(event.events & EPOLLOUT)) {
		_clientFdsPipeCGI.erase(fd);
		_mainInstance->deleteFd(fd);
		_mainInstance->epollCtlMod(clientFd, EPOLLOUT);
	}
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

void	VServ::uploadFile(HttpRequest request, t_binary content) {
	std::string uploadFile = request.getRules()->getUpload();
	std::string locationPath = request.getRules()->getLocationPath();
	std::string rqPath = request.getPath().substr(locationPath.size());

	if (rqPath == "/" || rqPath.empty()) {
		request.setResponseCode(200);
		return ;
	}

	std::string separator = uploadFile[uploadFile.size() - 1] == '/' ? "" : "/";
	std::string filePath = uploadFile + separator + rqPath;		
	std::ofstream outFile(filePath.c_str());
    if (!outFile)
		throw CreateFileException(); 
    outFile << content;
    outFile.close();
	request.setResponseCode(200);
}


bool	VServ::makeHttpRedirect(HttpRequest &request, HttpRequest &response) {
	Rules* reqRules = request.getRules();
	if (!reqRules)
		return (false);
	std::string redirectLocation = reqRules->getRedirect(); 
	if (!redirectLocation.empty() && redirectLocation[0] != '/')
		redirectLocation.insert(0, 1, '/');
	std::string	reqPath = request.getPath();
		
	if (!redirectLocation.empty() && reqPath != redirectLocation) {
		std::string responseStr = "HTTP/1.1 302 Not Found\r\nLocation: " +  redirectLocation + "\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
		t_binary rawResponse(responseStr.begin(), responseStr.end());
		response = HttpRequest(HTTP_RESPONSE, rawResponse);
		response.setResponseCode(302);
		return (true);	
	}
	return (false);
}

void	VServ::processRequest(int &clientFd) {
	struct stat path_stat;	
	HttpRequest request;

	try {
		if (!readSocketFD(clientFd))
			return ;

		t_binary clientRequestBuffer = _clientRequestBuffer[clientFd];
		request = HttpRequest(HTTP_REQUEST, clientRequestBuffer);
		request.setClientFD(clientFd);

		setTargetRules(request);
		std::string reqMethod = request.getMethod();
		
		request.log();
		checkAllowedMethod(request);
		handleBigRequest(request);
		
		std::string rootPath = makeRootPath(request);
		if (reqMethod == GET || reqMethod == HEAD) {
			if (stat(rootPath.c_str(), &path_stat) != 0) 
			throw FileNotExist();
			if (S_ISREG(path_stat.st_mode)) {
				readRequest(request);
			} else if (S_ISDIR(path_stat.st_mode)) {
				request.getRules()->getAutoIndex() ? showDirectory(request) : readDefaultPages(request);
			}
		}
		
		if (reqMethod == POST) {
			const t_binary& requestBody = request.getBody();
			if (!isCGI(request)) {
				if (!request.getRules()->getUpload().empty())
				uploadFile(request, requestBody);
				else
					throw MethodNotAllowed();
			}
		}
		
		if (reqMethod == DELETE) {
			remove(rootPath.c_str());
		}
		
		if (isCGI(request)) {
			_clientRequests[clientFd] = request;
			executeCGI(request);
			_mainInstance->epollCtlMod(clientFd, 0);
			return ;
		}

		_clientRequests[clientFd] = request;

	} catch (FileNotExist& e) {
		request.setResponseCode(HTTP_NOT_FOUND);
	} catch (OpenFileException& e) {
		request.setResponseCode(HTTP_FORBIDDEN);
	} catch (OpenFolderException& e) {
		request.setResponseCode(HTTP_FORBIDDEN);
	} catch (EntityTooLarge& e) {
		request.setResponseCode(HTTP_PAYLOAD_TOO_LARGE);
	} catch (RecvException& e) {
		request.setResponseCode(HTTP_INTERNAL_SERVER_ERROR);
	} catch (InterpreterEmpty& e) {
		request.setResponseCode(HTTP_INTERNAL_SERVER_ERROR);
	} catch (ExecveException& e) {
		request.setResponseCode(HTTP_INTERNAL_SERVER_ERROR);
	} catch (MethodNotAllowed& e) {
		request.setResponseCode(HTTP_METHOD_NOT_ALLOWED);
	} catch (HttpRequest::MalformedHttpHeader& e) {
		request.setResponseCode(HTTP_BAD_REQUEST);
	} catch (CreateFileException& e) {
		request.setResponseCode(HTTP_INTERNAL_SERVER_ERROR);
	} catch (NoUploadFileName& e) {
		request.setResponseCode(HTTP_BAD_REQUEST);
	} catch (ChildProcessException& e) {
		request.setResponseCode(HTTP_INTERNAL_SERVER_ERROR);
	} catch (PipeException& e) {
		request.setResponseCode(HTTP_INTERNAL_SERVER_ERROR);
	} catch (ForkException& e) {
		request.setResponseCode(HTTP_INTERNAL_SERVER_ERROR);
	} catch (ExtensionNotFound& e) {
		request.setResponseCode(HTTP_INTERNAL_SERVER_ERROR);
	} catch (EpollWaitException& e) {
		request.setResponseCode(HTTP_INTERNAL_SERVER_ERROR);
	} catch (EpollCTLException& e) {
		request.setResponseCode(HTTP_INTERNAL_SERVER_ERROR);
	} catch (EpollCreateException& e) {
		request.setResponseCode(HTTP_INTERNAL_SERVER_ERROR);
	} catch (ServerNameNotFoundException& e) {
		request.setResponseCode(HTTP_NOT_FOUND);
	} catch (BadRequestException& e) {
		request.setResponseCode(HTTP_BAD_REQUEST);
	}

	if (_clientRequests.count(clientFd) <= 0)
		_clientRequests[clientFd] = request;

	_mainInstance->epollCtlMod(clientFd, EPOLLOUT);
}

void VServ::processResponse(int &clientFd) {
	try {
		
		if (_clientResponses.count(clientFd) > 0) {
			HttpRequest& response = _clientResponses[clientFd];
			HttpRequest& request = _clientRequests[clientFd];
			try {
				if (sendRequest(response, clientFd)) {
					std::string connectionType = request.getHeader("Connection");
					eraseClient(clientFd);
					if (!connectionType.empty() && (connectionType.find("close") != std::string::npos))
						_mainInstance->deleteFd(clientFd);
					else
						_mainInstance->epollCtlMod(clientFd, EPOLLIN | EPOLLOUT);
				}
			} catch (SigPipe& e) {
				eraseClient(clientFd);
				_mainInstance->deleteFd(clientFd);
			}
			return ;
		}
		
		if (_clientRequests.count(clientFd) > 0) {
			HttpRequest response;
			HttpRequest& request = _clientRequests[clientFd];
			t_binary	reqBody = request.getBody();
			t_binary&	clientResponseBuffer = _clientResponseBuffer[clientFd];
			size_t		responseBufferSize = clientResponseBuffer.size();

			if (makeHttpRedirect(request, response)) {
				_clientResponses[clientFd] = response;
				return ;
			}

			int	requestResponseCode = request.getResponseCode();
			if (requestResponseCode != 0) {
				response.makeError(requestResponseCode, request);
			} else {
				if (responseBufferSize > 0) {
					response = HttpRequest(HTTP_RESPONSE, clientResponseBuffer);
				} else {
					response = HttpRequest(HTTP_RESPONSE, reqBody);
				}
			}
			# ifdef BONUS
					std::set< std::string >	requestCookies = request.getCookieSet();
					response.setResponseCookies(requestCookies);
			# endif
			_clientResponses[clientFd] = response;
		}

	
	} catch(std::exception &e) {
		std::cout << "Error: " << e.what() << std::endl; 
	}
}
