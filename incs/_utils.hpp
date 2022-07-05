/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   _utils.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamaurai <mamaurai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/27 15:33:58 by mamaurai          #+#    #+#             */
/*   Updated: 2022/07/05 14:09:49 by mamaurai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _UTILS_HPP
# define _UTILS_HPP

# define INLINE_NAMESPACE				webserv
#define _BEGIN_NAMESPACE_WEBSERV		namespace INLINE_NAMESPACE {
#define _END_NAMESPACE_WEBSERV 	 		}

# define DEBUG_OPTION					"--debug="
# define DEBUG_LEVEL_MAX					5
# define DEBUG_LEVEL_MIN					0
# define INVALID_ARG					"invalid argument number\n./webserv [--debug=`debug_value`] [configuration_file]"
# define FILE_MISSING					"configuration file missing\n./webserv [--debug=`debug_value`] [configuration_file]"

# define SYSCALL_ERR					(-1)
# define COUT(x)						std::cout << x << std::endl;
# define CO(x, o)						o << x << std::endl;
# define CERR(x)						std::cerr << x << std::endl;

# define CONFIG_STRING_MAX_SIZE			100

# define EXCEPTION(x, y)				public: class x : public std::exception {const char * what (void) const throw() {return (y);}};

# define DEBUG_1(x)						if (g_debug_prog_level >= 1) { x }
# define DEBUG_2(x)						if (g_debug_prog_level >= 2) { x }
# define DEBUG_3(x)						if (g_debug_prog_level >= 3) { x }
# define DEBUG_4(x)						if (g_debug_prog_level >= 4) { x }
# define DEBUG_5(x)						if (g_debug_prog_level >= 5) { x }

enum {
	M_GET = (1 << 0),
	M_POST = (1 << 1),
	M_DELETE = (1 << 2)
};	

std::vector<std::string>	vector_spliter (std::string str);
bool						str_is_print(std::string str);

#endif /* _UTILS_HPP */