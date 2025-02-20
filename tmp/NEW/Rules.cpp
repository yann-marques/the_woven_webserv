#include "Rules.hpp"

Rules::Rules() {
	_autoIndex = true;
	_maxBodyBytes = 1024;
	_root = "www/default.html";
	_defaultPages.push_back("index");
	_errorKeys.insert(404);
	_errorPages[404] = "404.html";
	_allowedMethods.push_back("GET");
	_allowedMethods.push_back("POST");
	_allowedMethods.push_back("DELETE");
	_cgiKeys.insert(".default");
	_cgiPath[".default"] = "path_to_default";
	_redirect = "redirect";
	_upload = "upload";
}

Rules::Rules(std::multimap< std::string, std::string > args, const Rules& rhs): Config() { // heritage foireux
	_args = args;
	setArgs(_args);
	*this |= rhs;
	if (args.count("location"))
		setLocation(args.equal_range("location"));
}

void	Rules::setAutoIndex(std::string str) {
//	std::cout << "autoIndex = " << str << std::endl;
	_autoIndex = (str == "true" || str == "yes" || str == "1");
}

void	Rules::setArgs(std::multimap< std::string, std::string > args) {
	// set default values or previous (parent) values
	if (args.count("root"))
		_root = args.find("root")->second;
	if (args.count("default_pages"))
		setVector(_defaultPages, args.equal_range("default_pages"));
	// errorPages ?
	if (args.count("error_pages"))
		setErrorPages(args.equal_range("error_pages"));
	if (args.count("cgi_path"))
		setCgiPath(args.equal_range("cgi_path"));
	if (args.count("auto_index"))
		setAutoIndex(args.find("auto_index")->second);
	if (args.count("allowed_methods"))
		setVector(_allowedMethods, args.equal_range("allowed_methods"));
	if (args.count("max_body_bytes"))
		_maxBodyBytes = parseMaxBodyBytes(args.find("max_body_bytes")->second);
//	_cgiPath = args.find("cgi_path")->second;
//	setVector(_cgiPath, args.equal_range("cgi_paths"));
	if (args.count("redirect"))
		_redirect = args.find("redirect")->second;
	if (args.count("upload"))
		_upload = args.find("upload")->second; // set default ?
}

