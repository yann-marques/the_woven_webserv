#include "AParser.hpp"

void	AParser::checkErrorPages(t_mmap_range< std::string, std::string >::t range) const {
	t_mmap_it< std::string, std::string >::t	mmIt = range.first, mmIte = range.second;

	if (mmIt->second.empty())
		return ;
	while (mmIt != mmIte) {
		std::string	str = mmIt->second;
		size_t	pos1 = str.find(':');
		if (pos1 == std::string::npos)
			throw(ConfigSyntaxException());
		std::string	key(str.substr(0, pos1)), value(str.substr(pos1 + 1));
		if (!isDigitString(key))
			throw (UnexpectedKeyException(key));
		mmIt++;
	}
}

void	AParser::checkCgiPath(t_mmap_range< std::string, std::string >::t range) const {
	t_mmap_it< std::string, std::string >::t	mmIt = range.first, mmIte = range.second;

	if (mmIt->second.empty())
		return ;
	while (mmIt != mmIte){
		std::string	str = mmIt->second;
		size_t	pos1 = str.find(':');
		if (pos1 == std::string::npos) {
			throw(ConfigSyntaxException());
		}
		std::string	key(str.substr(0, pos1)), value(str.substr(pos1 + 1));
		if (key[0] != '.')
			throw (UnexpectedKeyException(key));
		mmIt++;
	}
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

void	AParser::checkAllowedMethods(t_mmap_range< std::string, std::string >::t mmRange) const {
	t_mmap_it< std::string, std::string >::t	mmIt = mmRange.first, mmIte = mmRange.second;
	std::vector< std::string >	methodsVec;
	methodsVec.push_back("GET");
	methodsVec.push_back("POST");
	methodsVec.push_back("DELETE");
	methodsVec.push_back("HEAD");
	t_vec_range< std::string >::t	vecRange(methodsVec.begin(), methodsVec.end());
	while (mmIt != mmIte && isInVecRange< std::string >(vecRange, mmIt->second))
		mmIt++;
	if (mmIt != mmIte)
		throw (UnexpectedValueException(mmIt->second));
}

void	AParser::checkRoot(t_mmap_range< std::string, std::string >::t mmRange) const {
	t_mmap_it< std::string, std::string >::t	mmIt = mmRange.first, mmIte = mmRange.second;
	if (mmIt == mmIte)
		return ;
	std::string	str = mmIt->second;
	if (str[str.size() - 1] != '/')
		throw (UnexpectedValueException(str + " ; root must end with '/'"));
	mmIt++;
	if (mmIt != mmIte)
		throw (DoubleArgException("root"));
}

void	AParser::noEmptyStrValues(std::multimap< std::string, std::string > args) const {
	t_mmap_it< std::string, std::string >::t	it = args.begin(), ite = args.end();

	while (it != ite) {
		if (it->first != "error_pages" && it->first != "cgi_path"
			&& it->second.empty())
			throw (EmptyStrException());
		it++;
	}
}