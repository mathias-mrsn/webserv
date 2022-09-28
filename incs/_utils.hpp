/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   _utils.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamaurai <mamaurai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/27 15:33:58 by mamaurai          #+#    #+#             */
/*   Updated: 2022/09/28 15:44:09 by mamaurai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _UTILS_HPP
# define _UTILS_HPP

class Server;

# define INLINE_NAMESPACE				webserv
# define _BEGIN_NAMESPACE_WEBSERV		namespace INLINE_NAMESPACE {
# define _END_NAMESPACE_WEBSERV 	 		}

# define DEBUG_OPTION					"--debug="
# define DEBUG_LEVEL_MAX					5
# define DEBUG_LEVEL_MIN					0
# define USE_HOSTNAME						false

# define INVALID_ARG					"invalid argument number\n./webserv [--debug=`debug_value`] [configuration_file]"
# define FILE_MISSING					"configuration file missing\n./webserv [--debug=`debug_value`] [configuration_file]"

#if defined(__APPLE__) && defined(__MACH__)
# define DEFAULT_FILE	 				"configuration_files/.default_mac.conf"
#else
# define DEFAULT_FILE	 				"configuration_files/.default_ubuntu.conf"
#endif

# define SYSCALL_ERR					(-1)
# define FATAL_ERROR                    (-1)
# define SUCCESS						0

# define COUT(x)						std::cout << x;
# define CO(x, o)						o << x;
# define CERR(x)						std::cerr << x;
# define CNOUT(x)						std::cout << x << std::endl;
# define CNO(x, o)						o << x << std::endl;
# define CNERR(x)						std::cerr << x << std::endl;
# define CCOUT(color, x)				std::cout << color << x << CRESET << std::endl;
# define CONFIG_STRING_MAX_SIZE			100
# define MAX_CLIENT						512
# define BUFFER_SIZE					1025
# define MAX_SERVER						20
# define WWW_PATH                       "./www/"

# define EXCEPTION(x, y)				public: class x : public std::exception {const char * what (void) const throw() {return (y);}};

# define DEBUG_ON 		 				true

#	define DEBUG_1(x)						if (g_debug_prog_level >= 1) { x } // Simple input and output of request and response
#	define DEBUG_2(x)						if (g_debug_prog_level >= 2) { x } // Print full request and full response
#	define DEBUG_3(x)						if (g_debug_prog_level >= 3) { x } // Print update of the server
#	define DEBUG_4(x)						if (g_debug_prog_level >= 4) { x } // Print server config + CGI output
#	define DEBUG_5(x)						if (g_debug_prog_level >= 5) { x } // Print errors

# define LEXER							g_config.getLexer()
# define SERVERS						g_config.getServers()
# define HEADER							g_header

# define FOREACH_SERVER					for (std::vector<INLINE_NAMESPACE::Server*>::const_iterator it = SERVERS.begin(); it != SERVERS.end(); ++it)
# define FOREACH_HEADER 				for (string_vector::const_iterator it = HEADER.begin(); it != HEADER.end(); ++it)
# define FOREACH_COOKIE					for (std::map<std::string, std::string>::const_iterator it = _cookies.begin(); it != _cookies.end(); ++it)

# define COUNT_SERVERS					g_config.getServers().size()
# define CHECKER(v, x)					(true == checker(v, x))

# define ITOA( x ) static_cast< const std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

typedef std::vector<std::string> string_vector;

enum {
	M_GET = (1 << 0),
	M_POST = (1 << 1),
	M_DELETE = (1 << 2)
};

typedef struct s_check {
	bool	(*check)(const string_vector &);
	std::size_t	flag;
} t_check;

enum {
	CHECK_SIZE_ONE = (1 << 0),
	CHECK_SIZE_TWO = (1 << 1),
	CHECK_CORRECT_PATH = (1 << 2),
	CHECK_IS_DIR = (1 << 3),
	CHECK_METHODS = (1 << 6),
	CHECK_SWITCH_ON = (1 << 7),
	CHECK_EXTENTION = (1 << 8),
	CHECK_MBS = (1 << 10),
	CHECK_PORT = (1 << 11),
	CHECK_ERROR_CODE = (1 << 12),
	CHECK_DEFAULT = (1 << 13),
	CHECK_IS_REG = (1 << 14)
};

string_vector				vector_spliter (std::string str, const char *, const char *, bool);

bool						str_is_print(std::string str);

string_vector				get_until_semicolon (string_vector::const_iterator & it);

bool						checker(const string_vector & v, std::size_t flags);

bool						endsWith(std::string const &str, std::string const &suffix);

int							calculate_size_file (char *filename);

int							calculate_size_piece_file (int mb_size);

bool						is_header (const std::string & str);

inline bool					file_exist (const std::string & path) {
	struct stat buffer;   
	return (stat (path.c_str(), &buffer) == 0 && buffer.st_mode & S_IFREG); 
}

std::string					read_file(std::string path);

bool						path_is_dir (const std::string & str);

bool						path_is_valid (const std::string & str);

std::string					create_html_error_page (int error_code);

std::string					remove_slash (const std::string & str);

std::string					get_file_extension(const std::string & path);

string_vector               delimiter_spliter (std::string str);

void						quit_program (int);

#endif /* _UTILS_HPP */