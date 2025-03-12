#ifndef CHECK_UTILS_HPP
# define CHECK_UTILS_HPP

# include <cctype>
# include <vector>
# include <string>
# include <cstdlib>

# define FORBIDDEN_CHARS "\'\"[]()<>{}"

std::vector< std::string >    ft_split(std::string str, char sep);
bool	bracketsAreClosed(std::string str);
bool	badSpaces(std::string str);
bool	isDigitString(std::string str);
bool    isValidHost(std::string str);
bool	isValidAutoIndex(std::string str);
bool	isValidMaxBodyBytes(std::string str);
bool	noForbiddenChar(std::string str);
bool	isValidLocationKey(std::string key);

#endif