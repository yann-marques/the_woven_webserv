#include "Config.hpp"

static void	checkPortFormat(size_t count, t_range range) {
	if (!count)
		throw Config::MissingPortException();
	else if (count > 1)
		throw Config::DoubleArgException("port");

	t_multimap_it	mit = range.first;
	std::string	str = mit->second;
	std::string::iterator	it = str.begin(), ite = str.end();
	ite--;
	while (it != ite && isdigit(*it))
		it++;
	if (it != ite)
		throw Config::UnexpectedValueException(str);

}
/*
static void	checkServerNames(std::multimap< std::stirng, std::string >& args, std::string key) {
	t_multimap_it	mit = range.first;
	if (!count)
		mit->second = "DEFAULT"; /////
	// else verification de caracteres speciaux ?
}

static void	checkAllowedMethodsFormat(size_t count, t_range range) {
	std::vector< std::string >	methods;
	methods.push_back("GET");
	methods.push_back("POST");
	methods.push_back("DELETE");

	t_multimap_it	mit = range.first;
	std::string&	str = mit->second;
	if (!count) {
		for (size_t i = 0; i < 3; i++) {
			str += methods[i];
			if (i < 2)
				str += ",";
			else
				str += ";";
		}
	} else {
		size_t	pos1 = 0, pos2, i = 0;
		while (str[pos1] && str[pos1] != ';') {
			pos2 = str.find(',', pos1);
			if (pos2 == std::string::npos)
				pos2 = str.find(';');
			for (i = 0; i < 3; i++)
				if (!str.compare(pos1, pos2 - pos1, methods[i]))
					break ;
			if (i == 3)
				throw Config::UnexpectedValueException(str);
			pos1 = pos2 + 1;
		}
	}
}

static void	checkMaxBodyBytes(size_t count, t_range range) {
	t_multimap_it	mit = range.first;
	if (!count)
		mit->second = "1024"; // ? set default value
	else if (count > 1)
		throw Config::DoubleArgException(mit->first);
	else { // limit ?
		std::string	mkg("MKG");
		std::string	str = mit->second;
//		size_t	len = arg.size() - 1; // len of string minus ;
		size_t	pos = 0;
		while (str[pos] && isdigit(str[pos]))
			pos++;
		if (mkg.find(str[pos]) != std::string::npos) {
			pos++;
			while (str[pos] && isdigit(str[pos]))
				pos++;
		}
		if (str[pos] != ';')
			throw Config::UnexpectedValueException(mit->second);

	//	std::cout << "substr = " << arg.substr(0, len) << std::endl;
		// parse M K G later
///// .....
	}
}

static void	checkAutoIndex(size_t count, t_range range) {
	t_multimap_it	mit = range.first;
	std::string	str = mit->second;
	str = str.substr(0, str.size() - 1);
	if (!count)
		mit->second = "true"; //
	else if (count > 1)
		throw Config::DoubleArgException(mit->first);
	else if (str != "yes" && str != "true" && str != "1"
		&& str != "no" && str != "false" && str != "0")
		throw Config::UnexpectedValueException(str);
}
*/
// WARNING: tenter d'acceder a une value de key "key1" creera une value ""
// a la key "key1", et donc map.count("key1") renverra 1

void	Config::checkArgsFormat(std::multimap< std::string, std::string >& args) {
	// set default values if there is none
	// do it in Rules() ? maybe better
	std::set< std::string >::iterator	it = _argsToFind.begin(), ite = _argsToFind.end();
	while (it != ite) {
		std::string	argToFind = *it;
		if (!args.count(argToFind)) {
		//	if (argToFind == "port")
		//		throw (MissingPortException());
			args.insert(std::make_pair(argToFind, _defaultValues[argToFind]));
		}
		it++;
	}
//	printMultimap(_argsToFind, args);


	checkPortFormat(args.count("port"), args.equal_range("port"));
/*
//	checkServerNames(args.count("server_names"), args.equal_range("server_names"));
	checkServerNames(args, "server_names");
	// to print a multimap value

//	std::cout << "count = " << args.count("allowed_methods") << std::endl;

	checkAllowedMethodsFormat(args.count("allowed_methods"), args.equal_range("allowed_methods"));

	checkMaxBodyBytes(args.count("max_body_bytes"), args.equal_range("max_body_bytes"));

//	std::cout << "mbb = " << args["max_body_bytes"] << std::endl;
//	std::cout << "mbb count = " << args.count("max_body_bytes") << std::endl;

	checkAutoIndex(args.count("auto_index"), args.equal_range("auto_index"));
	// ...
*/
}