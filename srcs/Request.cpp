/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamaurai <mamaurai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/23 14:38:10 by mamaurai          #+#    #+#             */
/*   Updated: 2022/09/28 15:42:18 by mamaurai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

#if defined(USE_HOSTNAME) && (USE_HOSTNAME == true)

static bool
is_number_(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

static INLINE_NAMESPACE::Server *
find_server_ (std::string str) {
    if (str.empty()) {
        return (NULL);
    }

    int pos = str.find(":");
    std::string s_name = str.substr(0, pos);
    std::string port;
    if (pos != std::string::npos)
        port = str.substr(pos + 1, str.size());
    if (s_name.empty()) {
        return (NULL);
    }
    if (!port.empty() && (port.length() > 5 || !is_number_(port))) {
        return (NULL);
    }
    FOREACH_SERVER {
        if ((*it)->get_server_name() == s_name) {
            if (port.empty()) {
                return (*it);
            }
            for (std::vector<int>::const_iterator it2 = (*it)->get_port().begin(); it2 != (*it)->get_port().end(); it2++) {
                if (*it2 == std::atoi(port.c_str())) {
                    return (*it);
                }
            }
        }
    }
    return (NULL);
}

#else

static INLINE_NAMESPACE::Server *
find_server_ (std::string str) {
	if (str.empty()) {
		return (NULL);
	}

	uint64_t pos = str.find(":");
	std::string port = str.substr(pos + 1);
	if (port.empty() || pos == std::string::npos || port.length() > 5)
		return (NULL);
		
	FOREACH_SERVER {
		for (std::vector<int>::const_iterator it2 = (*it)->get_port().begin(); it2 != (*it)->get_port().end(); it2++) {
			if (*it2 == std::atoi(port.c_str()))
				return (*it);
		}
	}
	return (NULL);
}

#endif

static INLINE_NAMESPACE::Location *
find_location_ (INLINE_NAMESPACE::Server * srv, std::string path) {
    uint64_t max_size = 0;
    if (srv == NULL)
        return (NULL);
        
    INLINE_NAMESPACE::Server::location_type loc = srv->get_locations();
    static INLINE_NAMESPACE::Location * save;
    std::string tmp_path;
    std::string tmp_loc_path;
    
    for (INLINE_NAMESPACE::Server::location_type::const_iterator it = loc.begin(); it != loc.end(); it++) {
        tmp_path = (*it)->get_path().substr((((*it)->get_path()[0] == '/') ? 1 : 0), (*it)->get_path().length());
        if (strncmp(tmp_path.c_str(), "./", 2) == 0)
        {
            tmp_path = tmp_path.substr(2, tmp_path.length() - 2);
            (*it)->set_path(tmp_path);
        }
        if (!tmp_path.empty() && tmp_path.length() != path.length())
            tmp_path = (tmp_path + ((tmp_path[tmp_path.length() - 1] != '/') ? "/" : ""));
        if ((tmp_path.empty() || path.rfind(tmp_path, 0) == 0) && max_size <= tmp_path.length()) {
            max_size = tmp_path.length();
            save = *it;
        
        }
    }
    if (!save) {
        return (NULL);
    } else {
        return (save);
    }
}

static std::string
is_header_ (const std::string & str) {
    std::string header_name;
    size_t pos = str.find(": ");

    if (pos == std::string::npos)
        return ("");

    header_name = str.substr(0, pos);
    FOREACH_HEADER {
        if ((*it) == header_name) {
            return (header_name);
        }
    }
    return ("");
}

short
INLINE_NAMESPACE::Request::parse_first_line (std::string str) {
	string_vector vector = vector_spliter(str, " ", "", false);
	
	if (vector.size() < 3)
		return (400);
	else if ((vector[0] != "GET" && vector[0] != "POST" && vector[0] != "DELETE"))
		return (405);
	else if (vector[2] != "HTTP/1.1")
		return (505);

	if (vector[0] == "GET") {
		_method |= M_GET;
	} else if (vector[0] == "POST") {
		_method |= M_POST;
	} else if (vector[0] == "DELETE") {
		_method |= M_DELETE;
	}
	_path = vector[1].substr(((vector[1][0] == '/') ? 1 : 0), vector[1].length());
	_version = vector[2];
	
	return (0);
}

void
INLINE_NAMESPACE::Request::request_line_parser (std::string str) {
	std::string header_name;
	std::string content;
	uint64_t pos = str.find(": ");

	if (pos == std::string::npos)
		return;

	header_name = str.substr(0, pos);
	content = str.substr(pos + 2);

	FOREACH_HEADER {
		if ((*it) == header_name) {
			if (_params[header_name] != "")
				_params[header_name] += " ";
			_params[header_name] += content;
			break;
		}
	}
}

void
INLINE_NAMESPACE::Request::parse_content (void) {
    uint64_t ret;

    ret = _body.find("\r\n\r\n");
    if (ret == std::string::npos)
        return;

    std::string content = _body.substr(ret + 4);
    string_vector vector = delimiter_spliter(content);
    for (string_vector::const_iterator it = vector.begin(); it != vector.end(); it++) {
        _content += *it;
    }
}

void
INLINE_NAMESPACE::Request::set_final_path (void) {
	if (_location == NULL) {
		_construct_path = _path;
		return;
	}
	std::string tmp;

	if (!_location->get_root().empty()) {
		tmp = _location->get_root();
        if (!_path.empty())
		tmp += "/" + _path;
	} else {
		tmp = _path;
	}
	if (path_is_dir(tmp) && !_location->get_index().empty()) {
		
        tmp += "/" + _location->get_index();
	}
	_construct_path = tmp;
    DEBUG_3(CNOUT(BBLU << "Updating : final path is \'" << _construct_path << "\'" << CRESET));
}

short
INLINE_NAMESPACE::Request::check_request (void) {
    if (!_location) {
        return (403);
    }
    if (!(_location && _method & _location->get_methods())) {
        return (405);
    }
    if (!path_is_valid((!_construct_path.empty()) ? _construct_path : "./")) {
        return (404);
    }
    std::ifstream file(_construct_path.c_str());
    if (!file.is_open()) {
        file.close();
        return (403);
    }
    file.close();
	if (path_is_dir(_construct_path)) {
        return (403);
    } 
    if (_server && _server->get_max_body_size() < _content_file.length()) {
        return (413);
    }
    return (0);
}

int
INLINE_NAMESPACE::Request::request_parser (void) {
	string_vector v = delimiter_spliter(_body);
	uint64_t pos;
    int ret;

    DEBUG_3(CNOUT(BBLU << "Updating : parsing Request" << CRESET))
	if (v.empty()) {
        DEBUG_5(CNOUT(BRED << "Error : Request body is empty" << CRESET));
		return (FATAL_ERROR);
	} else if ((ret = parse_first_line(v[0])) != 0) {
        DEBUG_5(CNOUT(BRED << "Error : Request first line is invalid" << CRESET));
		return (ret);
	} else if ((pos = _path.find('?')) != std::string::npos) {
		_query_string = _path.substr(pos + 1, _path.length());
		_path = remove_slash(_path.substr(0, pos));
	}
	for (string_vector::const_iterator it = v.begin() + 1; it != v.end(); it++) {
		if (!is_header_(*it).empty()) {
            request_line_parser(*it);
        }
	}
    parse_content();
	_server = find_server_(_params["Host"]);
	_location = find_location_(_server, _path);
	_chunked = ((_params["Transfer-Encoding"].find("chunked")) != std::string::npos);
	set_final_path();
	_header_content = _body.substr(0, _body.find("\r\n\r\n") + 4);
    if ((ret = check_request()) != 0) {
        return (ret);
    }
	return (200);
}

bool	INLINE_NAMESPACE::Request::is_upload_case (void) {
	if (_method == M_POST) {
		if ((_params["Content-Type"].find("multipart/form-data") != std::string::npos) 
			&& (_params["Content-Type"].find("boundary=") != std::string::npos)
			&& (_body.find("filename=") != std::string::npos)) {
			return (true);
		}
	}
	return (false);
}	

bool	INLINE_NAMESPACE::Request::define_upload(void)
{
	if(!is_upload_case())
		return (false);

	_boundary = vector_spliter((_params["Content-Type"].substr(_params["Content-Type"].find("boundary=") + 9, _params["Content-Type"].length())), " ", "", false)[0];
	if (_body.find("filename=") != std::string::npos) {
		_filename = _body.substr(_body.find("filename=") + 9, _body.length());
		_filename = _filename.substr(_filename.find("\"") + 1, _filename.find("\"", _filename.find("\"") + 1) - _filename.find("\"") - 1);
	}
	_content_file = _body.substr(_body.find(_boundary) + _boundary.length(), _body.length());
	_content_file = _content_file.substr(_content_file.find(_boundary) + _boundary.length(), _content_file.length());
	_content_file = _content_file.substr(0, _content_file.find(_boundary) - 2);
	_content_file = _content_file.substr(_content_file.find("\r\n\r\n") + 4, _content_file.length());
	if (_server->get_max_body_size() < _content_file.length())
		return (false);
	return (true);
}

void INLINE_NAMESPACE::Request::unchunk_body () {
	std::string unchunk_body = "";
	std::string	head = "";
	std::string	last_line = "";
	std::stringstream ss;
    std::string s1;
	int64_t hex_count = -1;
	int	i = 0;

	head = _body.substr(0, _body.find("\r\n\r\n") + 4);
	ss << _body.substr(_body.find("\r\n\r\n") + 4, _body.length());
	for (std::string line; std::getline(ss, line); ) {
		if ((i != 0) && (i % 2 != 0))
		{
			unchunk_body += line.substr(line.find("\r\n")+ 1, line.length());
			if ((hex_count != -1) && static_cast<uint64_t>(hex_count) != (line.length() - 1)) {
				return ;
			}
		}
		else
		{
			last_line = line;
            s1 = last_line.substr(0, last_line.find("\r\n"));
            std::istringstream iss(s1);
            iss >> std::hex >> hex_count;
		}
		i++;
		if (hex_count == 0 && line.find("0\r") != std::string::npos) {
			break;
		}
	}
	_body = head + unchunk_body;
}

bool
INLINE_NAMESPACE::Request::max_body_size_check(size_t size) {
	if (_server && (size - _header_content.size() > _server->get_max_body_size())) {
		set_error_value(413);
		return (false);
	}
	return (true);
}