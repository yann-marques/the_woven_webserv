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

class VServ;

class	WebServ {
	private:
		bool							_debug;
		int const						_maxClients; 
		int const						_maxEvents;
		Config							_config;	// parsed config file

		int								_epollFd;
		epoll_event						_event;
		std::vector<struct epoll_event> _epollEvents;

		std::size_t						_VServerNbr;
		std::map<int, VServ*>			_VServers;
		std::set<int>					_VServerFds;
		
		std::set<std::string>			_envp;
		std::set<std::string>			_argv;

	public:
		WebServ();
		WebServ(std::string filename, char **argv, char **envp);
		WebServ(const WebServ& rhs);
		WebServ&	operator=(const WebServ& rhs);
		~WebServ();

		//SETTERS
		void	insertVServFd(int fd);
		void	setVServ(int fd, VServ* rhs);

		//GETTERS
		VServ*	getVServ(int fd);
		int	getEpollFd() const;

		std::set<int> getServersFd(void) const;
		std::size_t	getServerNbr() const;

		//METHODS
		void	handleServerEvent(VServ* vserv);
		void	handleClientEvent(int fd, VServ* vserv);
		bool	isVServFD(int fd);
		void	handleSignal(int signal);
		void	listenEvents(void);
		int		epollWait(void);
		void	epollCtlAdd(int fd);
		void	epollCtlDel(int fd);
		void	setEvent(uint32_t epoll_event, int fd, void *ptr);
		void	deleteFd(int fd);
		

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
