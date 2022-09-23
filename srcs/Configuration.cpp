/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configuration.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamaurai <mamaurai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/27 15:38:48 by mamaurai          #+#    #+#             */
/*   Updated: 2022/09/23 18:01:51 by mamaurai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

bool
INLINE_NAMESPACE::Configuration::_duplicate_server (std::vector<Server *> & server) {
	std::vector<Server *>::const_iterator current = server.end() - 1;
	FOREACH_SERVER {
		if (current == it) {
			break;
		} else {
			if ((*it)->get_server_name() == (*current)->get_server_name()) {
				for (std::vector<int>::iterator it2 = (*it)->get_port().begin(); it2 != (*it)->get_port().end(); ++it2) {
					if (std::find((*current)->get_port().begin(), (*current)->get_port().end(), *it2) != (*current)->get_port().end()) {
						return (true);
					}
				}
			}
		}
	}
	return (false);
}

void
INLINE_NAMESPACE::Configuration::parser (void) {
	string_vector::const_iterator it = _lexer.begin();

    DEBUG_3(CNOUT(BBLU << "Updating : parsing configuration file..." << CRESET));
	while (it != _lexer.end()) {
		if (*it == "server" && ((it + 1) != _lexer.end() && (*(it + 1)) == "{")) {
			if (_servers.size() > MAX_SERVER) {
				throw Configuration::TooManyServer();
			}
			INLINE_NAMESPACE::Server* s = new Server();
			it += 2;
			_servers.push_back(s);
			s->create_server(it);
			if (_duplicate_server(_servers)) {
				throw Configuration::DuplicateServer();
			}
		} else {
			throw Configuration::SyntaxError();
		}
		if (it != _lexer.end())
			it++;
	}
}

void
INLINE_NAMESPACE::Configuration::lexer (std::string conf_file) {
	std::string 				buffer;
	std::ifstream				ifs;
	string_vector 				v;

	ifs.open(conf_file.c_str());
	if (ifs.fail()) {
		throw Configuration::FileCannotBeOpened();
	}
    DEBUG_3(CNOUT(BBLU << "Updating : loading configuration file..." << CRESET));
	while (!ifs.eof()) {
		std::getline(ifs, buffer);
		v = vector_spliter(buffer, "}{;", "\t", true);
		
		if (v.empty() || v[0][0] == '#') {
			continue;
		} else {
			_lexer.insert(_lexer.end(), v.begin(), v.end());
		}
	}
	if (_lexer.empty()) {
		throw Configuration::InvalidFile();
	}
	ifs.close();
}
