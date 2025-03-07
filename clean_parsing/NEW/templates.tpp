#ifndef TEMPLATES_TPP
# define TEMPLATES_TPP

template< typename Key, typename Value >
struct	t_mm_it {
	typedef typename std::multimap< Key, Value >::iterator	t;
};
// std::multimap< int, std::string >::iterator	mmIt;
// t_mm_it< int, std::string >::t	mmIt;

template< typename Key, typename Value >
struct	t_mm_range {
	typedef typename std::pair< typename t_mm_it< Key, Value >::t, typename t_mm_it< Key, Value >::t >	t;
};
// std::pair< std::multimap< int, std::string >::iterator, std::multimap< int, std::string >::iterator >	mmRange;
//	t_mm_range< int, std::string >::t	mmRange;

template< typename Key >
struct	t_set_it {
	typedef typename std::set< Key >::iterator	t;
};
// std::set< int >::iterator	setIt;
// t_set_it< int >::t	setIt;

#endif