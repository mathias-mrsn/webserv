/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   checking.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamaurai <mamaurai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/08 07:53:38 by mamaurai          #+#    #+#             */
/*   Updated: 2022/09/23 18:02:01 by mamaurai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

# define MAX_PORT_	65535
# define MAX_ERROR_CODE_ 600

inline static bool
size_one_ (const string_vector & v) {
	return (v.size() == 1);
}

inline static bool
size_two_ (const string_vector & v) {
	return (v.size() == 2);
}

inline static bool
size_correct_path_ (const string_vector & v) {
	struct stat buffer;
	
  	return (stat (((v.size() == 1) ? v[0] : v[1]).c_str(), &buffer) == 0);
}

inline static bool
methods_(const string_vector & v) {
	for (string_vector::const_iterator it = v.begin(); it != v.end(); it++) {
		if (*it != "GET" && *it != "POST" && *it != "DELETE")
			return (false);
	}
	return (true);
}

inline static bool
switch_on_ (const string_vector & v) {
	if (v[0] == "on" || v[0] == "off")
		return (true);
	return (false);
}

inline static bool
extention_ (const string_vector &v) {
	if (v[0].find_last_of(".") == 0)
		return (true);
	return (false);
}

inline static bool
file_is_dir_ (const string_vector & v) {
	struct stat buffer;
	
  	return (stat (((v.size() == 1) ? v[0] : v[1]).c_str(), &buffer) == 0 && buffer.st_mode & S_IFDIR);
}

inline static bool
file_is_reg_ (const string_vector & v) {
	struct stat buffer;
	
  	return (stat (((v.size() == 1) ? v[0] : v[1]).c_str(), &buffer) == 0 && buffer.st_mode & S_IFREG);
}

static bool
port_ (const string_vector & v) {
	uint64_t ret;
	struct sockaddr_in adr_inet;
	
	if ((ret = v[0].find(":")) == std::string::npos) {
		return (v[0].length() < 6 
				&& v[0].find_first_not_of("0123456789") == std::string::npos
				&& std::atoll((v[0].c_str())) < MAX_PORT_);
	}
	std::string ip = v[0].substr(0, ret);
	std::string port = v[0].substr(ret + 1, v[0].length());

	if (ip.empty() || port.empty()) {
		return (false);
	}
	adr_inet.sin_addr.s_addr =  inet_addr(ip.c_str());
 	if ( adr_inet.sin_addr.s_addr == INADDR_NONE  || std::count(ip.begin(), ip.end(), '.') != 3) {
		return (false);
	}
	return (port.length() < 6 
			&& port.find_first_not_of("0123456789") == std::string::npos
			&& std::atoll((port.c_str())) < MAX_PORT_);
}

inline static bool
max_b_s_ (const string_vector & v) {
	if (v[0].find_first_not_of("0123456789") == std::string::npos && v[0].length() < 20 && std::atoll(v[0].c_str()) < LLONG_MAX) {
		return (true);
	} else if (v[0].find_first_not_of("0123456789") == v[0].length() - 1
			&& v[0].length() < 12
			&& std::atoll(v[0].substr(0, v[0].length() - 1).c_str()) < INT_MAX
			&& (v[0][v[0].length() - 1] == 'K' || v[0][v[0].length() - 1] == 'k' || v[0][v[0].length() - 1] == 'M' || v[0][v[0].length() - 1] == 'm')) {
		return (true);
	} else {
		return (false);
	}
}

inline static bool
error_val_ (const string_vector & v) {
	if (v[0].find_first_not_of("0123456789") == std::string::npos && v[0].length() < 4 && std::atoi(v[0].c_str()) < MAX_ERROR_CODE_) {
		return (true);
	} else {
		return (false);
	}
}

inline static bool
default_ (const string_vector & v) {
	if (v.size() == 1)
		return (true);
	if (v.size() == 2)
		return (v[1] == "default_server");
	return (false);
}

bool
checker(const string_vector & v, std::size_t flags) {
	t_check	pairs[] = {	{&size_one_, CHECK_SIZE_ONE},
						{&size_two_, CHECK_SIZE_TWO},
						{&switch_on_, CHECK_SWITCH_ON},
						{&methods_, CHECK_METHODS},
						{&extention_, CHECK_EXTENTION},
						{&size_correct_path_, CHECK_CORRECT_PATH},
						{&file_is_dir_, CHECK_IS_DIR},
						{&port_, CHECK_PORT},
						{&max_b_s_, CHECK_MBS},
						{&error_val_, CHECK_ERROR_CODE},
						{&default_, CHECK_DEFAULT},
						{&file_is_reg_, CHECK_IS_REG},
						{NULL, 0} };

	for (int i = 0; pairs[i].flag != 0; i++) {
		if (flags & pairs[i].flag) {
			if (!pairs[i].check(v))
				return (false);
		}
	}
	return (true);
}
