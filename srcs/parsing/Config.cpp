#include "Config.hpp"
#include "Rules.hpp"

Config::Config(): AParser() {
	setArgsToFind();
}

Config::Config(const char* fileName): AParser() {
	setArgsToFind();

	try {
		std::string	fileContent = extractFileContent(fileName);
		if (!bracketsAreClosed(fileContent))
			throw UnclosedScopeException();

		std::vector< std::string >	serverLines = splitScope(fileContent, "server");
		deleteBrackets(serverLines);
		std::multimap< std::string, std::multimap< std::string, std::string > >	hostArgs;
		for (size_t i = 0, n = serverLines.size(); i < n; i++) {
			std::multimap< std::string, std::string >	args = parseLine(serverLines[i]);
			checkArgsFormat(args);

			std::string	host;
			if (args.count("host"))
				host = args.find("host")->second;
			else
				host = "127.0.0.1";

			hostArgs.insert(make_pair(host, args));
		}
		t_mmap_it< std::string, std::multimap< std::string, std::string > >::t
			hostIt = hostArgs.begin(), hostIte = hostArgs.end();
		while (hostIt != hostIte) {
			setArgsByHost(hostArgs.equal_range(hostIt->first));
			hostIt++;
		}
	} catch (AParser::ArgOutOfServerScopeException& e) {
		std::cerr << e.what() << std::endl;
		destruct();
		throw(e);
	} catch (AParser::ConfigSyntaxException& e) {
		std::cerr << e.what() << std::endl;
		destruct();
		throw(e);
	} catch (AParser::UnexpectedKeyException& e) {
		std::cerr << e.what() << std::endl;
		destruct();
		throw(e);
	} catch (AParser::UnexpectedValueException& e) {
		std::cerr << e.what() << std::endl;
		destruct();
		throw(e);
	} catch (AParser::DoubleArgException& e) {
		std::cerr << e.what() << std::endl;
		destruct();
		throw(e);
	} catch (AParser::ForbiddenCharException& e) {
		std::cerr << e.what() << std::endl;
		destruct();
		throw(e);
	}
	// Config exceptions
	catch (Config::IsDirException& e) {
		std::cerr << e.what() << std::endl;
		destruct();
		throw(e);
	} catch (Config::OpenFileException& e) {
		std::cerr << e.what() << std::endl;
		destruct();
		throw(e);
	} catch (Config::UnclosedScopeException& e) {
		std::cerr << e.what() << std::endl;
		destruct();
		throw(e);
	} catch (Config::BadSpacesException& e) {
		std::cerr << e.what() << std::endl;
		destruct();
		throw(e);
	} catch (Config::MissingPortException& e) {
		std::cerr << e.what() << std::endl;
		destruct();
		throw(e);
	}
	// Rules exceptions
	catch (Rules::RedefinedArgException& e) {
		std::cerr << e.what() << std::endl;
		destruct();
		throw(e);
	} catch (Rules::InvalidLocationKeyException& e) {
		std::cerr << e.what() << std::endl;
		destruct();
		throw(e);
	} catch (std::exception& e) {
		destruct();
		throw(e);
	}

}


Config::Config(const Config& rhs): AParser() {
	setArgsToFind();
	*this = rhs;
}

Config&	Config::operator=(const Config& rhs) {
	t_map_it< std::string, std::map< int, std::map< std::string, Rules* > > >::t
		hostIt = rhs.getParsedConfig().begin(), hostIte = rhs.getParsedConfig().end();
	while (hostIt != hostIte) {
		std::string host = hostIt->first;
		t_map_it< int, std::map< std::string, Rules* > >::t
			portIt = hostIt->second.begin(), portIte = hostIt->second.end();
			while (portIt != portIte) {
				int	port = portIt->first;
				t_map_it< std::string, Rules* >::t
					sNamesIt = portIt->second.begin(), sNamesIte = portIt->second.end();
				while (sNamesIt != sNamesIte) {
					std::string	serverName = sNamesIt->first;
					_parsedConfig[host][port][serverName] = new Rules(*(sNamesIt->second));
					sNamesIt++;
				}
				portIt++;
			}
		hostIt++;
	}

	return (*this);
}

std::string	Config::extractFileContent(const char* fileName) {
	int	fd = open(fileName, O_DIRECTORY);
	if (fd > -1) {
		close(fd);
		throw (IsDirException(fileName));
	}
	std::ifstream	ifs(fileName);
	if (!ifs.is_open())
		throw (OpenFileException(fileName));
	
	std::stringstream	buffer;
	buffer << ifs.rdbuf();
	std::string	fileContent = buffer.str();

	ifs.close();
	if (badSpaces(fileContent))
		throw BadSpacesException();

	std::istringstream	iss(fileContent);
	fileContent.clear();

	std::string	word;
	while (iss >> word)
		fileContent += word;

	return (fileContent);
}

const std::map< std::string, std::map< int, std::map< std::string, Rules* > > >&	Config::getParsedConfig() const {
	return (_parsedConfig);
}

void	Config::destruct() {
	t_mmap_it< std::string, std::map< int, std::map< std::string, Rules* > > >::t
		hostIt = _parsedConfig.begin(), hostIte = _parsedConfig.end();
	while (hostIt != hostIte) {
		t_mmap_it< int, std::map< std::string, Rules* > >::t
			portIt = hostIt->second.begin(), portIte = hostIt->second.end();
		while (portIt != portIte) {
			t_mmap_it< std::string, Rules* >::t
				sNamesIt = portIt->second.begin(), sNamesIte = portIt->second.end();
			while (sNamesIt != sNamesIte) {
				Rules*	ptr = sNamesIt->second;
				if (ptr != NULL) {
					delete ptr;
					ptr = NULL;
				}
				sNamesIt++;
			}
			portIt++;
		}
		hostIt++;
	}
}

Config::~Config() {
	destruct();
}

std::ostream&	operator<<(std::ostream& os, const Config& rhs) {
	const std::map< std::string, std::map< int, std::map< std::string, Rules* > > >&
		configRef = rhs.getParsedConfig();
	std::cout	<< "########################################################## CONFIG" << std::endl << std::endl;
	t_mmap_it< std::string, std::map< int, std::map< std::string, Rules* > > >::t
		hostIt = configRef.begin(), hostIte = configRef.end();
	std::string	host = "";
	while (hostIt != hostIte) {
		if (host != hostIt->first)
			std::cout << "################################################### HOST " << hostIt->first << std::endl << std::endl;
		host = hostIt->first;
		t_mmap_it< int, std::map< std::string, Rules* > >::t
			portIt = hostIt->second.begin(), portIte = hostIt->second.end();
		int	port = -1;
		while (portIt != portIte) {
			if (port != portIt->first)
				std::cout << "############################################ PORT " << portIt->first << std::endl << std::endl;
			port = portIt->first;
			t_mmap_it< std::string, Rules* >::t
				sNamesIt = portIt->second.begin(), sNamesIte = portIt->second.end();
			while (sNamesIt != sNamesIte) {
				Rules*	ptr = sNamesIt->second;
				if (ptr) {
					ptr->printDeep(0, sNamesIt->first);
				}
				sNamesIt++;
			}
			portIt++;
		}
		hostIt++;
	}
	return (os);
}
