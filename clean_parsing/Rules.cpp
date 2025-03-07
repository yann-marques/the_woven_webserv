#include "Rules.hpp"

Rules::Rules() {
//	_locationPath = "";

	_autoIndex = true;
	_maxBodyBytes = 1024;
	_root = "www/default.html";
	_redirect = "redirect";
	_upload = "upload";

	_defaultPages.push_back("index");

	_allowedMethods.push_back("GET");
	_allowedMethods.push_back("POST");
	_allowedMethods.push_back("DELETE");

	_errorKeys.insert(404);
	_errorPages[404] = "404.html";
/*
	_cgiKeys.insert(".default");
	_cgiPath[".default"] = "path_to_default";
*/
}

Rules::Rules(const Rules& rhs) {
	*this = rhs;
}

Rules&	Rules::operator=(const Rules& rhs) {
	_args = rhs.getArgs();
	_locationPath = rhs.getLocationPath();
	_autoIndex = rhs.getAutoIndex();
	_maxBodyBytes = rhs.getMaxBodyBytes();
	_root = rhs.getRoot();
	_redirect = rhs.getRedirect();
	_upload = rhs.getUpload();
	_defaultPages = rhs.getDefaultPages();
	_allowedMethods = rhs.getAllowedMethods();
	_errorKeys = rhs.getErrorKeys();
	_errorPages = rhs.getErrorPages();
	_cgiKeys = rhs.getCgiKeys();
	_cgiPath = rhs.getCgiPath();

	_locationKeys = rhs.getLocationKeys();
	for (std::vector< std::string >::iterator it = _locationKeys.begin(), ite = _locationKeys.end(); it != ite; it++) {
		std::string	key = *it;
		_location[key] = new Rules(*(rhs.getLocation().at(key)));
	}

	return (*this);
}

Rules::Rules(std::multimap< std::string, std::string > args, const Rules& rhs, std::string locationPath): Config() {
	_args = args;
	setArgs(_args);
	_locationPath = locationPath; // '/'
	*this |= rhs;
	if (args.count("location"))
		setLocation(args.equal_range("location"));
}

void	Rules::setAutoIndex(std::string str) {
	_autoIndex = (str == "true" || str == "yes" || str == "1");
}

void	Rules::setArgs(std::multimap< std::string, std::string > args) {
	if (args.count("port"))
		throw (RedefinedArgException("port"));
	if (args.count("server_names"))
		throw (RedefinedArgException("server_names"));

	if (args.count("auto_index"))
		setAutoIndex(args.find("auto_index")->second);
	if (args.count("max_body_bytes"))
		_maxBodyBytes = parseMaxBodyBytes(args.find("max_body_bytes")->second);
	if (args.count("root"))
		_root = args.find("root")->second;
	if (args.count("redirect"))
		_redirect = args.find("redirect")->second;
	if (args.count("upload"))
		_upload = args.find("upload")->second;

	if (args.count("default_pages"))
		setVector(_defaultPages, args.equal_range("default_pages"));
	if (args.count("allowed_methods"))
		setVector(_allowedMethods, args.equal_range("allowed_methods"));

	if (args.count("error_pages"))
		setErrorPages(args.equal_range("error_pages"));
	if (args.count("cgi_path"))
		setCgiPath(args.equal_range("cgi_path"));
}

void	Rules::setErrorPages(t_range range) {
	t_multimap_it	mmIt = range.first, mmIte = range.second;
	do {
		std::string	str = mmIt->second;
		size_t	pos1 = str.find(':'), pos2 = str.find(';');
		int	key = std::atoi(str.substr(0, pos1).c_str());
		if (!_errorPages.count(key)) {
			_errorKeys.insert(key);
			_errorPages[key] = str.substr(pos1 + 1, pos2 - pos1 - 1);
	//		std::cout << "TEST: " << key << " : " << _errorPages[key] << std::endl; 
		}
		mmIt++;
	} while (mmIt != mmIte);
}

