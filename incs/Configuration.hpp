/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configuration.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamaurai <mamaurai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/27 15:32:45 by mamaurai          #+#    #+#             */
/*   Updated: 2022/07/06 15:55:21 by mamaurai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGURATION_FILE_HPP
# define CONFIGURATION_FILE_HPP

#include "webserv.hpp"

_BEGIN_NAMESPACE_WEBSERV

class Configuration {

	typedef typename std::vector<Server> server_vector;
	typedef typename std::vector<std::string> lexer_type;

	private :
		server_vector&	_servers;
		lexer_type&		_lexer;

	public:
		Configuration (void) :
			_servers(*new server_vector()),
			_lexer(*new lexer_type())
		{ }
		
		virtual ~Configuration (void) {
			delete &_servers;
			delete &_lexer;
		}

	private:
		Configuration (const Configuration & ref) :
			_servers(ref._servers),
			_lexer(ref._lexer)
		{ }
		
		// Configuration &	operator= (const Configuration & ref) {
		// 	if (this ==  &ref) {return (*this);}

		// 	return (*this);
		// }

		

	public:

		server_vector& 	getServers (void) const {return (_servers);}
		lexer_type& 		getLexer (void) const {return (_lexer);}

		void	lexer (std::string);
		void	parser (void);

	public:
		friend std::ostream & operator<< (std::ostream & o, const Configuration & c ) { 
        	CO("configuration of webserv", o);
			CO("debug_level = " << g_debug_prog_level, o);
			for (server_vector::const_iterator it = c._servers.begin(); it != c._servers.end(); it++) {
				CO(*it, o);
			}
        	return (o);
    	}

	EXCEPTION(FileCannotBeOpened, "configuration_file can't be opened, please check if the file exist and your allowed to open it")
	EXCEPTION(InvalidFile, "configuration_file : invalid file")
	EXCEPTION(SyntaxError, "configuration_file : syntax error")

	EXCEPTION(InvalidPort, "configuration_file : invalid port")
	EXCEPTION(InvalidBodySizeMax, "configuration_file : invalid body size max")
	EXCEPTION(InvalidServerName, "configuration_file : invalid server name")
	EXCEPTION(InvalidErrorPage, "configuration_file : invalid error page")
	EXCEPTION(InvalidLocation, "configuration_file : invalid location")

	EXCEPTION(InvalidMethod, "configuration_file : invalid method field")
	EXCEPTION(InvalidRoot, "configuration_file : invalid root field")
	EXCEPTION(InvalidIndex, "configuration_file : invalid index field")
	EXCEPTION(InvalidAutoIndex, "configuration_file : invalid auto index filed")
	EXCEPTION(InvalidUpload, "configuration_file : invalid upload path")
	EXCEPTION(InvalidReturn, "configuration_file : invalid return field")
	EXCEPTION(InvalidCgi, "configuration_file : invalid cgi field")
};

typedef struct s_function_pair_config {
	void (Configuration::*f) (std::ifstream  &);
	std::string	str;
}	t_function_pair_config;

_END_NAMESPACE_WEBSERV

#endif /* CONFIGURATION_FILE_HPP */
