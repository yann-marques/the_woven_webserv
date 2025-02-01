#ifndef VSERV_HPP
# define VSERV_HPP

# include "Config.hpp"

class	VServ {
	private:
		int	_fd;
		int	_port;
		int	_host;
		// ...

	public:
		VServ();
		VServ(/* server config */);
		VServ(const VServ& rhs);
		VServ&	operator=(const VServ& rhs);
		~VServ();
};

#endif