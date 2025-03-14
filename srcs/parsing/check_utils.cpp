#include "check_utils.hpp"

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

bool	bracketsAreClosed(std::string str) {
	std::string	limiter("{}");

	size_t	count = 0, i = 0;
	while (str[i]) {
		while (str[i] && limiter.find(str[i], 0) == std::string::npos)
			i++;
		if (str[i] == limiter[0])
			count++;
		else if (str[i] == limiter[1])
			count--;
		i++;
	}
	return (count == 0);
}

bool	badSpaces(std::string str) {
	for (size_t i = 0; str[i]; i++) {
		if (std::isspace(str[i]) && i > 0
			&& isalnum(str[i - 1])) {
			while (str[i] && std::isspace(str[i]))
				i++;
			if (!str[i] || isalnum(str[i]))
				return (true);
		}
	}
	return (false);
}

bool	isDigitString(std::string str) {
	std::string::iterator	it = str.begin(), ite = str.end();
	while (it != ite && isdigit(*it))
		it++;
	return (it == ite);
}

bool    isValidHost(std::string str) {
    if (str.find("..") != std::string::npos
        || str[0] == '.' || str[str.size() - 1] == '.')
        return (false);

    std::vector< std::string >    vec = ft_split(str, '.');
    size_t    i = 0, ie = vec.size();
    if (ie != 4)
        return (false);

    while (i < ie && isDigitString(vec[i]) && vec[i].size() <= 3) {
        int n = std::atoi(vec[i].c_str());
        if (n < 0 || n > 255)
            break ;
        i++;
    }

    return (i == ie);
}

bool	isValidAutoIndex(std::string str) {
	return (str == "yes" || str == "true" || str == "1"
		|| str == "no" || str == "false" || str == "0");
}

bool	isValidMaxBodyBytes(std::string str) {
	std::string	mkg("MKG");
	size_t	pos = 0;
	while (str[pos] && isdigit(str[pos]))
		pos++;
	if (mkg.find(str[pos]) != std::string::npos) {
		pos++;
		while (str[pos] && isdigit(str[pos]))
			pos++;
	}
	return (!str[pos]);
}

bool	noForbiddenChar(std::string str) {
	return (str.find_first_of(FORBIDDEN_CHARS) == std::string::npos);
}

bool	isValidLocationKey(std::string key) {
	if (key[0] != '/')
		return (false);
	size_t	i = 1;
	while (isalnum(key[i]) || key[i] == '_')
		i++;
	return (!key[i]);
}