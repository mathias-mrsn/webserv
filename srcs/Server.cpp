/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamaurai <mamaurai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/27 17:09:05 by mamaurai          #+#    #+#             */
/*   Updated: 2022/07/06 16:00:47 by mamaurai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

# define MAX_BODY_SIZE_ 1000000
# define MAX_PORT_ 65535
# define MAX_ERROR_CODE_

/*
	TODO add get_until_semicolon in _utils.cpp
*/


string_vector
get_until_semicolon (string_vector::const_iterator & it) {
	string_vector ret;


	for (; it != LEXER.end(); it++) {
		if ((*it).find(";") == (*it).length() - 1) {
			ret.push_back((*it).substr(0, (*it).length() -1 ));
			break;
		} else {
			ret.push_back(*it);
		}
	}
	// PUT_VECTOR(ret)
	return ret;
}

void
INLINE_NAMESPACE::Server::_set_port (string_vector::const_iterator & it) {
	string_vector v = get_until_semicolon(it);

	// ! CONDITION

	if (v.size() != 1) {
		throw Configuration::InvalidPort();
	}

	_port = std::stoll(v[0]);
}

void
INLINE_NAMESPACE::Server::_set_max_body_size (string_vector::const_iterator & it) {
	string_vector v = get_until_semicolon(it);

	// ! CONDITION

	if (v.size() != 1) {
		throw Configuration::InvalidBodySizeMax();
	}

	_max_body_size = std::stoll(v[0]);

}

void
INLINE_NAMESPACE::Server::_set_server_name (string_vector::const_iterator & it) {
	string_vector v = get_until_semicolon(it);

	if (v.size() != 1) {
		throw Configuration::InvalidServerName();
	}

	_server_name = v[0];

	// ! CONDITION
}

void
INLINE_NAMESPACE::Server::_set_error_page (string_vector::const_iterator & it) {
	string_vector v = get_until_semicolon(it);

	// ! CONDITION

	if (v.size() != 2) {
		throw Configuration::InvalidErrorPage();
	}

	_error_pages.push_back(std::make_pair(std::stoi(v[0]), v[1]));
}

void
INLINE_NAMESPACE::Server::_set_location (string_vector::const_iterator & it) {

	// TODO Do better than this

	if (it != LEXER.end() && (it + 1) != LEXER.end() && (*it) != "{" && (*(it + 1)) == "{") {
		Location loc(*it);
		it += 2;
		loc.create_location(it);
	} else {
		throw Configuration::InvalidLocation();
	}
}

void
INLINE_NAMESPACE::Server::create_server (string_vector::const_iterator & it) {
	int idx;
	const t_function_pair_server	pairs[] = {	{&Server::_set_port, "listen"},
										{&Server::_set_max_body_size, "client_max_body_size"},
										{&Server::_set_server_name, "server_name"},
										{&Server::_set_error_page, "error_page"},
										{&Server::_set_location, "location"},
										{NULL, ""} };
	
	while (it != LEXER.end() && *it != "}") {
		for (idx = 0; pairs[idx].f != NULL; idx++) {
			if (*it == pairs[idx].str) {
				++it;
				(this->*pairs[idx].f)(it);
				break;
			}
		}
		if (pairs[idx].str == "") {
			COUT(*it)
			DEBUG_2(COUT(*this))
			throw Server::InvalidKeyword();
		}
		if (it != LEXER.end())
			it++;
	}
	if (it == LEXER.end()) {
		throw Configuration::SyntaxError();
	}

	DEBUG_2(COUT(*this))
}