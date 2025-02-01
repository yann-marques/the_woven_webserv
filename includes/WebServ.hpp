#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <vector>
# include <map>
# include "Config.hpp"
# include "VServ.hpp"

class	WebServ {
	private:
		Config	_config;	// parsed config file

		vector<int>	_serverFds;
		map<int, VServ>	_servers; // indexes = server fds // VServ pointers ?

		vector<int>	_clientFds;

	public:
		WebServ();
		WebServ(std::string filename);
		WebServ(const Webserv& rhs);
		WebServ&	operator=(const WebServ& rhs);
		~WebServ();

		void	setServerFd(int i, int fd);
		void	setServer(int fd, const VServ& rhs);
		void	setClientFd(int i, int fd);

		int	getServerFd(int i);
		VServ	getServer(int fd);
		int	getClientFd(int i);

		// exceptions:
};

#endif