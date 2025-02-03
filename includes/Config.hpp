#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <iostream>
# include <vector>

//peut etre separer VServConfig de Config dans les fichier .hpp ?

class	VServConfig { // ?
	private:
		int _port;
		int _host; // int ? //string -> "0.0.0.0" ou "127.0.0.1". sera specifie dans la config par le user.
	//	std::string	_name;
 	// error pages + locations
	//	std::string	root;
	// std::string	index;
	public:
		VServConfig(int i) {
			_port = 8080 + i;
			_host = i + 10; //  i + 10 n'est pas une ip valide
		}

		int	getPort() const { return (_port); }
		int	getHost() const { return (_host); }
};

class	Config {
	private:
		std::vector<VServConfig>	_serverConfig;
		std::size_t	_serversNbr;

		std::size_t	_maxClientBody;
	public:
		Config();
		Config(std::string filename);
		Config(const Config& rhs);
		Config&	operator=(const Config& rhs);
		~Config() {}

		std::size_t		getServersNbr() const;
		VServConfig&	getServerConfig(int i);
};

#endif