#pragma once

# include <exception>
# include <algorithm>
# include <sys/socket.h>
# include <sys/epoll.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <netinet/in.h>
# include <unistd.h>
# include <fcntl.h>
# include <dirent.h>
# include <sstream>
# include <iostream>
# include <sys/wait.h>
# include <errno.h>
# include <cstring>
# include <set>
# include <arpa/inet.h>
# include <math.h>


# include "parsing/Config.hpp"
# include "HTTPRequest.hpp"


class	VServ {
	private:
		const int					_maxClients; // defined in config file ?
		// config
		int							_port;
	//	int							_host;

	//	std::string					_root;

	//	std::vector<std::string>	_defaultPages;

		std::map< std::string, Rules* >	_rules;

		std::set<std::string>		_envp;
		std::set<std::string>		_argv;
		// ...
		int							_fd;
		sockaddr_in					_address;
		bool						_debug;

	public:
//		VServ(): _maxClients(1024), _root("www"), _envp(), _argv() {}
		VServ(): _maxClients(1024) {}
		VServ(int port, const std::map< std::string, Rules* >& rules, int maxClients, std::set<std::string> argv, std::set<std::string> envp);
		VServ(const VServ& rhs): _maxClients(rhs._maxClients) { *this = rhs; }
		VServ&	operator=(const VServ& rhs);
		~VServ();

		// SETTERS
		void	setAddress();

		// GETTERS
//		int	getPort() const;
//		int	getHost() const;
		int	getFd() const;

		// METHODS
		void				socketInit();
		int					clientAccept(void);
		std::string 		readSocketFD(const int fd);
		std::string 		readFile(int fd);
		void				processRequest(std::string rawRequest, int clientFd);
		void 				sendRequest(HttpRequest &request, int clientFd);
		std::string			readRequest(HttpRequest &request);
		std::string			readDefaultPages(HttpRequest &request);
		void				showDirectory(DIR* dir, HttpRequest &response);
		void				handleBigRequest(HttpRequest &request);
		std::string 		makeRootPath(HttpRequest &request);
		bool				fileIsCGI(HttpRequest &request);
		std::string			handleCGI(std::string &fileData, HttpRequest &request);
		std::vector<char*>	makeEnvp(HttpRequest &request);
		std::string			getPagePath(HttpRequest &request);

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
		class	FileNotExist: public std::exception {
			public:
				const char*	what() const throw();
		};
		class	OpenFileException: public std::exception {
			public:
				const char*	what() const throw();
		};
		class	ReadFileException: public std::exception {
			public:
				const char*	what() const throw();
		};
		class	OpenFolderException: public std::exception {
			public:
				const char*	what() const throw();
		};
		class	EntityTooLarge: public std::exception {
			public:
				const char*	what() const throw();
		};
		class	ExtensionNotFound: public std::exception {
			public:
				const char*	what() const throw();
		};
		class	PipeException: public std::exception {
			public:
				const char*	what() const throw();
		};
		class	ForkException: public std::exception {
			public:
				const char*	what() const throw();
		};
		class	ExecveException: public std::exception {
			public:
				const char* what() const throw();
		};
};
