#include "Rules.hpp"

void	Rules::checkArgsFormat(const std::multimap< std::string, std::string >& args) const {
	checkArgNoDouble(args.equal_range("auto_index"), isValidAutoIndex);
	checkArgNoDouble(args.equal_range("max_body_bytes"), isValidMaxBodyBytes);
	checkArgNoDouble(args.equal_range("root"), noForbiddenChar);
	checkArgNoDouble(args.equal_range("redirect"), noForbiddenChar);
	checkArgNoDouble(args.equal_range("upload"), noForbiddenChar);

	checkErrorPages(args.equal_range("error_pages"));
	checkCgiPath(args.equal_range("cgi_path"));
}