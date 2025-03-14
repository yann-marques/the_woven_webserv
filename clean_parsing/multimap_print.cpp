#include "Config.hpp"

static void	printRange(t_range range) {
	t_multimap_it	it = range.first, ite = range.second;
	while (it != ite) {
		std::cout << '\t' << it->second << std::endl;
		it++;
	}
}

void	printMultimap(std::set< std::string > keys, std::multimap< std::string, std::string > map) {
	std::set< std::string >::iterator	it = keys.begin(), ite = keys.end();
	while (it != ite) {
		std::cout << *it << std::endl;
		printRange(map.equal_range(*it));
		it++;
	}
}