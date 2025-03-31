#include "Config.hpp"

void	Config::checkPortFormat(t_mmap_range< std::string, std::string >::t range) const {
	if (range.first == range.second)
		throw MissingPortException();
	checkArgNoDouble(range, isDigitString);
}

void	Config::checkServerNames(t_mmap_range< std::string, std::string >::t mmRange) const {
	t_mmap_it< std::string, std::string >::t
		mmIt = mmRange.first, mmIte = mmRange.second;
	while (mmIt != mmIte && mmIt->second.find_first_of(FORBIDDEN_CHARS) == std::string::npos) {
		mmIt++;
	}
	if (mmIt != mmIte)
		throw (ForbiddenCharException(mmIt->second));
}

void	Config::checkArgsFormat(const std::multimap< std::string, std::string >& args) const {
	noEmptyStrValues(args);

	checkArgNoDouble(args.equal_range("host"), isValidHost);
	checkPortFormat(args.equal_range("port"));
	checkServerNames(args.equal_range("server_names"));

	checkArgNoDouble(args.equal_range("auto_index"), isValidAutoIndex);
	checkArgNoDouble(args.equal_range("max_body_bytes"), isValidMaxBodyBytes);
	checkArgNoDouble(args.equal_range("root"), noForbiddenChar);
	checkArgNoDouble(args.equal_range("redirect"), noForbiddenChar);
	checkArgNoDouble(args.equal_range("upload"), noForbiddenChar);

	checkRoot(args.equal_range("root"));

	checkAllowedMethods(args.equal_range("allowed_methods"));
	checkErrorPages(args.equal_range("error_pages"));
	checkCgiPath(args.equal_range("cgi_path"));
}