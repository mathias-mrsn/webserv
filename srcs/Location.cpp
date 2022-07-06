/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamaurai <mamaurai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/05 13:18:15 by mamaurai          #+#    #+#             */
/*   Updated: 2022/07/06 16:01:38 by mamaurai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

void
INLINE_NAMESPACE::Location::_set_methods (string_vector::const_iterator & it) {
	string_vector v = get_until_semicolon(it);

	// ! CONDITION

	for (string_vector::const_iterator vit = v.begin(); vit != v.end(); vit++) {
		COUT(*vit)
		if (*vit == "GET") {
			_methods |= M_GET;
		} else if (*vit == "POST") {
			_methods |= M_POST;
		} else if (*vit == "DELETE") {
			_methods |= M_DELETE;
		} else {
			throw Configuration::InvalidMethod();
		}
	}
}

void
INLINE_NAMESPACE::Location::_set_root (string_vector::const_iterator & it) {
	string_vector v = get_until_semicolon(it);

	// ! CONDITION

	if (v.size() != 1) {
		throw Configuration::InvalidRoot();
	}
	_root = v[0];
}

void
INLINE_NAMESPACE::Location::_set_index (string_vector::const_iterator & it) {
	string_vector v = get_until_semicolon(it);

	// ! CONDITION

	if (v.size() != 1) {
		throw Configuration::InvalidIndex();
	}
	_index = v[0];
}

void
INLINE_NAMESPACE::Location::_set_auto_index(string_vector::const_iterator & it) {
	string_vector v = get_until_semicolon(it);

	// ! CONDITION

	if (v.size() != 1) {
		throw Configuration::InvalidAutoIndex();
	}
	
	if (v[0] == "on") {
		_autoindex = true;
	} else if (v[0] == "off") {
		_autoindex = false;
	} else {
		throw Configuration::InvalidAutoIndex();
	}
}

void
INLINE_NAMESPACE::Location::_set_upload (string_vector::const_iterator & it) {
	string_vector v = get_until_semicolon(it);

	// ! CONDITION

	if (v.size() != 1) {
		throw Configuration::InvalidUpload();
	}
	_upload_path = v[0];
}

void
INLINE_NAMESPACE::Location::_set_cgi (string_vector::const_iterator & it) {
	string_vector v = get_until_semicolon(it);

	// ! CONDITION
	
	if (v.size() != 2) {
		throw Configuration::InvalidCgi();
	}
	_cgi = std::make_pair(v[0], v[1]);
}

void
INLINE_NAMESPACE::Location::_set_return (string_vector::const_iterator & it) {
	string_vector v = get_until_semicolon(it);

	// ! CONDITION

	if (v.size() != 2) {
		throw Configuration::InvalidReturn();
	}
	_return.push_back(std::make_pair(std::stoi(v[0]), v[1]));
}

void
INLINE_NAMESPACE::Location::create_location (string_vector::const_iterator & it) {
	int idx;
	const t_function_pair_location	pairs[] = {{&Location::_set_methods, "allow_methods"},
												{&Location::_set_root, "root"},
												{&Location::_set_index, "index"},
												{&Location::_set_auto_index,"auto_index"},
												{&Location::_set_upload, "upload"},
												{&Location::_set_cgi, "cgi"},
												{&Location::_set_return, "return"},
												{NULL, ""}};
	
	while (it != LEXER.end() && *it != "}") {
		for (idx = 0; pairs[idx].f != NULL; idx++) {
			if (*it == pairs[idx].str) {
				++it;
				(this->*pairs[idx].f)(it);
				break;
			}
		}
		if (pairs[idx].str == "") {
			// COUT(*it)
			DEBUG_2(COUT(*this))
			throw Server::InvalidKeyword();
		}
		if (it != LEXER.end())
			it++;
	}
	if (it == LEXER.end()) {
		throw Configuration::SyntaxError();
	}
}