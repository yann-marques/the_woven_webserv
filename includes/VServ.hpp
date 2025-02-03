#pragma once

# include <exception>
# include <sys/socket.h>
# include <sys/epoll.h>
# include <netinet/in.h>
# include <unistd.h>
# include <fcntl.h>

# include "Config.hpp"
# include "HTTPRequest.hpp"

class	VServ {
	private:
		const int	_maxClients; // defined in config file ?
		// config
		int			_port;
		int			_host;
		// ...
		int			_fd;
		sockaddr_in	_address;

	public:
		VServ(): _maxClients(1024) {}
		VServ(VServConfig config, int maxClients);
		VServ(const VServ& rhs): _maxClients(rhs._maxClients) { *this = rhs; }
		VServ&	operator=(const VServ& rhs);
		~VServ();

		// SETTERS
		void	setAddress();

		// GETTERS
		int	getPort() const;
		int	getHost() const;
		int	getFd() const;

		// METHODS
		void	socketInit();
		int		clientAccept(void);
		std::string readRequest(const int fd);
		void	processRequest(std::string rawRequest, int clientFd);

		// EXCEPTIONS
		class	SocketException: public std::exception {
			public:
				const char*	what() const throw();
		};
		class	SetSockOptException: public std::exception {
			public:
				const char*	what() const throw();
		};
		class	BindException: public std::exception {
			public:
				const char*	what() const throw();
		};
		class	ListenException: public std::exception {
			public:
				const char*	what() const throw();
		};
		class	AcceptException: public std::exception {
			public:
				const char*	what() const throw();
		};
		class	RecvException: public std::exception {
			public:
				const char*	what() const throw();
		};
		class	SendException: public std::exception {
			public:
				const char*	what() const throw();
		};
		class	SendPartiallyException: public std::exception {
			public:
				const char*	what() const throw();
		};
};

std::ostream&	operator<<(std::ostream& os, const VServ& vs);