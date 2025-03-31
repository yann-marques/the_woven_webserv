/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StrException.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: locharve <locharve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 14:12:30 by locharve          #+#    #+#             */
/*   Updated: 2025/03/31 14:12:31 by locharve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "StrException.hpp"

StrException::StrException() {}

StrException::StrException(std::string str): _str(str) {}

StrException::StrException(const StrException& rhs) {
	*this = rhs;
}

StrException&	StrException::operator=(const StrException& rhs) {
	_str = rhs._str;
	return (*this);
}

StrException::~StrException() throw() {}