void	Rules::setCgiPath(t_range range) {
	t_multimap_it	mmIt = range.first, mmIte = range.second;
	do {
		std::string	str = mmIt->second;
		size_t	pos1 = str.find(':'), pos2 = str.find(';');
		std::string	key = str.substr(0, pos1);
		if (!_cgiPath.count(key)) {
			_cgiKeys.insert(key);
			_cgiPath[key] = str.substr(pos1 + 1, pos2 - pos1 - 1);
		}
		mmIt++;
	} while (mmIt != mmIte);
}

void	Rules::setLocation(t_range range) {
	t_multimap_it	mit = range.first, mite = range.second;
	while (mit != mite) {
		std::multimap< std::string, std::string >	args = parseLocationLine(mit->second);
		checkArgsFormat(args);

		if (!_locationKeys.size())
			return ;
		for (size_t i = 0, n = _locationKeys.size(); i < n; i++) {
			if (!_location.count(_locationKeys[i])) {
				std::string	newLocPath;
				if (_locationPath == "/")
					newLocPath = _locationKeys[i];
				else
					newLocPath = _locationPath + _locationKeys[i];
				Rules*	rules = new Rules(args, *this, newLocPath);
				_location[_locationKeys[i]] = rules;
			}
		}
		args.clear();
		mit++;
	}
}

static bool	isValidLocationKey(std::string key) {
	if (key[0] != '/')
		return (false);
	size_t	i = 1;
	while (isalnum(key[i]))
		i++;
	return (!key[i]);
}

void	Rules::setLocationKey(std::string str) {
	_locationKeys.push_back(str);
}

std::multimap< std::string, std::string >	Rules::parseLocationLine(std::string line) {
	std::multimap< std::string, std::string >	args;
	std::string	key;
	size_t	pos = line.find(',');
	while (pos != std::string::npos && pos < line.find('{')) {
		key = line.substr(0, pos);
		if (!isValidLocationKey(key))
			throw (InvalidLocationKeyException(key));
		setLocationKey(key);
		pos++;
		line.erase(0, pos);
		pos = line.rfind(',');
	}
	pos = line.find('{');
	if (pos != std::string::npos) {
		key = line.substr(0, pos);
		if (!isValidLocationKey(key))
			throw (InvalidLocationKeyException(key));
		setLocationKey(key);
		pos++;
		line.erase(0, pos);
		pos = line.rfind('}');
		if (pos != std::string::npos)
			line.erase(pos, 1);
	}
	args = parseLine(line);
	return (args);
}

void	Rules::setVector(std::vector< std::string >& vec, t_range range) {
	for (t_multimap_it	mit = range.first, mite = range.second; mit != mite; mit++) {
		std::string	str = mit->second;
		do {
			size_t	pos = str.find(',');
			if (pos == std::string::npos)
				pos = str.find(';');
			std::string	subStr = str.substr(0, pos);
			vec.push_back(subStr);
			str.erase(0, subStr.size() + 1);
		} while (!str.empty());
	}
}

size_t	Rules::parseMaxBodyBytes(std::string str) {
	size_t	pos = 0, result = 0;
	while (str[pos] && isdigit(str[pos]))
		pos++;
	result = std::atoi(str.c_str());
	if (str[pos]) {
		if (str[pos] == ';')
			return (result);
		else if (str[pos] == 'K')
			result *= 1024;
		else if (str[pos] == 'M')
			result *= 1048576;
		else if (str[pos] == 'G')
			result *= 1073741824;
		pos++;
	}
	if (str[pos])
		result += std::atoi(str.substr(pos).c_str());
	return (result);
}

Rules::~Rules() {
	if (!_location.size())
		return ;

	for (size_t i = 0, n = _locationKeys.size(); i < n; i++)
		delete _location[_locationKeys[i]];

	_defaultPages.clear();
	_errorPages.clear();
	_allowedMethods.clear();
	_cgiPath.clear();
	_locationKeys.clear();
	_location.clear();
}

template< typename T >
static void	printVec(const std::vector< T >& vec, std::string tabs) {
	for (size_t i = 0, n = vec.size(); i < n; i++)
		std::cout << tabs << '\t' << vec[i] << std::endl;
}

