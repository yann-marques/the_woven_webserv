#ifndef VSERV_HPP
# define VSERV_HPP

# include <exception>
# include "Config.hpp"

class	VServ {
	private:
		const int			_maxClients; // defined in config file ?
		// config
		int					_port;
		int					_host;
		// ...
		int					_fd;
		struct sockaddr_in	_address;
		struct epoll_event	_event;

	public:
		VServ();
		VServ(VServConfig config, int maxClients);
		VServ(const VServ& rhs);
		VServ&	operator=(const VServ& rhs);
		~VServ();

		// SETTERS
		void	setAddress();
		void	setEvent();

		// GETTERS
		int	getFd() const;

		//
		void	socketInit();
		void	epollCtl(int epollFd);

		// EXCEPTIONS
		class	SocketException(): public std::exception {
			public:
				const char*	what() const throw();
		};
		class	SetSockOptException(): public std::exception {
			public:
				const char*	what() const throw();
		};
		class	BindException(): public std::exception {
			public:
				const char*	what() const throw();
		};
		class	ListenException(): public std::exception {
			public:
				const char*	what() const throw();
		};
		class	EpollCtlException(): public std::exception {
			public:
				const char*	what() const throw();
		};
};

#endif