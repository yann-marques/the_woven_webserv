#ifdef BONUS
# include "HTTPRequest.hpp"

t_mmap_range< std::string, std::string >::t	HttpRequest::getCookiesRange() const {
	return (_headers.equal_range("Cookie"));
}

std::set< std::string >	HttpRequest::parseRequestCookieRange(t_mmap_range< std::string, std::string >::t range) {
	std::set< std::string >	cookieSet;
	t_mmap_it< std::string, std::string >::t
		mmIt = range.first, mmIte = range.second;
	while (mmIt != mmIte) {
		std::string str = mmIt->second;
		size_t	pos1 = 0, pos2 = str.find('=');
		while (pos1 != std::string::npos && pos2 != std::string::npos) {
			cookieSet.insert(str.substr(pos1, pos2 - pos1));
			pos1 = str.find(';', pos2);
			if (pos1 != std::string::npos) {
				pos1++;
				if (isspace(str[pos1]))
					pos1++;
			}
			pos2 = str.find('=', pos1);
		}
		mmIt++;
	}
	return (cookieSet);
}

std::set< std::string >	HttpRequest::getCookieSet() {
	return (parseRequestCookieRange(getCookiesRange()));
}

static std::string	getTimeStamp() {
	std::time_t	time = std::time(NULL);
	return (std::string(std::asctime(std::localtime(&time))));
}

void    HttpRequest::setResponseCookies(const std::set< std::string >& requestCookies) {
	std::string	cookieValue = getTimeStamp();
	(void) requestCookies;
	if (!requestCookies.count("sessionStart")) {
		_headers.insert(make_pair("Set-Cookie", "sessionStart=" + cookieValue + "; Max-Age=10; HttpOnly"));
	}
	_headers.insert(make_pair("Set-Cookie", "actualTime=" + cookieValue + "; Max-Age=10; HttpOnly"));
}

#endif