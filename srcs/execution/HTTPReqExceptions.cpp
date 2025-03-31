/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPReqExceptions.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: locharve <locharve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 14:10:34 by locharve          #+#    #+#             */
/*   Updated: 2025/03/31 14:10:35 by locharve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPRequest.hpp"

const char*	HttpRequest::OpenFileException::what() const throw() {
	return ("Error to opening the file");
}

const char*	HttpRequest::MalformedHttpHeader::what() const throw() {
	return ("Malformed http header");
}