/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamaurai <mamaurai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/27 15:55:35 by mamaurai          #+#    #+#             */
/*   Updated: 2022/09/27 13:57:18 by mamaurai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include "webserv.hpp"

_BEGIN_NAMESPACE_WEBSERV

class Server {

	public:
		typedef std::vector<std::pair<int, std::string> > 			error_page_type;
		typedef std::vector<Location*>								location_type;
	

	private:
		std::vector<int> 							_port;
		std::string 								_ip;
		size_t 										_max_body_size;
		bool										_default;
		std::string									_server_name;
		error_page_type								_error_pages;
		location_type								_locations;
		

	public:
	
		Server (void) :
			_port(),
			_ip(""),
			_max_body_size(8000000),
			_default(false),
			_server_name("localhost"),
			_error_pages(),
			_locations()
		{ }

		virtual ~Server (void) {
			for (location_type::iterator it = _locations.begin(); it != _locations.end(); ++it) {
				delete *it;
			}
			_locations.clear();
			_error_pages.clear();
		}

		Server (const Server& ref) {
			*this = ref;
		}

		std::vector<int>	&				get_port (void)					{return (_port);}
		std::string	&						get_ip (void)        			{return (_ip);}
		size_t 								get_max_body_size (void) const 	{return (_max_body_size);}
		std::string 						get_server_name (void) const 	{return (_server_name);}
		error_page_type 					get_error_pages (void) const 	{return (_error_pages);}
		location_type 						get_locations (void) const 		{return (_locations);}
		bool								get_default (void) const 		{return (_default);}
		void								create_server (string_vector::const_iterator &);
        std::string         				return_path_matching (int);

	private:

		void								_set_port (string_vector::const_iterator &);
		void								_set_max_body_size (string_vector::const_iterator &);
		void								_set_server_name (string_vector::const_iterator &);
		void								_set_error_page (string_vector::const_iterator &);
		void								_set_location (string_vector::const_iterator &);
		


	public:
		
		Server&	operator= (const Server& ref) {
			if (this == &ref) {return (*this);}
			
			_port = ref._port;
			_ip = ref._ip;
			_max_body_size = ref._max_body_size;
			_default = ref._default;
			_server_name = ref._server_name;
			_error_pages = ref._error_pages;
			_locations = ref._locations;
			return (*this);
		}

		friend std::ostream & operator<< (std::ostream & o, const Server & s) {
			CNO("--> Server :", o);
			for (std::vector<int>::const_iterator it = s._port.begin(); it != s._port.end(); it++) {
				CNO("ports -> " << *it, o);
			}
			CNO("ip -> " << s._ip, o);
			CNO("max_body_size -> " << s._max_body_size, o);
			CNO("default -> " << s._default, o);
			CNO("server_name -> " << s._server_name, o);
			CNO("error pages :", o);
			for (error_page_type::const_iterator it = s._error_pages.begin(); it != s._error_pages.end(); it++) {
				CNO(it->first << " -- " << it->second, o);
			}
			CNO("location :", o);
			for (location_type::const_iterator it = s._locations.begin(); it != s._locations.end(); it++) {
				CO(*(*it), o);
			}
			return (o);
		}		
};

typedef struct s_function_pair_server {
	void (Server::*f) (string_vector::const_iterator &);
	std::string	str;
}	t_function_pair_server;


_END_NAMESPACE_WEBSERV

#endif /* SERVER_HPP */