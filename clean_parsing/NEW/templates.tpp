#ifndef TEMPLATES_TPP
# define TEMPLATES_TPP

template< typename Key, typename Value >
struct	t_map_it {
	typedef typename std::map< Key, Value >::const_iterator	t;
};
// std::map< int, std::string >::iterator	mIt;
// t_map_it< int, std::string >::t	mIt;

template< typename Key, typename Value >
struct	t_mmap_it {
	typedef typename std::multimap< Key, Value >::const_iterator	t;
};
// std::multimap< int, std::string >::iterator	mmIt;
// t_mmap_it< int, std::string >::t	mmIt;

template< typename Key, typename Value >
struct	t_mmap_range {
	typedef typename std::pair< typename t_mmap_it< Key, Value >::t, typename t_mmap_it< Key, Value >::t >	t;
};
// std::pair< std::multimap< int, std::string >::iterator, std::multimap< int, std::string >::iterator >	mmRange;
//	t_mmap_range< int, std::string >::t	mmRange;

template< typename Key >
struct	t_set_it {
	typedef typename std::set< Key >::iterator	t;
};
// std::set< int >::iterator	setIt;
// t_set_it< int >::t	setIt;

template< typename T >
void	printVec(const std::vector< T >& vec, std::string tabs) {
	for (size_t i = 0, n = vec.size(); i < n; i++)
		std::cout << tabs << '\t' << vec[i] << std::endl;
}

template< typename T, typename U >
void	printMap(std::set< T > keys, const std::map< T, U >& map, std::string tabs) {
	typename t_set_it< T >::t	keyIt = keys.begin(), keyIte = keys.end();
	while (keyIt != keyIte) {
		std::cout << tabs << '\t' << *keyIt << ": " << map.at(*keyIt) << std::endl;
		keyIt++;
	}
}

template< typename Key, typename Value >
void	printMultimap(std::set< Key > keys, const std::multimap< Key, Value >& mmap) {
	typename t_set_it< Key >::t	keyIt = keys.begin(), keyIte = keys.end();
	while (keyIt != keyIte) {
		typename t_mmap_range< Key, Value >::t	mmRange = mmap.equal_range(*keyIt);
		typename t_mmap_it< Key, Value >::t	mmIt = mmRange.first, mmIte = mmRange.second;
		while (mmIt != mmIte) {
			std::cout << *keyIt << ": " << mmIt->second << std::endl;
			mmIt++;
		}
		keyIt++;
	}
}

template< typename Key, typename Value >
bool	isInRange(typename t_mmap_range< Key, Value >::t range, Value toFind) {
	typename t_mmap_it< Key, Value >::t	mmIt = range.first, mmIte = range.second;
	while (mmIt != mmIte && mmIt->second != toFind)
		mmIt++;
	return (mmIt != mmIte);
}

#endif