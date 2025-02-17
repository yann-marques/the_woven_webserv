#include "Rules.hpp"

Rules::Rules() {
//	_root = "www/default.html";
	_autoIndex = true;
	_maxBodyBytes = 1024;
}

Rules::Rules(std::multimap< std::string, std::string > args): Config() { // heritage foireux
	_rawArgs = args;
	setArgs(args);
	// location
//	t_range	range = args.equal_range("location");
	if (args.count("location"))
		setLocation(args.equal_range("location"));
}

void	Rules::setAutoIndex(std::string str) {
	_autoIndex = (str == "true" || str == "yes" || str == "1");
}

void	Rules::setArgs(std::multimap< std::string, std::string > args) {
	// set default values or previous (parent) values
	_root = args.find("root")->second;
	setVector(_defaultPages, args.equal_range("default_pages"), "default_pages");
	// errorPages ?
	setErrorPages(args.equal_range("error_pages"));
	setAutoIndex(args.find("auto_index")->second);
	setVector(_allowedMethods, args.equal_range("allowed_methods"), "allowed_methods");
	_maxBodyBytes = parseMaxBodyBytes(args.find("max_body_bytes")->second);
	setVector(_cgiPaths, args.equal_range("cgi_paths"), "cgi_paths");
	_redirect = args.find("redirect")->second;
	_upload = args.find("upload")->second; // set default ?
}

void	Rules::inheritArgs(std::multimap< std::string, std::string >& args) {
	std::set< std::string >::iterator	it = _argsToFind.begin(), ite = _argsToFind.end();
	while (it != ite) {
		if (!args.count(*it) || args.equal_range(*it).first->second == _defaultValues[*it]) {
			t_range range = _rawArgs.equal_range(*it);
			t_multimap_it	mit = range.first, mite = range.second;
			while (mit != mite) {
				args.insert(make_pair(*it, mit->second));
				mit++;
			}
		}
		it++;
	}
}

void	Rules::setErrorPages(t_range range) {
	t_multimap_it	mit = range.first, mite = range.second;
	std::string	str = mit->second, defaultValue(_defaultValues["error_pages"]);
	while (mit != mite && str != defaultValue) {
		size_t	pos1 = str.find(':'), pos2 = str.find(';');
		int	key = std::atoi(str.substr(0, pos1).c_str());
		if (!_errorPages.count(key)) {
			_errorKeys.insert(key);
			_errorPages[key] = str.substr(pos1 + 1, pos2 - pos1 - 1);
		}
		mit++;
		str = mit->second;
	}
}

void	Rules::setLocation(t_range range) {
	t_multimap_it	mit = range.first, mite = range.second;
	while (mit != mite && mit->second != _defaultValues["location"]) {
		std::multimap< std::string, std::string >	args = parseLocationLine(mit->second);
		checkArgsFormat(args);
		inheritArgs(args); // not good
	//	printMultimap(_argsToFind, args);
	//	std::cout << args.equal_range("root").first->second << std::endl;

		if (!_locationKeys.size())
			return ;
		for (size_t i = 0, n = _locationKeys.size(); i < n; i++) {
			if (!_location.count(_locationKeys[i])) {
				Rules*	rules = new Rules(args);
//				std::cout << this << " creates : " << rules << std::endl;
//				std::cout << "\tkey: " << _locationKeys[i] << "\tRules addr: " << rules << std::endl;
				_location[_locationKeys[i]] = rules;
			}
		}
		args.clear();
		mit++;
	}
}

void	Rules::setLocationKey(std::string str) {
//	std::cout << "str: " << str << std::endl;
	_locationKeys.push_back(str);
}

std::multimap< std::string, std::string >	Rules::parseLocationLine(std::string line) {
//	std::cout << "////////////////// parseLocationLine" << std::endl;
//	std::cout << "line before: " << line << std::endl;
	std::multimap< std::string, std::string >	args;
	size_t	pos = line.find(',');
	while (pos != std::string::npos && pos < line.find('{')) {
		setLocationKey(line.substr(0, pos));
		pos++;
		line.erase(0, pos);
		pos = line.rfind(',');
	}
	pos = line.find('{');
	if (pos != std::string::npos) {
		setLocationKey(line.substr(0, pos));
		pos++;
		line.erase(0, pos);
		pos = line.rfind('}');
		if (pos != std::string::npos)
			line.erase(pos, 1);
	}
//	std::cout << "line after: " << line << std::endl;
//	std::cout << "//////// location keys in " << this << std::endl;
//	for (size_t i = 0, n = _locationKeys.size(); i < n; i++)
//		std::cout << _locationKeys[i] << std::endl;
	args = parseLine(line);
	return (args);
}

