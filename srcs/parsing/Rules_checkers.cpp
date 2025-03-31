/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Rules_checkers.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: locharve <locharve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 14:12:09 by locharve          #+#    #+#             */
/*   Updated: 2025/03/31 14:12:09 by locharve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Rules.hpp"

void	Rules::checkArgsFormat(const std::multimap< std::string, std::string >& args) const {
	noEmptyStrValues(args);

	checkArgNoDouble(args.equal_range("auto_index"), isValidAutoIndex);
	checkArgNoDouble(args.equal_range("max_body_bytes"), isValidMaxBodyBytes);
	checkArgNoDouble(args.equal_range("root"), noForbiddenChar);
	checkArgNoDouble(args.equal_range("redirect"), noForbiddenChar);
	checkArgNoDouble(args.equal_range("upload"), noForbiddenChar);

	checkRoot(args.equal_range("root"));

	checkAllowedMethods(args.equal_range("allowed_methods"));
	checkErrorPages(args.equal_range("error_pages"));
	checkCgiPath(args.equal_range("cgi_path"));
}