void	Rules::setInheritArgs(std::multimap< std::string, std::string >& args) {
	std::set< std::string >::iterator	it = _argsToFind.begin(), ite = _argsToFind.end();
	while (it != ite) {
		if (!args.count(*it)) {
			t_range range = _inherit.equal_range(*it);
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
	std::string	str = mit->second;
	while (mit != mite) {
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

void	Rules::setCgiPath(t_range range) {
	t_multimap_it	mmIt = range.first, mmIte = range.second;
	std::string	str = mmIt->second;
	while (mmIt != mmIte) {
		size_t	pos1 = str.find(':'), pos2 = str.find(';');
		std::string	key = str.substr(0, pos1);
		if (!_cgiPath.count(key)) {
			_cgiKeys.insert(key);
			_cgiPath[key] = str.substr(pos1 + 1, pos2 - pos1 - 1);
		}
		mmIt++;
		str = mmIt->second;
	}
}

void	Rules::setLocation(t_range range) {
	t_multimap_it	mit = range.first, mite = range.second;
	while (mit != mite) {
		std::multimap< std::string, std::string >	args = parseLocationLine(mit->second);
		checkArgsFormat(args);
	//	inheritArgs(args); // not good
	//	printMultimap(_argsToFind, args);
	//	std::cout << args.equal_range("root").first->second << std::endl;

		if (!_locationKeys.size())
			return ;
		for (size_t i = 0, n = _locationKeys.size(); i < n; i++) {
			if (!_location.count(_locationKeys[i])) {
	//			Rules*	rules = new Rules(args, _args);
				Rules*	rules = new Rules(args, *this); //
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

void	Rules::setVector(std::vector< std::string >& vec, t_range range) {
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
		} while (!str.empty());
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
	_cgiPath.clear();
	_locationKeys.clear();
	_location.clear();
}

template< typename T >
static void	printVec(const std::vector< T >& vec, std::string tabs) {
	for (size_t i = 0, n = vec.size(); i < n; i++)
		std::cout << tabs << "\t" << vec[i] << std::endl;
}

template< typename T, typename U >
static void	printMap(std::set< T > keys, const std::map< T, U >& map, std::string tabs) {
	typename std::set< T >::iterator	keyIt = keys.begin(), keyIte = keys.end();
	while (keyIt != keyIte) {
		std::cout << tabs << "\t" << *keyIt << ":\t" << map.at(*keyIt) << std::endl;
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


void	Rules::goDeep(size_t i, std::string name) {
//	(void) i;
//	std::cout << "///////////////////////////////////////////////////////// i = " << i << std::endl;
	std::string	slashes(i * 8, '/');
	std::string	tabs(i, '\t');
	for (size_t pos = 0, n = _locationKeys.size(); pos < n; pos++) {
		std::cout	<< slashes << " GODEEP " << i << " : " << name << " " << this << " " << " contains " << _locationKeys[pos] << " " << _location[_locationKeys[pos]] << std::endl
					<< tabs << "root:\t" << getRoot() << std::endl
					<< tabs << "autoIndex:\t" << getAutoIndex() << std::endl
					<< tabs << "maxBodyBytes:\t" << getMaxBodyBytes() << std::endl
					<< tabs << "redirect:\t" << getRedirect() << std::endl
					<< tabs << "upload:\t" << getUpload() << std::endl
					<< tabs << "defaultPages:" << std::endl;
		printVec(getDefaultPages(), tabs);
		std::cout	<< tabs << "allowedMethods:" << std::endl;
		printVec(getAllowedMethods(), tabs);
		std::cout	<< tabs << "cgiPaths:" << std::endl;
	//	printVec(getCgiPaths(), tabs);
		printMap(getCgiKeys(), getCgiPath(), tabs);
		std::cout	<< tabs << "errorPages:" << std::endl;
		printMap(getErrorKeys(), getErrorPages(), tabs);
		std::cout << std::endl;
		_location[_locationKeys[pos]]->goDeep(i + 1, _locationKeys[pos]);
	}
}

std::ostream&	operator<<(std::ostream& os, const Rules& rhs) {
	std::string	tabs('\t', 1);
	os	<< "root:\t" << rhs.getRoot() << std::endl
		<< "autoIndex:\t" << rhs.getAutoIndex() << std::endl
		<< "maxBodyBytes:\t" << rhs.getMaxBodyBytes() << std::endl
		<< "redirect:\t" << rhs.getRedirect() << std::endl
		<< "upload:\t" << rhs.getUpload() << std::endl
		<< "defaultPages:" << std::endl;
	printVec(rhs.getDefaultPages(), tabs);
	std::cout	<< "allowedMethods:" << std::endl;
	printVec(rhs.getAllowedMethods(), tabs);
	std::cout	<< "cgiPaths:" << std::endl;
	printMap(rhs.getCgiKeys(), rhs.getCgiPath(), tabs);
	std::cout	<< "errorPages:" << std::endl;
	printMap(rhs.getErrorKeys(), rhs.getErrorPages(), tabs);
	printLocation(rhs.getLocationKeys(), rhs.getLocation());
	return (os);
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
		if (!_errorKeys.count(*setItA))
			_errorKeys.insert(*setItA);
		_errorPages[*setItA] = rhs.getErrorPages().at(*setItA);
		setItA++;
	}
	std::set< std::string >::iterator	setItB = rhs.getCgiKeys().begin(), setIteB = rhs.getCgiKeys().end();
	while (setItB != setIteB) {
		if (!_cgiKeys.count(*setItB))
			_cgiKeys.insert(*setItB);
		_cgiPath[*setItB] = rhs.getCgiPath().at(*setItB);
		setItB++;
	}

	return (*this);
}