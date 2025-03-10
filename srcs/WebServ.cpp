#include "WebServ.hpp"

// WebServ::WebServ() {} // private ?

WebServ::WebServ(std::string filename, char **argv, char **envp): _maxClients(1000), _maxEvents(1000), _config(filename.c_str()) {
//	signal(SIGINT, handleSignal); // in execution

	try {
		// epoll init
		_epollFd = epoll_create(_maxClients + 1);
		if (_epollFd == -1)
			throw EpollCreateException();
	
		//_config = Config(filename.c_str());
		
		//parse envp and argv:
		std::set<std::string> arg;
		std::set<std::string> env;

		for (size_t i = 0, n = sizeof(argv); i < n; i++) {
			if (argv[i])
				arg.insert(argv[i]);
		}
		for (size_t i = 0, n = sizeof(envp); i < n; i++) {
			if (envp[i])
				env.insert(envp[i]);
		}

		this->_argv = arg;
		this->_envp = env;
		this->_debug = false;
		if (arg.find("--debug=yes") != arg.end())
			this->_debug = true;

		std::set< int >::iterator	portIt = _config.getPorts().begin(), portIte = _config.getPorts().end();
		while (portIt != portIte) {
			VServ*	vserv = new VServ(this, *portIt, _config.getParsedConfig().at(*portIt), _maxClients, _argv, _envp);

			//////
			int	sfd = vserv->getFd();

			//insertVServFd(sfd);
			_fds[sfd] = SERVER_SOCK;  
			setVServ(sfd, vserv);

			// set the event for sfd then epoll ctl the server fd
			epollCtlAdd(sfd, EPOLLIN);

			portIt++;
		}
/*	
		_serverNbr = _config.getServersNbr();
		std::cout << "serverNbr = " << _serverNbr << std::endl;
		for (size_t i = 0; i < _serverNbr; i++) { // c.serverConfig : container ? vector
			VServ*	server = new VServ(_config.getServerConfig(i), _maxClients, _argv, _envp); // epoll ? epollFd en arg ?
			int	sfd = server->getFd();

			insertServerFd(sfd);
			setServerToServerFd(sfd, server);

			// set the event for sfd then epoll ctl the server fd
			setEvent(EPOLLIN, sfd);
			epollCtlAdd(sfd);
		}
*/
		_epollEventsBuff.resize(_maxEvents);
		listenEvents();

	} catch (EpollCreateException& e) {
		std::cerr << e.what() << std::endl;
	} catch (EpollCtlAddException& e) {
		std::cerr << e.what() << std::endl;
	} catch (EpollCtlDelException& e) {
		std::cerr << e.what() << std::endl;
	} catch (UnknownFdException& e) {
		std::cerr << e.what() << std::endl;
	} catch (VServ::SocketException& e) {
		std::cerr << e.what() << std::endl;
	} catch (VServ::SetSockOptException& e) {
		std::cerr << e.what() << std::endl;
	} catch (VServ::BindException& e) {
		std::cerr << e.what() << std::endl;
	} catch (VServ::ListenException& e) {
		std::cerr << e.what() << std::endl;
	} catch (Config::OpenFileException& e) {
		std::cerr << e.what() << filename << std::endl;
	} catch (Config::ArgOutsideServerScopeException& e) {
		std::cerr << e.what() << std::endl;
	} catch (Config::UnclosedScopeException& e) {
		std::cerr << e.what() << std::endl;
	} catch (Config::ConfigSyntaxException& e) {
		std::cerr << e.what() << std::endl;
	} catch (Config::UnexpectedKeyException& e) {
		std::cerr << e.what() << std::endl;
	} catch (Config::DoubleArgException& e) {
		std::cerr << e.what() << std::endl;
	} catch (Config::MissingPortException& e) {
		std::cerr << e.what() << std::endl;
	} catch (Config::MultiplePortsException& e) {
		std::cerr << e.what() << std::endl;
	} catch (Config::UnexpectedValueException& e) {
		std::cerr << e.what() << std::endl;
	} catch (Rules::RedefinedArgException& e) {
		std::cerr << e.what() << std::endl;
	}
}

// WebServ::WebServ(const Webserv& rhs) {} // private ?

// WebServ&	WebServ::operator=(const WebServ& rhs) {} // private ?

