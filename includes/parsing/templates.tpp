/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   templates.tpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: locharve <locharve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 14:10:17 by locharve          #+#    #+#             */
/*   Updated: 2025/03/31 14:10:19 by locharve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
void	printMap(const std::map< Key, Value >& map) {
	typename t_map_it< Key, Value >::t	it = map.begin(), ite = map.end();
	while (it != ite) {
		std::cout << it->first << " " << it->second << std::endl;
		it++;
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