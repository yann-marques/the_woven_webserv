/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StrException.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: locharve <locharve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 14:10:12 by locharve          #+#    #+#             */
/*   Updated: 2025/03/31 14:10:13 by locharve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STREXCEPTION_HPP
# define STREXCEPTION_HPP

# include <exception>
# include <string>

class	StrException: public std::exception {
	protected:
		std::string	_str;
	public:
		StrException();
		StrException(std::string str);
		StrException(const StrException& rhs);
		StrException&	operator=(const StrException& rhs);

		virtual ~StrException() throw();
};

#endif