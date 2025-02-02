#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <exception>
# include <vector>
# include <map>
# include "Config.hpp"
# include "VServ.hpp"

class	WebServ {
	private:
		static int const		_maxClients = 1000; // defined in config file ?
		static int const		_maxEvents = 1000; // defined in config file ?
		const Config			_config;	// parsed config file

		int						_epollFd;

		std::vector<int>		_serverFds;
		std::map<int, VServ>	_servers;

		std::vector<int>		_clientFds;

	public:
		WebServ();
		WebServ(std::string filename);
		WebServ(const Webserv& rhs);
		WebServ&	operator=(const WebServ& rhs);
		~WebServ();

		void	setServerFd(int i, int fd);
		void	setServer(int fd, const VServ& rhs);
		void	setClientFd(int i, int fd);

		int	getServerFd(int i) const;
		VServ&	getServer(int fd) const;
		int	getClientFd(int i) const;

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

#endif