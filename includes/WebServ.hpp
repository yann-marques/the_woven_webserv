#pragma once

# include <sys/socket.h>
# include <sys/epoll.h>
# include <unistd.h>
# include <signal.h>
# include <iostream>
# include <sstream>
# include <exception>
# include <vector>
# include <map>
# include <set>
# include <algorithm>
# include <cstring>
# include <vector>
# include <set>

# include "parsing/Rules.hpp"
# include "VServ.hpp"

class	WebServ {
	private:
		bool							_debug;
		int const						_maxClients; 
		int const						_maxEvents;
		Config							_config;	// parsed config file

		int								_epollFd;
		epoll_event						_event;
		std::vector<struct epoll_event> _epollEvents;

		std::set<int>					_serverFds;
		std::set<int>					_clientFds;

		std::size_t						_serverNbr;

		std::map<int, VServ*>			_serversFdToServer;
		std::map<int, VServ*>			_clientsFdToServer;
		
		std::set<std::string>			_envp;
		std::set<std::string>			_argv;

	public:
		WebServ();
		WebServ(std::string filename, char **argv, char **envp);
		WebServ(const WebServ& rhs);
		WebServ&	operator=(const WebServ& rhs);
		~WebServ();

		//SETTERS
		void	insertServerFd(int fd);
		void	insertClientFd(int fd);
		void	setServerToServerFd(int fd, VServ* rhs);
		void	setServerToClientFd(int fd, VServ* rhs);

		//GETTERS
		VServ*	getRelatedServer(int fd);
		int	getEpollFd() const;

		std::set<int> getServersFd(void) const;
		std::size_t	getServerNbr() const;

		//METHODS
		void	handleServerEvent(VServ* vserv);
		void	handleClientEvent(int fd, VServ* vserv);
		bool	fdIsServer(int fd);
		bool	fdIsClient(int fd);
		void	listenEvents(void);
		int		epollWait(void);
		void	epollCtlAdd(int fd);
		void	epollCtlDel(int fd);
		void	deleteFd(int fd, std::set<int>& sets);
		void	setEvent(uint32_t epoll_event, int fd);


		// EXCEPTIONS
		class	SIGINTException: public std::exception {
			public:
				const char*	what() const throw();
		};
		class	EpollCreateException: public std::exception {
			public:
				const char*	what() const throw();
		};
		class	EpollWaitException: public std::exception {
			public:
				const char*	what() const throw();
		};
		class	EpollCtlAddException: public std::exception {
	public:
				const char*	what() const throw();
		};
		class	EpollCtlDelException: public std::exception {
			public:
				const char*	what() const throw();
		};
		class	UnknownFdException: public std::exception {
			public:
				const char*	what() const throw();
		};
};
