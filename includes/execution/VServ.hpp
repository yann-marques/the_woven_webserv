/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VServ.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: locharve <locharve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 14:09:33 by locharve          #+#    #+#             */
/*   Updated: 2025/03/31 14:09:34 by locharve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
# include <cstring>
# include <set>
# include <arpa/inet.h>
# include <math.h>
# include <cstdio>
# include <fstream>


# include "Rules.hpp"
# include "HTTPRequest.hpp"
# include "WebServ.hpp"

class WebServ;

typedef std::vector<unsigned char> t_binary;
std::ofstream&	operator<<(std::ofstream& ofs, const t_binary& rhs);

class	VServ {
	private:
		const int											_maxClients; // defined in config file ?

		std::string											_host;
		int													_port;

		WebServ*											_mainInstance;
		std::map< std::string, Rules* >						_rules;

		std::set<std::string>								_envp;
		std::set<std::string>								_argv;

		int													_fd;
		sockaddr_in											_address;
		bool												_debug;

		std::map<int, size_t>								_cgiBytesWriting;
		std::map<int, size_t>								_totalBytesSent;

		std::map<int, t_binary>								_clientRequestBuffer;
		std::map<int, size_t>								_clientHeaderEndPos;
		std::map<int, t_binary>								_clientResponseBuffer;

		std::map<int, int>									_clientFdsPipeCGI;

		std::map<int, HttpRequest>							_clientRequests;
		std::map<int, HttpRequest>							_clientResponses;

		std::map<std::string, t_binary>						_cachedPages; // <path, content>


	public:
		VServ();
		VServ(WebServ* mainInstance, std::string host, int port, const std::map< std::string, Rules* >& sNamesMap,
			int maxClients, std::set< std::string > argv, std::set< std::string > envp);
		
		VServ(const VServ& rhs);
		VServ&	operator=(const VServ& rhs);
		~VServ();

		// SETTERS
		void	setRulesKeys(std::pair< std::multimap< const int, std::string >::const_iterator, std::multimap< const int, std::string >::const_iterator >& range);
		void	setAddress();

		int	getFd() const;

		// METHODS
		void						socketInit();
		int							clientAccept(void);
		void						eraseClient(int fd);
		bool 						readSocketFD(int fd);
		std::vector<unsigned char>	readFile(std::string rootPath);
		void						readRequest(HttpRequest &request);
		bool						sendRequest(HttpRequest &request, int clientFd);
		void						processRequest(int &clientFd);
		void						processResponse(int &clientFd);
		void						readDefaultPages(HttpRequest &request);
		void						showDirectory(HttpRequest &request);
		void						handleBigRequest(HttpRequest &request);
		std::string 				makeRootPath(HttpRequest &request);
		bool						isCGI(HttpRequest &request);
		void						talkToCgi(epoll_event event);
		void						executeCGI(HttpRequest &request);
		std::vector<char*>			makeEnvp(HttpRequest &request);
		void						setTargetRules(HttpRequest &req);
		void 						checkAllowedMethod(HttpRequest& request);
		bool						isEndedChunckReq(t_binary &clientBuffer);
		bool						isHttpRequestComplete(t_binary &clientBuffer, int clientFd);
		void						uploadFile(HttpRequest request, t_binary content);
		bool						makeHttpRedirect(HttpRequest &request, HttpRequest &response);

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
		class	ChildProcessException: public std::exception {
			public:
				const char* what() const throw();
		};	
		class	ServerNameNotFound: public std::exception {
			public:
				const char* what() const throw();
		};
		class	InterpreterEmpty: public std::exception {
			public:
				const char* what() const throw();
		};
		class	MethodNotAllowed: public std::exception {
			public:
				const char* what() const throw();
		};
		class	CreateFileException: public std::exception {
			public:
				const char* what() const throw();
		};
		class	NoUploadFileName: public std::exception {
			public:
				const char* what() const throw();
		};
		class	EpollWaitException: public std::exception {
			public:
				const char* what() const throw();
		};
		class	EpollCTLException: public std::exception {
			public:
				const char* what() const throw();
		};
		class	EpollCreateException: public std::exception {
			public:
				const char* what() const throw();
		};
		class	ServerNameNotFoundException: public std::exception {
			public:
				const char*	what() const throw();
		};
		class	BadRequestException: public std::exception {
			public:
				const char*	what() const throw();
		};
		class	SigPipe: public std::exception {
			public:
				const char*	what() const throw();
		};
		
};
