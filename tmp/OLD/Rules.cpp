#include "Rules.hpp"

void	Rules::setVector(std::vector< std::string >& vec, t_range range) {
	for (t_multimap_it	mit = range.first, mite = range.second; mit != mite; mit++) {
		// parse , ;
		std::string	str = mit->second;
		do {
			size_t	pos = str.find(',');
			if (pos == std::string::npos)
				pos = str.find(';');
			std::string	subStr = str.substr(0, pos);
			vec.push_back(subStr);
			str.erase(0, pos + 1);
		} while (!str.empty());
	}
}

static size_t	parseMaxBodyBytes(std::string str) {
	size_t	pos = 0, result = 0;
	while (str[pos] && isdigit(str[pos]))
		pos++;
	result = std::atoi(str.c_str());
	if (str[pos] == ';')
		return (result);
	else if (str[pos] == 'K')
		result *= 1024;
	else if (str[pos] == 'M')
		result *= 1048576;
	else if (str[pos] == 'G')
		result *= 1073741824;
	result += std::atoi(str.substr(pos + 1).c_str());
	// limit ?
//	std::cout << "mbb: str = " << str.substr(pos) << "\tatoi: " << std::atoi(str.substr(pos).c_str()) << std::endl;
	return (result);
}

void	Rules::setLocation(std::map< std::string, std::multimap< std::string, std::string > > location) {
	(void) location;
}

Rules::Rules(std::multimap< std::string, std::string > args, std::map< std::string, std::multimap< std::string, std::string > > location) {
	_root = args.find("root")->second;
//	std::cout << "root: " << _root << std::endl;
	setVector(_defaultPages, args.equal_range("default_pages"));
	// set errorPages
//	std::string	str = args.equal_range("auto_index").first->second;
//	std::cout << "str = " << str << std::endl;
//	_autoIndex = parseAutoIndex(args.find("auto_index"));
	setVector(_allowedMethods, args.equal_range("allowed_methods"));
	_maxBodyBytes = parseMaxBodyBytes(args.find("max_body_bytes")->second);
//	std::cout << "MBB = " << _maxBodyBytes << std::endl;
	setVector(_cgiPaths, args.equal_range("cgi_paths"));
	if (args.count("redirect"))
		_redirect = args.find("redirect")->second; // set default ?
	if (args.count("upload"))
		_upload = args.find("upload")->second; // set default ?

	// set location // TERRIFYING
	if (location.size())
		setLocation(location);
}

Rules::~Rules() {}