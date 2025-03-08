#include "Config.hpp"

static bool    isDigitStr(std::string str) {
    size_t    i = 0, ie = str.size();

    while (i < ie && isdigit(str[i]))
        i++;

    return (i == ie);
}

std::vector< std::string >    ft_split(std::string str, char sep) {
    std::vector< std::string >    vec;
    size_t    pos = 0;

    do {
        pos = str.find(sep, pos);
        std::string    subStr = str.substr(0, pos);

        if (!subStr.empty())
            vec.push_back(subStr);

        pos += (str[pos] == sep);

        str.erase(0, pos);
        pos = 0;
    } while (!str.empty());

    return (vec);
}

static bool    isValidHost(std::string str) {
    if (str.find("..") != std::string::npos
        || str[0] == '.' || str[str.size() - 1] == '.')
        return (false);

    std::vector< std::string >    vec = ft_split(str, '.');
    size_t    i = 0, ie = vec.size();
    if (ie != 4)
        return (false);

    while (i < ie && isDigitStr(vec[i]) && vec[i].size() <= 3) {
        int n = std::atoi(vec[i].c_str());
        if (n < 0 || n > 255)
            break ;
        i++;
    }

    return (i == ie);
}

static void	checkHost(t_mmap_range< std::string, std::string >::t range) {
	std::string	str = range.first->second;
	if (!isValidHost(str))
		throw (Config::UnexpectedValueException(str));
}

static void	checkPortFormat(size_t count, t_mmap_range< std::string, std::string >::t mmRange) {
	if (!count)
		throw Config::MissingPortException();
	else if (count > 1)
		throw Config::DoubleArgException("port");

	t_mmap_it< std::string, std::string >::t	mit = mmRange.first;
	std::string	str = mit->second;
	std::string::iterator	it = str.begin(), ite = str.end();
	ite--;
	while (it != ite && isdigit(*it))
		it++;
	if (it != ite)
		throw Config::UnexpectedValueException(str);

}

static void	checkErrorPages(t_mmap_range< std::string, std::string >::t mmRange) {
	t_mmap_it< std::string, std::string >::t	mmIt = mmRange.first, mmIte = mmRange.second;
	do {
		std::string	str = mmIt->second;
		size_t	pos1 = str.find(':');
		if (pos1 == std::string::npos)
			throw(Config::ConfigSyntaxException());
		std::string	key(str.substr(0, pos1)), value(str.substr(pos1 + 1));
		if (!isDigitStr(key)) // + verification du format de value ?
			throw (Config::UnexpectedKeyException(key));
		mmIt++;
	} while (mmIt != mmIte);
}

static void	checkCgiPath(t_mmap_range< std::string, std::string >::t range) {
	t_mmap_it< std::string, std::string >::t	mmIt = range.first, mmIte = range.second;
	do {
		std::string	str = mmIt->second;
		size_t	pos1 = str.find(':');
		if (pos1 == std::string::npos) {
			throw(Config::ConfigSyntaxException());
		}
		std::string	key(str.substr(0, pos1)), value(str.substr(pos1 + 1));
		if (key[0] != '.') // + verification du format de value ?
			throw (Config::UnexpectedKeyException(key));
		mmIt++;
	} while (mmIt != mmIte);
}

void	Config::checkArgsFormat(const std::multimap< std::string, std::string >& args) const {
	size_t	count = args.count("host");
	if (count > 1)
		throw DoubleArgException("host");
	else if (count)
		checkHost(args.equal_range("host"));

	checkPortFormat(args.count("port"), args.equal_range("port"));

	// check server_names ?

	if (args.count("error_pages"))
		checkErrorPages(args.equal_range("error_pages"));

	if (args.count("cgi_path"))
		checkCgiPath(args.equal_range("cgi_path"));
}