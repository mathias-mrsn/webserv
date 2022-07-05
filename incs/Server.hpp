/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamaurai <mamaurai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/27 15:55:35 by mamaurai          #+#    #+#             */
/*   Updated: 2022/07/05 13:37:00 by mamaurai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include "webserv.hpp"

_BEGIN_NAMESPACE_WEBSERV

class Server {

	typedef typename std::vector<std::pair<int, std::string> > 	error_page_type;
	typedef typename std::vector<t_location>					location_type;

	private:
		int 										_port;
		size_t 										_max_body_size;
		std::string									_server_name;
		error_page_type								_error_pages;
		location_type								_locations;
		

	public:
		Server (void) :
			_port(0),
			_max_body_size(0),
			_server_name(""),
			_error_pages(),
			_locations()
		{ }
		
		~Server (void)
		{ }
		
		Server (const Server& ref) {
			*this = ref;
		}

		Server&	operator= (const Server& ref) {
			if (this == &ref) {return (*this);}
			
			_port = ref._port;
			_max_body_size = ref._max_body_size;
			_server_name = ref._server_name;
			_error_pages = ref._error_pages;
			_locations = ref._locations;
			return (*this);
		}

		void	_parse_location (std::ifstream &, std::vector<std::string> &);

		void	_set_server_name (std::vector<std::string> &);
		void	_set_listen (std::vector<std::string> &);
		void	_set_error_page (std::vector<std::string> &);
		void	_set_client_max_body_size (std::vector<std::string> &);

		public:
			friend std::ostream & operator<< (std::ostream & o, const Server & s) {
				CO("server :", o);
				CO("port -> " << s._port, o);
				CO("max_body_size -> " << s._max_body_size, o);
				CO("server_name -> " << s._server_name, o);
				CO("error pages :", o);
				for (error_page_type::const_iterator it = s._error_pages.begin(); it != s._error_pages.end(); it++) {
					CO(it->first << " -- " << it->second, o);
				}
				CO("location :", o);
				for (location_type::const_iterator it = s._locations.begin(); it != s._locations.end(); it++) {
					CO(*it, o);
				}
				return (o);
			}

		EXCEPTION(InvalidServerName, "configuration_file : invalid server_name field")
		EXCEPTION(InvalidPort, "configuration_file : invalid listen field")
		EXCEPTION(InvalidServerBlock, "configuration_file : syntax error in server block")
		EXCEPTION(InvalidErrorPage, "configuration_file : invalid error_page field")
		EXCEPTION(CannotOpenErrorPage, "configuration_file : cannot open error_page")
		EXCEPTION(InvalidBodySize, "configuration_file : invalid client_max_body_size field")
};

typedef struct s_function_pair_server {
	void (Server::*f) (std::vector<std::string> &);
	std::string	str;
}	t_function_pair_server;


_END_NAMESPACE_WEBSERV

#endif /* SERVER_HPP */