/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamaurai <mamaurai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/01 16:03:56 by mamaurai          #+#    #+#             */
/*   Updated: 2022/09/23 18:02:17 by mamaurai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP

#include "webserv.hpp"

_BEGIN_NAMESPACE_WEBSERV

class Server;

class Location
{
	public:
		typedef std::vector<std::pair<int, std::string> > 	return_type;
		typedef std::vector<std::pair<std::string, std::string> > 	cgi_type;

	private:
		uint8_t			_methods;
		return_type& 	_return;
		std::string		_root;
		std::string 	_index;
		std::string 	_path;
		bool			_autoindex;
		cgi_type		_cgi;
		std::string 	_upload_path;
        Server  *       _server;


	public:
		Location (void) :
			_methods(0),
			_return(*new return_type()),
			_root(""),
			_index(""),
			_path(""),
			_autoindex(false),
			_cgi(),
			_upload_path(),
            _server(NULL)
		{ }

		Location (const std::string s) :
			_methods(0),
			_return(*new return_type()),
			_root(""),
			_index(""),
			_path(""),
			_autoindex(false),
			_cgi(),
			_upload_path(),
            _server(NULL)
		{
            _path = remove_slash(s);
        }

		virtual ~Location (void) {
			delete &_return;
		}

		Location& operator= (const Location & ref) {
			if (this == &ref) {return (*this);}
			
			_methods = ref._methods;
			_return = ref._return;
			_root = ref._root;
			_index = ref._index;
			_path = ref._path;
			_autoindex = ref._autoindex;
			_cgi = ref._cgi;
			_upload_path = ref._upload_path;
            _server = ref._server;
			return (*this);
		}

		uint8_t			get_methods (void) const 		{return (_methods);}
		return_type&	get_return (void) const			{return (_return);}
		std::string		get_root (void) const 			{return (_root);}
		std::string		get_index (void) const 			{return (_index);}
		std::string		get_path (void) const 			{return (_path);}
		bool			get_autoindex (void) const 		{return (_autoindex);}
		cgi_type		get_cgi (void) const		 	{return (_cgi);}
		std::string		get_upload_path (void) const 	{return (_upload_path);}
        Server *        get_server (void) const         {return (_server);}
		
		void			create_location (string_vector::const_iterator &);
        std::string     return_path_matching (int);

        void            set_server (Server * s) {_server = s;}
		void			set_path (std::string const str) {_path = str;}

	private:

		void			_set_methods (string_vector::const_iterator &);
		void			_set_root (string_vector::const_iterator & it);
		void			_set_index (string_vector::const_iterator & it);
		void			_set_auto_index (string_vector::const_iterator & it);
		void			_set_upload (string_vector::const_iterator & it);
		void			_set_cgi (string_vector::const_iterator & it);
		void			_set_return (string_vector::const_iterator & it);
		
	
	public:
		friend std::ostream & operator<< (std::ostream & o, const Location & l) {
			CNO("location of webserv", o);
			CNO("debug_level = " << g_debug_prog_level, o);
			CNO("methods = " << l._methods, o);
			CNO("root = " << l._root, o);
			CNO("index = " << l._index, o);
			CNO("path = " << l._path, o);
			CNO("autoindex = " << l._autoindex, o);
			CNO("cgi = ", o);
			for (cgi_type::const_iterator it = l._cgi.begin(), end = l._cgi.end(); it != end; ++it) {
				CO(it->first, o);
				CNO(" " << it->second, o);
			}
			CNO("upload_path = " << l._upload_path, o);
			CNO("server = " << l._server, o);
			return (o);
		}
	
};

typedef struct s_function_pair_location {
	void (Location::*f) (string_vector::const_iterator &);
	std::string	str;
}	t_function_pair_location;

_END_NAMESPACE_WEBSERV

#endif /* LOCATION_HPP */