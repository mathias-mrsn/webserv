/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamaurai <mamaurai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/23 18:02:22 by mamaurai          #+#    #+#             */
/*   Updated: 2022/09/23 18:02:23 by mamaurai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "webserv.hpp"

_BEGIN_NAMESPACE_WEBSERV

class Cgi;

class Response
{
	private: /* Attributes */

		int				_file_size;
		std::string		_body;
		Request	*		_request;
		std::string		_error_path;
		std::string		_message_send;
		int				_error_value;
		Server *		_server;
		Location *		_location;
        Cgi *           _cgi;

	public: /* Constructors */

		Response (void) :
			_file_size(0),
			_body(""),
			_request(NULL),
			_error_path(""),
			_message_send(""),
			_error_value(0),
			_server(NULL),
			_location(NULL),
            _cgi (NULL)
		{ }

		Response (Request * request) :
			_file_size(0),
			_body(""),
			_request(request),
			_error_path(""),
			_message_send(""),
			_error_value(request->get_error_value()),
			_server(request->get_server()),
			_location(request->get_location()),
            _cgi(NULL)
		{}

		Response (const Response & copy) {
			*this = copy;
		}

		~Response (void)
		{ 
			// if (_cgi != NULL)
			// 	delete _cgi;
		}

	public: /* Accessors */

		int 				get_file_size (void) const 				{ return _file_size; }
		Request * 			get_request (void)		 				{ return _request; }
		std::string 		get_error_path (void) const 			{ return _error_path; }
		std::string 		get_message_send (void) const 			{ return _message_send; }
		int 				get_error_value (void) const 			{ return _error_value; }
		Server * 			get_server (void) const 				{ return _server; }
		Location * 			get_location (void) const 				{ return _location; }
		std::string			get_body (void) const 					{ return _body; }
		std::string			get_cookie(void) const 					{ return _request->get_params("Cookie"); }
        Cgi *               get_cgi (void) const 					{ return _cgi; }

		void				set_file_size(int size) 				{ _file_size = size; }
		void				set_request(Request * request) 			{ _request = request; }
		void				set_error_path(std::string path) 		{ _error_path = path; }
		void				set_message_send(std::string message) 	{ _message_send = message; }
		void				set_error_value(int value) 				{ _error_value = value; }
		void				set_server(Server * server) 			{ _server = server; }
		void				set_location(Location * location) 		{ _location = location; }
        void				set_body(std::string body) 				{ _body = body; }

	public: /* Functions */

		bool			cookie_exist(void)						{ return _request->params_exist("Cookie"); }
		void			fill_status_code(void);
		void 			fill_header(void);
		void 			fill_start_header(void);
		void			fill_body(void);
		void			manage_response(void);
		void			manage_response_get(void);
		void			manage_response_post(void);
		void			manage_response_delete(void);
		void			manage_response_cgi(void);
		bool			manage_autoindex(void);
		void			manage_error_page(void);
		void			generate_header(void);
		std::string		auto_index (void);
		void			create_upload_file(std::string);
		int				manage_cgi (void);
		void			fatal_error (void);
		bool			return_path_url(void);

	public: /* Operators overloaded */

		friend std::ostream& operator<<(std::ostream& o, const Response & ref) {
			o << ref._file_size << std::endl;
			o << ref._body << std::endl;
			o << ref._request << std::endl;
			o << ref._error_path << std::endl;
			o << ref._message_send << std::endl;
			o << ref._error_value << std::endl;
			o << ref._server << std::endl;
			o << ref._location << std::endl;
            o << ref._cgi << std::endl;
			return o;
		}

		Response & operator=(const Response & copy) {
			if (this == &copy) {return (*this);}
			_file_size = copy._file_size;
			_body = copy._body;
			_request = copy._request;
			_error_path = copy._error_path;
			_message_send = copy._message_send;
			_error_value = copy._error_value;
			_server = copy._server;
			_location = copy._location;
            _cgi = copy._cgi;
			return (*this);
		}

		public: /* Exceptions */
};

_END_NAMESPACE_WEBSERV

#endif
