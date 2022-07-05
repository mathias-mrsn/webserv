/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamaurai <mamaurai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/05 13:18:15 by mamaurai          #+#    #+#             */
/*   Updated: 2022/07/05 14:13:42 by mamaurai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

void
INLINE_NAMESPACE::s_location::_set_methods (std::vector<std::string> &v) {
	if (v.size() < 2)
		throw s_location::InvalidLocationBlock();
	for (std::vector<std::string>::iterator it = v.begin() + 1; it != v.end(); it++) {
		if (*it == "GET") {
			_methods |= M_GET;
		} else if (*it == "POST") {
			_methods |= M_POST;
		} else if (*it == "DELETE") {
			_methods |= M_DELETE;
		} else {
			throw s_location::InvalidLocationBlock();
		}
	}
}

void
INLINE_NAMESPACE::s_location::_set_path (std::vector<std::string> &v) {
	if (v.size() != 2 || v[1].length() > CONFIG_STRING_MAX_SIZE)
		throw s_location::InvalidLocationBlock();
	_path = v[1];
}

// void
// INLINE_NAMESPACE::s_location::return (std::vector<std::string> &v) {
	
// }	