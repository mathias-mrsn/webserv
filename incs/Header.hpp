/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Header.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamaurai <mamaurai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/23 18:02:12 by mamaurai          #+#    #+#             */
/*   Updated: 2022/09/23 18:02:13 by mamaurai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEADER_HPP
# define HEADER_HPP

# include "webserv.hpp"

_BEGIN_NAMESPACE_WEBSERV

class Header
{
	private:
		/* data */
        std::string _date;
		std::string	_content_type;
		std::string	_content_length;
		std::string	_cookie;
		std::string _status;
        std::string _server;
		int			_status_code;

	public: /* Constructors */

		Header (void):
			_date(""),
			_content_type("Content-Type: "),
			_content_length("Content-Length: "),
			_cookie(""),
			_status("\r\n"),
			_server(""),
			_status_code(0)
		{ }

		Header(const Response & response):
			_date(""),
			_content_type("Content-Type: "),
			_content_length("Content-Length: "),
			_cookie("Set-Cookie: "),
			_server(""),
			_status_code(response.get_error_value())
		{ }

		~Header (void)
		{ }

		Header (const Header & copy) {
			*this = copy;
		}

	public: /* Accessors */
		void		set_content_type(std::string type) 		{ _content_type = type; }
		void		set_status(std::string status) 			{ _status = status; }
		void		set_content_length(std::string length)	{ _content_length = length; }
		void		set_status_code(int value) 				{ _status_code = value; }
		void		set_cookie_value(std::string value) 	{ _cookie = value; }
		std::string	get_content_type(void) const 			{ return _content_type; }
		std::string get_status(void) const 					{ return _status; }
		std::string	get_content_length(void) const 			{ return _content_length; }
		std::string	get_cookie(void) const 			{ return _cookie; }
		int			get_status_code(void) const 			{ return _status_code; }

	public: /* Methods */
		void			fill (Response & response);
		std::string		append (void);

	public: /* Operators */
		Header & operator=(const Header & copy) {
            _date = copy._date;
			_content_type = copy._content_type;
			_content_length = copy._content_length;
			_status_code = copy._status_code;
			_cookie = copy._cookie;
            _status = copy._status;
            _server = copy._server;
			return *this;
		}

		friend std::ostream& operator<< (std::ostream& o, const Header & ref) {
			o << ref._content_type << std::endl;
			o << ref._content_length << std::endl;
			o << ref._status_code << std::endl;
			o << ref._cookie << std::endl;
            o << ref._status << std::endl;
            o << ref._server << std::endl;
			o << ref._date << std::endl;
			return o;
		}


};

_END_NAMESPACE_WEBSERV

#endif