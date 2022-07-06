/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configuration.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamaurai <mamaurai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/27 15:38:48 by mamaurai          #+#    #+#             */
/*   Updated: 2022/07/06 13:59:58 by mamaurai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

/*
	TODO add _set_server_name in server class
*/

// bool
// str_is_print (std::string str) {
// 	const char *s = str.c_str();
	
// 	for (size_t i = 0; i < str.length(); i++) {
// 		if (!isprint(s[i]))
// 			return (false);
// 	}
// 	return (true);
// }

void
remove_comment_ (string_vector & v) {
	string_vector::const_iterator it = v.begin();
	std::string replace;

	for (; it != v.end(); it++) {
		if ((*it).find("#") != std::string::npos) {
			int end = (*it).find("#");
			replace = (*it).substr(0, end);
			v.erase(it, v.end());
			if (!replace.empty())
				v.push_back(replace);
			return;
		}
	}
}

// bool
// is_last_elem (std::string str) {
// 	if (str.find(";") == str.length()) {
// 		return (true);
// 	} else {
// 		return (false);
// 	}
// }

string_vector
vector_spliter (std::string str) {
	string_vector vector;
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
	remove_comment_(vector);
	return (vector);
}

void
INLINE_NAMESPACE::Configuration::parser (void) {
	string_vector::const_iterator it = _lexer.begin();
	
	while (it != _lexer.end()) {
		if (*it == "server" && ((it + 1) != _lexer.end() && (*(it + 1)) == "{")) {
			INLINE_NAMESPACE::Server s;
			it += 2;
			s.create_server(it);
			_servers.push_back(s);
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
	string_vector 	v;

	ifs.open(conf_file);
	if (ifs.fail()) {
		throw Configuration::FileCannotBeOpened();
	}
	while (!ifs.eof()) {
		std::getline(ifs, buffer);
		v = vector_spliter(buffer);
		
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
	DEBUG_5(for (lexer_type::const_iterator it = _lexer.begin(); it != _lexer.end(); it++) {COUT(*it);})


}
