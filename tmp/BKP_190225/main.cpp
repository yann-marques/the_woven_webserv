#include "Config.hpp"

int	main(int argc, char** argv) {
	if (argc == 2) {
		Config	config(argv[1]);
	//	std::cout << config << std::endl;
	}
	return (0);
}