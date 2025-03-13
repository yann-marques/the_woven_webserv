#include "AParser.hpp"

void	AParser::checkErrorPages(t_mmap_range< std::string, std::string >::t range) const {
	t_mmap_it< std::string, std::string >::t	mmIt = range.first, mmIte = range.second;
	if (mmIt == mmIte)
		return ;
	do {
		std::string	str = mmIt->second;
		size_t	pos1 = str.find(':');
		if (pos1 == std::string::npos)
			throw(ConfigSyntaxException());
		std::string	key(str.substr(0, pos1)), value(str.substr(pos1 + 1));
		if (!isDigitString(key))
			throw (UnexpectedKeyException(key));
		mmIt++;
	} while (mmIt != mmIte);
}

void	AParser::checkCgiPath(t_mmap_range< std::string, std::string >::t range) const {
	t_mmap_it< std::string, std::string >::t	mmIt = range.first, mmIte = range.second;
	if (mmIt == mmIte)
		return ;
	do {
		std::string	str = mmIt->second;
		size_t	pos1 = str.find(':');
		if (pos1 == std::string::npos) {
			throw(ConfigSyntaxException());
		}
		std::string	key(str.substr(0, pos1)), value(str.substr(pos1 + 1));
		if (key[0] != '.')
			throw (UnexpectedKeyException(key));
		mmIt++;
	} while (mmIt != mmIte);
}

void	AParser::checkArgNoDouble(t_mmap_range< std::string, std::string >::t range, bool f(std::string)) const {
	t_mmap_it< std::string, std::string >::t	it = range.first, ite = range.second;
	if (it == ite)
		return ;
	std::string	str = it->second;
	if (!f(str))
		throw (UnexpectedValueException(str));
	if (++it != ite)
		throw (DoubleArgException(it->first));
}