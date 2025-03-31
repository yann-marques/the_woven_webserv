/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: locharve <locharve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 14:09:42 by locharve          #+#    #+#             */
/*   Updated: 2025/03/31 14:09:43 by locharve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

# include "templates.tpp"
# include "Rules.hpp"
# include "VServ.hpp"

class VServ;

enum fdType {
    SERVER_SOCK,
    CLIENT_SOCK,
	CGI_FD
};

class	WebServ {
	private:
		bool							_debug;
		int const						_maxClients; 
		int const						_maxEvents;
		Config							_config;

		int								_epollFd;
		std::vector<struct epoll_event> _epollEventsBuff;

		std::map<int, fdType>			_fds;
		std::map<int, VServ*>			_VServers;
		
		std::set<std::string>			_envp;
		std::set<std::string>			_argv;

	public:
		WebServ();
		WebServ(std::string filename, char **argv, char **envp);
		WebServ(const WebServ& rhs);
		WebServ&	operator=(const WebServ& rhs);
		~WebServ();

		//SETTERS
		void	setVServ(int fd, VServ* rhs);
		void	setVServMap(const std::map< std::string, std::map< int, std::map< std::string, Rules* > > >& config);
		void	setFdType(int fd, fdType type);

		//GETTERS
		VServ*	getVServ(int fd);
		int	getEpollFd() const;

		//METHODS
		void	handleServerEvent(VServ* vserv);
		void	handleClientEvent(int fd, VServ* vserv);
		void	listenEvents(void);
		int		epollWait(void);
		void	epollCtlAdd(int fd, uint32_t events);
		void	epollCtlMod(int fd, uint32_t events);
		void	epollCtlDel(int fd);
		void	deleteFd(int fd);
		bool	isServerFD(int fd);
		bool	isClientFD(int fd);
		bool	isCGIFd(int fd);

		void	destruct();

		// EXCEPTIONS
		class	SignalException: public StrException {
			public:
				SignalException(int signal);
				~SignalException() throw();

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
