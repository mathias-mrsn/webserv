/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamaurai <mamaurai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/27 15:55:35 by mamaurai          #+#    #+#             */
/*   Updated: 2022/07/06 15:16:59 by mamaurai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include "webserv.hpp"

_BEGIN_NAMESPACE_WEBSERV

class Server {

	typedef typename std::vector<std::pair<int, std::string> > 	error_page_type;
	typedef typename std::vector<Location>					location_type;

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
		
		virtual ~Server (void)
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

		void	create_server (string_vector::const_iterator &);

	private:
		void	_set_port (string_vector::const_iterator &);
		void	_set_max_body_size (string_vector::const_iterator &);
		void	_set_server_name (string_vector::const_iterator &);
		void	_set_error_page (string_vector::const_iterator &);
		void	_set_location (string_vector::const_iterator &);

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

		
		EXCEPTION(InvalidServerBlock, "configuration_file : syntax error in server block")
		EXCEPTION(CannotOpenErrorPage, "configuration_file : cannot open error_page")
		EXCEPTION(InvalidKeyword, "configuration_file : invalid keywood")
		
};

typedef struct s_function_pair_server {
	void (Server::*f) (string_vector::const_iterator &);
	std::string	str;
}	t_function_pair_server;


_END_NAMESPACE_WEBSERV

#endif /* SERVER_HPP */