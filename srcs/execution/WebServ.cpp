#include "WebServ.hpp"

// WebServ::WebServ() {} // private ?

void	WebServ::setVServMap(const std::map< std::string, std::map< int, std::map< std::string, Rules* > > >& config) {
	t_map_it< std::string, std::map< int, std::map< std::string, Rules* > > >::t
		hostIt = config.begin(), hostIte = config.end();

	while (hostIt != hostIte) {
		std::string	host = hostIt->first;
		t_map_it< int, std::map< std::string, Rules* > >::t
			portIt = hostIt->second.begin(), portIte = hostIt->second.end();

		while (portIt != portIte) {
			int port = portIt->first;
			std::map< std::string, Rules* >	sNamesMap = portIt->second;
			VServ*	vserv = new VServ(this, host, port, sNamesMap, _maxClients, _argv, _envp);
			
			int	sfd = vserv->getFd();
			setFdType(sfd, SERVER_SOCK);
			setVServ(sfd, vserv);

			epollCtlAdd(sfd, EPOLLIN | EPOLLOUT);
			portIt++;
		}
		hostIt++;
	}
}

WebServ::WebServ(std::string fileName, char **argv, char **envp): _maxClients(1000), _maxEvents(1000) { // , _config(filename.c_str()) {
//	signal(SIGINT, handleSignal); // in execution
		// epoll init
		_epollFd = epoll_create(_maxClients + 1);
		if (_epollFd == -1)
			throw EpollCreateException();
	
		_config = Config(fileName.c_str());
		// set argv
		for (size_t i = 0, n = sizeof(argv); i < n; i++) {
			if (argv[i])
				_argv.insert(argv[i]);
		}
		// set envp
		for (size_t i = 0, n = sizeof(envp); i < n; i++) {
			if (envp[i])
				_envp.insert(envp[i]);
		}

		this->_debug = false;
		if (_argv.find("--debug=yes") != _argv.end())
			this->_debug = true;

		setVServMap(_config.getParsedConfig());

		_epollEventsBuff.resize(_maxEvents);
	//	listenEvents();
}

WebServ::~WebServ() {
	if (_epollFd != -1)
		close(_epollFd);
	t_map_it< int, VServ* >::t	it = _VServers.begin(), ite = _VServers.end();
	while (it != ite) {
		if (isServerFD(it->first))
			delete it->second;
		close(it->first);
		it++;
	}
}

// SETTERS

void	WebServ::setVServ(int fd, VServ* rhs) {
	_VServers[fd] = rhs;
}

void	WebServ::setFdType(int fd, fdType type) {
	_fds[fd] = type;
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
	
	if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, fd, &event) == -1) {
		std::cout << "ERROR: " << strerror(errno) << std::endl;
		throw (EpollCtlAddException());
	}
}

void	WebServ::epollCtlMod(int fd, uint32_t events) {
	epoll_event event;
	event.events = events;
	event.data.fd = fd;
	
	if (epoll_ctl(_epollFd, EPOLL_CTL_MOD, fd, &event) == -1) {
		std::cout << "ERROR: " << strerror(errno) << std::endl;
		throw (EpollCtlAddException());
	}
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

bool WebServ::isCGIFd(int fd) {
    std::map<int, fdType>::iterator it = _fds.find(fd);
    return (it != _fds.end() && it->second == CGI_FD);
}

void	WebServ::deleteFd(int fd) {
	epollCtlDel(fd);
	close(fd);
}

void	WebServ::handleServerEvent(VServ* vserv) {
	int clientFd = vserv->clientAccept();
	setFdType(clientFd, CLIENT_SOCK); 
	setVServ(clientFd, vserv);
	fcntl(clientFd, F_SETFL, O_NONBLOCK);
	epollCtlAdd(clientFd, EPOLLIN | EPOLLOUT | EPOLLET);

	if (_debug)
		std::cout << "New client connection. FD: " << clientFd << std::endl; 
}

void	WebServ::listenEvents(void) {
	while (true) {
		try {
			int nbEvents = epollWait();
			for (int i = 0; i < nbEvents; i++) {
				epoll_event event = _epollEventsBuff[i];
				int fd = event.data.fd;

				VServ *vserv = getVServ(fd);
				if (!vserv)
					throw UnknownFdException();

				if (isServerFD(fd))
					handleServerEvent(vserv);
				else if (isClientFD(fd) && (event.events & EPOLLIN))
					vserv->processRequest(fd);
				else if (isClientFD(fd) && (event.events & EPOLLOUT))
					vserv->processResponse(fd);
				else if (isCGIFd(fd))
					vserv->talkToCgi(event);
				
			}
		} catch (UnknownFdException& e) {
			std::cerr << "Fatal error: Unknown FD problem.";
			break;
		} catch (VServ::AcceptException& e) {
			std::cout << "AcceptException" << std::endl;
		} catch (VServ::RecvException& e) {
			std::cout << "RecvException" << std::endl;
		} catch (VServ::SendPartiallyException& e) {
			std::cout << "SendPartiallyException" << std::endl;
		} catch (VServ::SendException& e) {
			std::cout << "SendException" << std::endl;
		}
	}
}
