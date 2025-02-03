#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <sys/socket.h>
# include <sys/epoll.h>
# include <unistd.h>
# include <signal.h>
# include <iostream>
# include <exception>
# include <vector>
# include <map>
# include "Config.hpp"
# include "VServ.hpp"

class	WebServ {
	private:
		static int const		_maxClients = 1000; // defined in config file ?
		static int const		_maxEvents = 1000; // defined in config file ?
		Config			_config;	// parsed config file

		int						_epollFd;

		std::vector<int>		_serverFds;
		std::size_t				_serverNbr;
		std::map<int, VServ*>	_servers;

		std::vector<int>		_clientFds;

	public:
		WebServ();
		WebServ(std::string filename);
		WebServ(const WebServ& rhs);
		WebServ&	operator=(const WebServ& rhs);
		~WebServ();

		void	setServerFd(int fd);
		void	setServer(int fd, VServ* rhs);
		void	setClientFd(int i, int fd);

		int	getEpollFd() const;
		int	getServerFd(int i) const;
		VServ*	getServer(int fd);
		int	getClientFd(int i) const;
		std::size_t	getServerNbr() const;

		void	handleSignal(int signal);

		// EXCEPTIONS
		class	SIGINTException: public std::exception {
			public:
				const char*	what() const throw();
		};
		class	EpollCreateException: public std::exception {
			public:
				const char*	what() const throw();
		};
};

std::ostream&	operator<<(std::ostream& os, WebServ& ws);

#endif