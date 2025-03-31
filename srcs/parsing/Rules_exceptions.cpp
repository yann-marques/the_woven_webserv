/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Rules_exceptions.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: locharve <locharve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 14:12:14 by locharve          #+#    #+#             */
/*   Updated: 2025/03/31 14:12:14 by locharve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Rules.hpp"

Rules::RedefinedArgException::RedefinedArgException(std::string where): StrException("Argument redefined in location: " + where) {}

Rules::RedefinedArgException::~RedefinedArgException() throw() {}

const char*	Rules::RedefinedArgException::what() const throw() {
	return (_str.c_str());
}

Rules::InvalidLocationKeyException::InvalidLocationKeyException(std::string where): StrException("Invalid location key: " + where + " (must be formated as: \"/<alnum>\")") {}

Rules::InvalidLocationKeyException::~InvalidLocationKeyException() throw() {}

const char*	Rules::InvalidLocationKeyException::what() const throw() {
	return (_str.c_str());
}