template< typename T, typename U >
static void	printMap(std::set< T > keys, const std::map< T, U >& map, std::string tabs) {
	typename std::set< T >::iterator	keyIt = keys.begin(), keyIte = keys.end();
	while (keyIt != keyIte) {
		std::cout << tabs << '\t' << *keyIt << ": " << map.at(*keyIt) << std::endl;
		keyIt++;
	}
}

void	Rules::printDeep(size_t i, std::string name) {
	std::string	slashes(i * 8, '/');
	std::string	tabs(i, '\t');
	std::cout	<< slashes << " PRINTDEEP " << i << " : " << name << " " << this << std::endl
				<< tabs << "locationPath:\t" << getLocationPath() << std::endl
				<< tabs << "root:\t" << getRoot() << std::endl
				<< tabs << "autoIndex:\t" << getAutoIndex() << std::endl
				<< tabs << "maxBodyBytes:\t" << getMaxBodyBytes() << std::endl
				<< tabs << "redirect:\t" << getRedirect() << std::endl
				<< tabs << "upload:\t" << getUpload() << std::endl
				<< tabs << "defaultPages:" << std::endl;
	printVec(getDefaultPages(), tabs);
	std::cout << tabs << "allowedMethods:" << std::endl;
	printVec(getAllowedMethods(), tabs);
		
	std::cout << tabs << "cgiPaths:" << std::endl;
	printMap(getCgiKeys(), getCgiPath(), tabs);
	std::cout << tabs << "errorPages:" << std::endl;
	printMap(getErrorKeys(), getErrorPages(), tabs);
	std::cout << std::endl;

	for (size_t pos = 0, n = _locationKeys.size(); pos < n; pos++) {
		std::cout	<< tabs << name << " contains " << _locationKeys[pos]
					<< " " << _location[_locationKeys[pos]] << std::endl;
		_location[_locationKeys[pos]]->printDeep(i + 1, _locationKeys[pos]);
	}
}

Rules&	Rules::operator|=(const Rules& rhs) {
	if (!_args.count("root"))
		_root = rhs.getRoot();
	if (!_args.count("default_pages"))
		_defaultPages = rhs.getDefaultPages();
	if (!_args.count("auto_index"))
		_autoIndex = rhs.getAutoIndex();
	if (!_args.count("allowed_methods"))
		_allowedMethods = rhs.getAllowedMethods();
	if (!_args.count("max_body_bytes"))
		_maxBodyBytes = rhs.getMaxBodyBytes();
	if (!_args.count("redirect"))
		_redirect = rhs.getRedirect();
	if (!_args.count("upload"))
		_upload = rhs.getUpload();

	std::set< int >::iterator	setItA = rhs.getErrorKeys().begin(), setIteA = rhs.getErrorKeys().end();
	while (setItA != setIteA) {
		if (!_errorKeys.count(*setItA)) {
			_errorKeys.insert(*setItA);
			_errorPages[*setItA] = rhs.getErrorPages().at(*setItA);
		}
		setItA++;
	}
	std::set< std::string >::iterator	setItB = rhs.getCgiKeys().begin(), setIteB = rhs.getCgiKeys().end();
	while (setItB != setIteB) {
		if (!_cgiKeys.count(*setItB)) {
			_cgiKeys.insert(*setItB);
			_cgiPath[*setItB] = rhs.getCgiPath().at(*setItB);
		}
		setItB++;
	}

	return (*this);
}

// EXCEPTIONS

Rules::RedefinedArgException::RedefinedArgException(std::string where): StrException("Argument redefined in location: " + where) {}

Rules::RedefinedArgException::~RedefinedArgException() throw() {}

const char*	Rules::RedefinedArgException::what() const throw() {
	return (_str.c_str());
}

Rules::InvalidLocationKeyException::InvalidLocationKeyException(std::string where): StrException("Invalid location key: " + where + " (must be formated as: \"/<alnum>\")") {}

Rules::InvalidLocationKeyException::~InvalidLocationKeyException() throw() {}

const char*	Rules::InvalidLocationKeyException::what() const throw() {
	return (_str.c_str());
}