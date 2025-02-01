#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <vector>

class	VServConfig; // ?

class	Config {
	private:
/*
		vector<VServConfig>	serverConfig;
*/
	public:
		Config();
		Config(std::string	filename);
		Config(const Config& rhs);
		Config&	operator=(const Config& rhs);
		~Config();
};

#endif