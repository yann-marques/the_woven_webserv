#ifndef PARSER_HPP
# define PARSER_HPP

# include <iostream>
# include <fstream>
# include <sstream>
# include <cctype>
# include <map>
# include <vector>
# include <set>
# include <cstddef>
# include <string>
# include "templates.tpp"

class	Parser {
	protected:
		const	std::set< std::string >	_argsToFind;
	public:
		Parser();
		Parser(const Parser& rhs);
		Parser&	operator=(const Parser& rhs);

	//	void	setArgsToFind();
		size_t	endOfScopeIndex(std::string str, size_t pos);

//		virtual void	foo() = 0;

		template< typename T >
		void	printVec(const std::vector< T >& vec, std::string tabs);
		template< typename T, typename U >
		void	printMap(std::set< T > keys, const std::map< T, U >& map, std::string tabs);

		virtual	~Parser();

};

#endif