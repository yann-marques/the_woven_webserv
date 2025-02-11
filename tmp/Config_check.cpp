#include "Config.hpp"

static void	checkPortFormat(size_t count, std::string arg) { // & ?
	if (!count)
		throw Config::MissingPortException(); // set default value ?
	std::string::iterator	it = arg.begin(), ite = arg.end();
	ite--;
	while (it != ite && isdigit(*it))
		it++;
	if (it != ite)
		throw Config::UnexpectedValueException(arg);
}

static void	checkAllowedMethodsFormat(size_t count, std::string& arg) {
	std::vector< std::string >	methods;
	methods.push_back("GET");
	methods.push_back("POST");
	methods.push_back("DELETE");

	if (!count) {
		for (size_t i = 0; i < 3; i++) {
			arg += methods[i];
			if (i < 2)
				arg += ",";
			else
				arg += ";";
		}
	} else {
		size_t	pos1 = 0, pos2, i = 0;
		while (arg[pos1] && arg[pos1] != ';') {
			pos2 = arg.find(',', pos1);
			if (pos2 == std::string::npos)
				pos2 = arg.find(';');
			for (i = 0; i < 3; i++)
				if (!arg.compare(pos1, pos2 - pos1, methods[i]))
					break ;
			if (i == 3)
				throw Config::UnexpectedValueException(arg);
			pos1 = pos2 + 1;
		}
	}
}

static void	checkMaxBodyBytes(size_t count, std::string& arg) {
	if (!count)
		arg = "1024"; // ? set default value
	else { // limit ?
		std::string	mkg("MKG");
//		size_t	len = arg.size() - 1; // len of string minus ;
		size_t	pos = 0;
		while (arg[pos] && isdigit(arg[pos]))
			pos++;
		if (mkg.find(arg[pos]) != std::string::npos) {
			pos++;
			while (arg[pos] && isdigit(arg[pos]))
				pos++;
		}
		if (arg[pos] != ';')
			throw Config::UnexpectedValueException(arg);

	//	std::cout << "substr = " << arg.substr(0, len) << std::endl;
		// parse M K G later
///// .....
	}
}

static void	checkAutoIndex(size_t count, std::string& arg) {
	if (!count)
		arg = "true"; //
}

// WARNING: tenter d'acceder a une value de key "key1" creera une value ""
// a la key "key1", et donc map.count("key1") renverra 1

void	Config::checkArgsFormat(std::map< std::string, std::string > args) {
	// check port // if not a location ?
	checkPortFormat(args.count("port"), args["port"]);
//	std::cout << "count = " << args.count("allowed_methods") << std::endl;
	checkAllowedMethodsFormat(args.count("allowed_methods"), args["allowed_methods"]);
	checkMaxBodyBytes(args.count("max_body_bytes"), args["max_body_bytes"]);
//	std::cout << "mbb = " << args["max_body_bytes"] << std::endl;
//	std::cout << "mbb count = " << args.count("max_body_bytes") << std::endl;
	checkAutoIndex(args.count("auto_index"), args["auto_index"]);
}