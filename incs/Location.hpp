/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamaurai <mamaurai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/01 16:03:56 by mamaurai          #+#    #+#             */
/*   Updated: 2022/07/06 15:57:50 by mamaurai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP

#include "webserv.hpp"

_BEGIN_NAMESPACE_WEBSERV

// TODO operator overload "<<"

class Location
{
	typedef typename std::vector<std::pair<int, std::string> > 	return_type;
	typedef typename std::pair<std::string, std::string>		cgi_type;

	private:
		uint8_t	_methods;
		return_type& _return;
		std::string	_root;
		std::string _index;
		std::string _path;
		bool	_autoindex;
		cgi_type	_cgi;
		std::string _upload_path;


	public:
		Location (void) :
			_methods(0),
			_return(*new return_type()),
			_root(""),
			_index(""),
			_path(""),
			_autoindex(false),
			_cgi(),
			_upload_path()
		{ }

		Location (const std::string s) :
			_methods(0),
			_return(*new return_type()),
			_root(""),
			_index(""),
			_path(s),
			_autoindex(false),
			_cgi(),
			_upload_path()
		{ }

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
			return (*this);
		}

		void	create_location (string_vector::const_iterator &);

	private:
		void	_set_methods (string_vector::const_iterator &);
		void	_set_root (string_vector::const_iterator & it);
		void	_set_index (string_vector::const_iterator & it);
		void	_set_auto_index (string_vector::const_iterator & it);
		void	_set_upload (string_vector::const_iterator & it);
		void	_set_cgi (string_vector::const_iterator & it);
		void	_set_return (string_vector::const_iterator & it);

		
	
	
	friend std::ostream & operator<< (std::ostream & o, const struct Location & l) {
		CO("location of webserv", o);
		CO("debug_level = " << g_debug_prog_level, o);
		CO("methods = " << l._methods, o);
		CO("root = " << l._root, o);
		CO("index = " << l._index, o);
		CO("path = " << l._path, o);
		CO("autoindex = " << l._autoindex, o);
		CO("cgi = " << l._cgi.first << " " << l._cgi.second, o);
		CO("upload_path = " << l._upload_path, o);
		return (o);
	}
	
};

typedef struct s_function_pair_location {
	void (Location::*f) (string_vector::const_iterator &);
	std::string	str;
}	t_function_pair_location;

_END_NAMESPACE_WEBSERV

#endif /* LOCATION_HPP */