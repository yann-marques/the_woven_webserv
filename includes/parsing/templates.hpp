#ifndef TEMPLATES_TPP
# define TEMPLATES_TPP

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <set>

template< typename Key, typename Value >
struct	t_map_it {
	typedef typename std::map< Key, Value >::const_iterator	t;
};

template< typename Key, typename Value >
struct	t_mmap_it {
	typedef typename std::multimap< Key, Value >::const_iterator	t;
};

template< typename Key, typename Value >
struct	t_mmap_range {
	typedef typename std::pair< typename t_mmap_it< Key, Value >::t, typename t_mmap_it< Key, Value >::t >	t;
};

template< typename Key >
struct	t_set_it {
	typedef typename std::set< Key >::iterator	t;
};

template< typename T >
struct	t_vec_it {
	typedef typename std::vector< T >::const_iterator	t;
};

template< typename T >
struct	t_vec_range {
	typedef typename std::pair< typename t_vec_it< T >::t, typename t_vec_it< T >::t >	t;
};

template< typename Container >
struct t_container_it {
	typedef typename Container::const_iterator	t;
};

template< typename Container >
struct t_container_range {
	typedef typename std::pair< typename t_container_it< Container >::t, typename t_container_it< Container >::t >	t;
};

/*
template< typename T >
void	printVec(const std::vector< T >& vec, const std::string& tabs);

template< typename T, typename U >
void	printMap(const std::set< T >& keys, const std::map< T, U >& map, const std::string& tabs);

template< typename Key, typename Value >
void	printMultimap(const std::set< Key >& keys, const std::multimap< Key, Value >& mmap);

template< typename Key, typename Value >
bool	isInMMRange(typename t_mmap_range< Key, Value >::t& range, const Value& toFind);
	
template< typename T >
bool	isInVecRange(typename t_vec_range< T >::t& range, const T& value);
*/

/*
template< typename Container, typename T >
bool	isInRange(typename t_container_range< Container >::t range, T value) {
	typename t_container_it< Container >::t	it = range.first, ite = range.second;
	while (it != ite && *it != value)
		it++;
	return (it != ite);
}
*/
template< typename T >
void	printVec(const std::vector< T >& vec, const std::string& tabs) {
	for (size_t i = 0, n = vec.size(); i < n; i++)
		std::cout << tabs << '\t' << vec[i] << std::endl;
}

template< typename T, typename U >
void	printMap(const std::set< T >& keys, const std::map< T, U >& map, const std::string& tabs) {
	typename t_set_it< T >::t	keyIt = keys.begin(), keyIte = keys.end();
	while (keyIt != keyIte) {
		std::cout << tabs << '\t' << *keyIt << ": " << map.at(*keyIt) << std::endl;
		keyIt++;
	}
}

template< typename Key, typename Value >
void	printMultimap(const std::set< Key >& keys, const std::multimap< Key, Value >& mmap) {
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
bool	isInMMRange(typename t_mmap_range< Key, Value >::t range, const Value& toFind) {
	typename t_mmap_it< Key, Value >::t	mmIt = range.first, mmIte = range.second;
	while (mmIt != mmIte && mmIt->second != toFind)
		mmIt++;
	return (mmIt != mmIte);
}

template< typename T >
bool	isInVecRange(typename t_vec_range< T >::t& range, const T& value) {
	typename t_vec_it< T >::t	it = range.first, ite = range.second;
	while (it != ite && *it != value)
		it++;
	return (it != ite);
}

#endif