/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamaurai <mamaurai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/27 17:09:05 by mamaurai          #+#    #+#             */
/*   Updated: 2022/07/05 15:23:26 by mamaurai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

# define MAX_BODY_SIZE_ 1000000
# define MAX_PORT_ 65535
# define MAX_ERROR_CODE_

void
INLINE_NAMESPACE::Server::_set_server_name (std::vector<std::string> &v) {
	if (v.size() != 2
		|| v[1].length() > CONFIG_STRING_MAX_SIZE) {
		throw Server::InvalidServerName();
	} else {
		_server_name = v[1];
	}
}

void
INLINE_NAMESPACE::Server::_set_listen (std::vector<std::string> &v) {
	if (v.size() != 2
		|| v[1].length() > 5
		|| v[1].find_first_not_of("0123456789") != std::string::npos
		|| atoi(v[1].c_str()) > MAX_PORT_) {
		throw (Server::InvalidPort());
	} else {
		_port = atoi(v[1].c_str());
	}
}

void
INLINE_NAMESPACE::Server::_set_error_page (std::vector<std::string> &v) {
	if (v.size() != 3
		|| v[1].length() != 3
		|| v[1].find_first_not_of("0123456789") != std::string::npos
		|| atoi(v[1].c_str()) > 527
		|| v[2].length() > CONFIG_STRING_MAX_SIZE) {
		throw (Server::InvalidErrorPage());
	} else {
		std::ifstream	ifs;
		
		ifs.open(v[2]);
		if (ifs.fail()) {
			ifs.close();
			throw Server::CannotOpenErrorPage();
		}
		ifs.close();
		_error_pages.push_back(std::make_pair(atoi(v[1].c_str()), v[2]));
	}
}

void
INLINE_NAMESPACE::Server::_set_client_max_body_size (std::vector<std::string> &v) {
	if (v.size() != 2
		|| v[1].length() > 11
		|| v[1].find_first_not_of("0123456789") != std::string::npos
		|| atoll(v[1].c_str()) > MAX_BODY_SIZE_) {
		throw (Server::InvalidBodySize());
	} else {
		_max_body_size = atoll(v[1].c_str());
	}
}

void
INLINE_NAMESPACE::Server::_parse_location (std::ifstream &ifs, std::vector<std::string> &old_v) {
	std::string 				buffer;
	std::vector<std::string> 	v;
	int							idx;
	INLINE_NAMESPACE::t_location location;
	t_function_pair_location pairs[] = {{&s_location::_set_methods, "allow_methods"},
										{NULL, ""}};
	
	location._set_path(old_v);
	for (size_t i = 0; !ifs.eof(); i++) {
		std::getline(ifs, buffer);
		v = vector_spliter(buffer);
		
		if (i == 0 && (v.empty() || v[0] != "{"))
			throw Configuration::SyntaxError();		
		if (!v.empty() && v[0] == "}")
			break;
		for (idx = 0; !pairs[idx].str.empty(); idx++) {
			if (v.empty() || v[0][0] == '#' || v[0][0] == '{' || v[0][0] == '}') {
				break;
			} else if (v[0] == pairs[idx].str) {
				(location.*(pairs[idx].f))(v);
				break;
			}
		}
		if (pairs[idx].str.empty()) {
			COUT(v[0]); // ! TO DELETE
			throw (s_location::InvalidLocationBlock());
		}
	}
	if (v.empty() || v[0] != "}" || v.size() != 1)
		throw Configuration::SyntaxError();
	_locations.push_back(location);
	DEBUG_1(COUT(location));
}