WebServ::~WebServ() {
	// in ~VServ()
/*
	int	size = _serverFds.size();
	for (int i = 0; i < size; i++)
		if (_serverFds[i] != -1)
			close(_serverFds[i]);
*/
	if (_epollFd != -1)
		close(_epollFd);

	//for (std::set<int>::iterator it = _VServerFds.begin(); it != _VServerFds.end(); ++it)
	//	delete getVServ(*it);
/*
	size = _clientFds.size();

	for (int i = 0; i < size; i++)
		close(_clientFds[i]);
*/
}

// SETTERS


void	WebServ::setVServ(int fd, VServ* rhs) {
	_VServers[fd] = rhs;
}


// GETTERS

int	WebServ::getEpollFd() const {
	return (_epollFd);
}


//Renvoie un VServ* associe au FD passe. Si c'est un FD client, ca renvoie le *VServ "attache" a ce client. Si c'est un FD Server, renvoie le *VServ.
VServ*	WebServ::getVServ(int fd) {
	return (_VServers[fd]);
}


// METHODS

void	WebServ::handleSignal(int signal) {
	if (signal == SIGINT)
		throw (SIGINTException());
}

int	WebServ::epollWait(void) {
	int numEvents = epoll_wait(_epollFd, _epollEventsBuff.data(), _maxEvents, -1);
	if (numEvents == -1)
		throw (EpollWaitException());
	return (numEvents);
}

void	WebServ::epollCtlAdd(int fd, uint32_t events) {
	epoll_event event;
	event.events = events;
	event.data.fd = fd;
	
	if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, fd, &event) == -1)
		throw (EpollCtlAddException());
}

void	WebServ::epollCtlDel(int fd) {
	if (epoll_ctl(_epollFd, EPOLL_CTL_DEL, fd, NULL) == -1) //the EVENT can be null.
		throw (EpollCtlDelException());
}

bool WebServ::isServerFD(int fd) {
    std::map<int, fdType>::iterator it = _fds.find(fd);
    return (it != _fds.end() && it->second == SERVER_SOCK);
}

bool WebServ::isClientFD(int fd) {
    std::map<int, fdType>::iterator it = _fds.find(fd);
    return (it != _fds.end() && it->second == CLIENT_SOCK);
}

void	WebServ::deleteFd(int fd) {
	epollCtlDel(fd);
	close(fd);
}

void	WebServ::handleServerEvent(VServ* vserv) {
	int clientFd = vserv->clientAccept();
	_fds[clientFd] = CLIENT_SOCK;  
	setVServ(clientFd, vserv);
	fcntl(clientFd, F_SETFL, O_NONBLOCK);
	epollCtlAdd(clientFd, EPOLLIN | EPOLLET);

	if (_debug)
		std::cout << "New client connection. FD: " << clientFd << std::endl; 
}

void	WebServ::handleClientEvent(int clientFd, VServ* vserv) {

	if (_debug)
		std::cout << "Client request receieved. FD socket client: " << clientFd << std::endl;
	
	std::string	rawRequest;
	vserv->readSocketFD(clientFd, rawRequest);

	if (!rawRequest.empty()) {
		std::cout << "Request finish" << std::endl;
		if (_debug)
			std::cout << "REQUEST ------" << std::endl << rawRequest << std::endl;
		vserv->processRequest(rawRequest, clientFd);
	}
}


void	WebServ::listenEvents(void) {
	while (true) {
		try {
			int nbEvents = epollWait();
			for (int i = 0; i < nbEvents; i++) {
				int fd = _epollEventsBuff[i].data.fd;

				std::cout << "FD: " << fd << std::endl;

				VServ *vserv = getVServ(fd);
				if (!vserv)
					throw UnknownFdException();

				if (isServerFD(fd))
					handleServerEvent(vserv);
				if (isClientFD(fd))
					handleClientEvent(fd, vserv);
				
			}
		} catch (UnknownFdException& e) {
			//deleteFd(fd, _serverFds);
			std::cerr << "Fatal error: Unknown FD problem.";
			break;
		} catch (VServ::AcceptException& e) {
			std::cout << "AcceptException" << std::endl;
			//break; ? On stop le VServ ?
			//deleteFd(fd, _serverFds);
		} catch (VServ::RecvException& e) {
			std::cout << "RecvException" << std::endl;
			//break; ? On stop le VServ ?
			//deleteFd(fd, _serverFds);
		} catch (VServ::SendPartiallyException& e) {
			std::cout << "SendPartiallyException" << std::endl;
			// Je sais pas trop... c'est dur
		} catch (VServ::SendException& e) {
			std::cout << "SendException" << std::endl;
			// Je sais toujours pas a ce stade... demande trop de mana.
		}
	}
}
