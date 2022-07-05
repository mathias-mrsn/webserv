/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configuration.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamaurai <mamaurai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/27 15:38:48 by mamaurai          #+#    #+#             */
/*   Updated: 2022/07/05 14:12:16 by mamaurai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

/*
	TODO add _set_server_name in server class
*/

bool
str_is_print (std::string str) {
	const char *s = str.c_str();
	
	for (size_t i = 0; i < str.length(); i++) {
		if (!isprint(s[i]))
			return (false);
	}
	return (true);
}

std::vector<std::string>
vector_spliter (std::string str) {
	std::vector<std::string> vector;
	std::string stockage;

	std::replace( str.begin(), str.end(), '\t', ' '); 
    int start = 0U;
    int end = str.find(' ');
    while (end != std::string::npos)
    {
		stockage = str.substr(start, end - start);
		if (!stockage.empty())
			vector.push_back(stockage);
        start = end + 1;
        end = str.find(' ', start);
    }

    stockage = str.substr(start, end);
	if (!stockage.empty())
			vector.push_back(stockage);

	return (vector);
}

void
INLINE_NAMESPACE::Configuration::_parse_server (std::ifstream &ifs) {
	std::string 				buffer;
	std::vector<std::string> 	v;
	int							idx;
	INLINE_NAMESPACE::Server server;
	t_function_pair_server pairs[] = {	{&Server::_set_server_name, "server_name"},
										{&Server::_set_listen, "listen"},
										{&Server::_set_error_page, "error_page"},
										{&Server::_set_client_max_body_size, "client_max_body_size"},
										{NULL, ""}};
	

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
				(server.*(pairs[idx].f))(v);
				break;
			} else if (v[0] == "location") {
				server._parse_location(ifs, v);
			}
		}
		if (pairs[idx].str.empty()) {
			COUT(v[0]); // ! TO DELETE
			throw (Server::InvalidServerBlock());
		}
	}
	if (v.empty() || v[0] != "}" || v.size() != 1)
		throw Configuration::SyntaxError();
	_servers.push_back(server);
	DEBUG_1(COUT(server));
}

void
INLINE_NAMESPACE::Configuration::parse (std::string conf_file) {
	std::string 				buffer;
	std::ifstream				ifs;
	std::vector<std::string> 	v;
	// t_function_pair_config 		pairs[] = {	{&Configuration::_parse_server, "server"},
	// 										{&Configuration::_parse_location, "location"},
	// 										{NULL, ""}};
	
	ifs.open(conf_file);
	if (ifs.fail()) {
		throw Configuration::FileCannotBeOpened();
	}

	while (!ifs.eof()) {
		std::getline(ifs, buffer);
		v = vector_spliter(buffer);

		
		if (v.empty() || v[0][0] == '#') {
			continue;
		} else if (v[0] == "server") {
			_parse_server(ifs);
		}
		
	}
	ifs.close();
	DEBUG_1(COUT(*this));
}