void	Rules::setVector(std::vector< std::string >& vec, t_range range, std::string key) {
	for (t_multimap_it	mit = range.first, mite = range.second; mit != mite; mit++) {
		// parse , ;
		std::string	str = mit->second;
		do {
//			std::cout << "str: " << str << std::endl;
			size_t	pos = str.find(',');
			if (pos == std::string::npos)
				pos = str.find(';');
			std::string	subStr = str.substr(0, pos);
//			std::cout << "key: " << key << "\tsubstr: " << subStr << "\tdefault: " << _defaultValues[key] << std::endl;
			vec.push_back(subStr);
			str.erase(0, subStr.size() + 1); //
		} while (str != _defaultValues[key] && !str.empty());
	}
}

size_t	Rules::parseMaxBodyBytes(std::string str) {
//	std::cout << "parseMBB: " << str << std::endl;
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
//		pos = std::string::npos;
//	std::cout << "substr: " << str.substr(pos) << std::endl; 
		result += std::atoi(str.substr(pos).c_str());
	// limit ?
//	std::cout << "mbb: str = " << str.substr(pos) << "\tatoi: " << std::atoi(str.substr(pos).c_str()) << std::endl;
	return (result);
}

void	Rules::goDeep(size_t i) {
	(void) i;
	for (size_t pos = 0, n = _locationKeys.size(); pos < n; pos++) {
		for (size_t j = 0; j < i; j++)
			std::cout << '\t';
		std::cout << "GODEEP " << i << " : " << this << " " << _locationKeys[pos] << " contains " << _location[_locationKeys[pos]] << std::endl;
		_location[_locationKeys[pos]]->goDeep(i + 1);
	}
}

Rules::~Rules() {
//	std::cout << "RULES DESTRUCTOR CALLED" << std::endl;
//	std::cout << "sizeof(Rules): " << sizeof(*this) << std::endl;
	if (!_location.size())
		return ;
	for (size_t i = 0, n = _locationKeys.size(); i < n; i++) {
//		std::cout << this << " destructs : " << _location[_locationKeys[i]] << std::endl;
//		destruct(_location[_locationKeys[i]]);
		delete _location[_locationKeys[i]];
	}
	_defaultPages.clear();
	_errorPages.clear();
	_allowedMethods.clear();
	_cgiPaths.clear();
	_locationKeys.clear();
	_location.clear();
}

template< typename T >
static void	printVec(const std::vector< T >& vec) {
	for (size_t i = 0, n = vec.size(); i < n; i++)
		std::cout << "\t" << vec[i] << std::endl;
}

template< typename T, typename U >
static void	printMap(std::set< T > keys, const std::map< T, U >& map) {
	typename std::set< T >::iterator	keyIt = keys.begin(), keyIte = keys.end();
	while (keyIt != keyIte) {
		std::cout << "\t" << *keyIt << ":\t" << map.at(*keyIt) << std::endl;
		keyIt++;
	}
}

static void	printLocation(std::vector< std::string > keys, std::map< std::string, Rules* > loc) {
	std::vector< std::string >::iterator	keyIt = keys.begin(), keyIte = keys.end();
	while (keyIt != keyIte) {
		std::cout << "////////// " << *keyIt << std::endl << *(loc.at(*keyIt)) << std::endl;
		keyIt++;
	}
}

	// to do
std::ostream&	operator<<(std::ostream& os, const Rules& rhs) {

	std::cout	<< "root:\t" << rhs.getRoot() << std::endl
				<< "autoIndex:\t" << rhs.getAutoIndex() << std::endl
				<< "maxBodyBytes:\t" << rhs.getMaxBodyBytes() << std::endl
				<< "redirect:\t" << rhs.getRedirect() << std::endl
				<< "upload:\t" << rhs.getUpload() << std::endl
				<< "defaultPages:" << std::endl;
	printVec(rhs.getDefaultPages());
	std::cout	<< "allowedMethods:" << std::endl;
	printVec(rhs.getAllowedMethods());
	std::cout	<< "cgiPaths:" << std::endl;
	printVec(rhs.getCgiPaths());
	std::cout	<< "errorPages:" << std::endl;
	printMap(rhs.getErrorKeys(), rhs.getErrorPages());
	printLocation(rhs.getLocationKeys(), rhs.getLocation());
//	count = 0;
	return (os);
}
