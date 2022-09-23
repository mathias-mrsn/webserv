/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamaurai <mamaurai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/23 18:02:19 by mamaurai          #+#    #+#             */
/*   Updated: 2022/09/23 18:02:20 by mamaurai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

#include "webserv.hpp"

_BEGIN_NAMESPACE_WEBSERV

class Request
{
	public:
		typedef std::map<std::string, std::string> param_type;

	private:
		param_type&	_params;
		int			_method;
		std::string _path;
		std::string _construct_path;
		std::string _version;
		bool		_chunked;
		int			_error_value;
		std::string _query_string;
		Server *	_server;
		Location *  _location;
		std::string _body;
		std::string	_boundary;
		std::string _filename;
		std::string _content_file;
        std::string _content;
		std::string _header_content;

	public:
		Request (void) :
			_params(*new param_type()),
			_method(0),
			_path(""),
			_construct_path(""),
			_version(""),
			_chunked(false),
			_error_value(0),
			_query_string(""),
			_server(NULL),
			_location(NULL),
			_body(""),
			_boundary(""),
			_filename(""),
			_content_file(""),
            _content(""),
			_header_content("")
		{
			FOREACH_HEADER {
				_params[*it] = "";
			}
			_params["Accept-Language:"] = "en";
			_params["Content-Disposition:"] = "attachment";
			_params["Connection:"] = "keep-alive";
		}

		virtual ~Request (void) {
			_params.clear();
			delete &_params;
		}

		Request (const Request & ref) :
			_params(ref._params),
			_method(ref._method),
			_path(ref._path),
			_construct_path(ref._construct_path),
			_version(ref._version),
			_chunked(ref._chunked),
			_error_value(ref._error_value),
			_query_string(ref._query_string),
			_server(ref._server),
			_location(ref._location),
			_body(ref._body),
			_boundary(ref._boundary),
			_filename(ref._filename),
			_content_file(ref._content_file),
            _content(ref._content),
			_header_content(ref._header_content)
		{ }

		Request (const std::string str) :
			_params(*new param_type()),
			_method(0),
			_path(""),
			_construct_path(""),
			_version(""),
			_chunked(false),
			_error_value(0),
			_query_string(""),
			_server(NULL),
			_location(NULL),
			_body(""),
			_boundary(""),
			_filename(""),
			_content_file(""),
            _content(""),
			_header_content("")
		{
            FOREACH_HEADER {
                _params[*it] = "";
            }
            _params["Accept-Language:"] = "en";
            _params["Content-Disposition:"] = "attachment";
            _params["Connection:"] = "keep-alive";
			_chunked = (str.find("\r\n\r\n") == std::string::npos);
			_error_value = request_parser();
		}

		Request (char *str, int size) :
			_params(*new param_type()),
			_method(0),
			_path(""),
			_construct_path(""),
			_version(""),
			_chunked(false),
			_error_value(0),
			_query_string(""),
			_server(NULL),
			_location(NULL),
			_body(""),
			_boundary(""),
			_filename(""),
			_content_file(""),
            _content(""),
			_header_content("")
		{
            FOREACH_HEADER {
                _params[*it] = "";
            }
            _params["Accept-Language:"] = "en";
            _params["Content-Disposition:"] = "attachment";
            _params["Connection:"] = "keep-alive";
			// add_body(str, size);
			_body.insert(0, str, size);
			//_error_value = request_parser();
			_error_value = 200;
		}

	public:

		param_type & 			get_params (void) const 		{ return (_params); }
		int						get_method (void) const				{ return (_method); }
		std::string				get_path (void) const				{ return (_path); }
		std::string				get_construct_path (void) const 	{ return (_construct_path); }
		std::string				get_version (void) const				{ return (_version); }
		bool					get_chunked (void) const				{ return (_chunked); }
		std::string &			get_body (void) 				{ return (_body); }
		int						get_error_value (void)const			{ return (_error_value); }
		std::string 			get_query_string (void) const		{ return (_query_string); }
		Server *				get_server (void) const				{ return (_server); }
		Location *				get_location (void) const			{ return (_location); }
		std::string				get_params (std::string str) const	{ return (_params.at(str)); }
		std::string				get_filename (void) const			{ return (_filename); }
		std::string				get_content_file (void) const		{ return (_content_file); }
        std::string             get_content (void) const            { return (_content); }
		std::string				get_header_content (void) const		{ return (_header_content); }

		// void					add_body(char * str_add, int bytes);
		void					add_body(char * str_add, int bytes) { _body.insert(_body.size(), str_add, bytes);  }

		void					set_chunked (bool b) 			{ _chunked = b; }
		void					set_error_value (int i)			{ _error_value = i; }
		void                    set_query_string (std::string str)    { _query_string = str; }
		void					set_server (Server * s)			{ _server = s; }
		void					set_location (Location * l)		{ _location = l; }
		void					set_filename (std::string str)		{ _filename = str; }
		void					set_content_file (std::string str)	{ _content_file = str; }
		void					set_boundary (std::string str)		{ _boundary = str; }
		void					set_version (std::string str)		{ _version = str; }
		void					set_construct_path (std::string str)	{ _construct_path = str; }
		void					set_path (std::string str)			{ _path = str; }
		void					set_method (int i)				{ _method = i; }
		void					set_params (param_type & p)		{ _params = p; }
		void					set_body (std::string str)			{ _body = str; }
        void					set_content (std::string str)		{ _content = str; }
		void					set_header_content (std::string str)	{ _header_content = str; }

		bool					params_exist(std::string str) 						{ if (_params.find(str) != _params.end()) return false; else return true; }

		void					clear (void) {
			_params.clear();
			_method = 0;
			_path = "";
			_construct_path = "";
			_version = "";
			_chunked = false;
			_error_value = 0;
			_body = "";
			_query_string = "";
			_server = NULL;
			_location = NULL;
			_boundary = "";
			_filename = "";
			_content_file = "";
            _content = "";
			_header_content = "";
		}

	public:

		int		request_parser (void);
		int		check_first_line (void);
		short	parse_first_line (std::string);
		void	request_line_parser (std::string);
		void	set_final_path (void);
		bool	is_upload_case (void);
		bool	define_upload (void);
		short	check_request (void);
		void	unchunk_body (void);
        void    parse_content (void);
		bool	max_body_size_check (size_t size);

	public:
	
		Request & operator= (const Request & ref) {
			if (this == &ref) {return (*this);}

			_method = ref._method;
			_path = ref._path;
			_construct_path = ref._construct_path;
			_version = ref._version;
			_chunked = ref._chunked;
			_params = ref._params;
			_body = ref._body;
			_error_value = ref._error_value;
			_query_string = ref._query_string;
			_server = ref._server;
			_location = ref._location;
			_boundary = ref._boundary;
			_filename = ref._filename;
			_content_file = ref._content_file;
            _content = ref._content;
			_header_content = ref._header_content;
			return *this;
		}

		friend std::ostream & operator<< (std::ostream & o, const Request & ref) {
			o << "Method: " << ref._method << std::endl;
			o << "Path: " << ref._path << std::endl;
			o << "Construct path: " << ref._construct_path << std::endl;
			o << "Version: " << ref._version << std::endl;
			o << "Chunked: " << ref._chunked << std::endl;
			o << "Body: " << ref._body << std::endl;
			o << "Error value: " << ref._error_value << std::endl;
			o << "Query string: " << ref._query_string << std::endl;
			o << "Server: " << ref._server << std::endl;
			o << "Location: " << ref._location << std::endl;
			o << "Boundary: " << ref._boundary << std::endl;
			o << "Filename: " << ref._filename << std::endl;
			o << "Content file: " << ref._content_file << std::endl;
            o << "Content: " << ref._content << std::endl;
			o << "Header content: " << ref._header_content << std::endl;
			o << "Params: " << std::endl;
			for (std::map<std::string, std::string>::const_iterator it = ref._params.begin(); it != ref._params.end(); ++it) {
				o << " -> " << it->first << ": " << it->second << std::endl;
			}
			return o;
		}
	
};

_END_NAMESPACE_WEBSERV

#endif
