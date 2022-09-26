/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gmary <gmary@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/23 18:01:30 by mamaurai          #+#    #+#             */
/*   Updated: 2022/09/26 17:25:16 by gmary            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include <iostream>
#include <fstream>
#include <dirent.h>

/*
	curl -X DELETE localhost:8080/file_to_delete/text.txt
*/

void INLINE_NAMESPACE::Response::manage_response_delete(void) {
	if (std::remove(_request->get_construct_path().c_str()) != 0) {
        _request->set_error_value(403);
    }
    if (_request->get_error_value() == 200) {
        _body.append("<html>\n");
        _body.append("<body>\n");
        _body.append("<h1>FILE DELETED</h1>\n");
        _body.append("</body>\n");
        _body.append("</html>\n");
    } else
        _body.append(create_html_error_page(_request->get_error_value()));
}

void INLINE_NAMESPACE::Response::create_upload_file(std::string upload_path) {
    std::ofstream file;
    std::string final_path = upload_path;

    final_path.append("/");
    final_path.append(_request->get_filename());
    if (upload_path.empty())
	{
		file.open(("example_html/upload/" + _request->get_filename()).c_str(), std::ios::out | std::ios::binary);
	}
    else
        file.open(final_path.c_str(), std::ios::out | std::ios::binary);
    file << _request->get_content_file();
    file.close();
}

void INLINE_NAMESPACE::Response::manage_response_post(void) {
    bool isupload = false;
    Location *location_ptr = _request->get_location();

	if (access(location_ptr->get_upload_path().c_str(), R_OK) < 0)
	{
        if (location_ptr->get_upload_path().empty())
		    set_error_value(502);
        else
            set_error_value(403);
		return ;
	}
    if (_request->define_upload()) {
        isupload = true;
    }

    if (location_ptr && !location_ptr->get_upload_path().empty() && isupload) {
        create_upload_file(location_ptr->get_upload_path());
    } else if (location_ptr && !isupload) {
        int size = 0;
	    size = read_file(_request->get_construct_path()).size();
        _body.insert(_body.size(), read_file(_request->get_construct_path()).c_str(), size);
	    _body.insert(_body.size(), "\r\n\r\n", 4);
    }
    if (isupload) {
        _body.append("<html>\n");
        _body.append("<body>\n");
        _body.append("<h1>FILE UPLOADED</h1>\n");
        _body.append("</body>\n");
        _body.append("</html>\n");
    }
}


void INLINE_NAMESPACE::Response::manage_response_get(void) {
	int size = 0;
	if (return_path_url() == false)
	{
		size = read_file(_request->get_construct_path()).size();
		_body.insert(_body.size(), read_file(_request->get_construct_path()).c_str(), size);
	}
	_body.insert(_body.size(), "\r\n\r\n", 4);
}

void
INLINE_NAMESPACE::Response::manage_error_page(void) {
    std::string ret;

	// if (_error_value == 400)
	// {
	// 	_body.append(create_html_error_page(_error_value));
	// 	return ;
	// }
    if (_location
        && _error_value == 403
        && _location->get_autoindex()
        && ((path_is_dir(_request->get_construct_path())) || _request->get_construct_path().empty())) {
        _error_value = 200;
        _body = auto_index();
    } else if (_location && !(ret = _location->return_path_matching(_error_value)).empty()) {
        _body.append(read_file(ret));
    } else if (_server && !(ret = _server->return_path_matching(_error_value)).empty()) {
        _body.append(read_file(ret));
    } else {
        _body.append(create_html_error_page(_error_value));
    }
}

int
INLINE_NAMESPACE::Response::manage_cgi(void) {

    Location *location_ptr = _request->get_location();
    Location::cgi_type cgi = location_ptr->get_cgi();

    if (location_ptr == NULL) {
        return (1);
    }
    for (Location::cgi_type::const_iterator it = cgi.begin(); it != cgi.end(); ++it) {
        if ((it->first == get_file_extension(_request->get_construct_path()))) {
            _cgi = new Cgi(it->first, it->second, location_ptr, _request);
            break;
        }
    }
    if (_cgi == NULL) {
        return (1);
    }
    _cgi->init();
    _cgi->start(this);
    _cgi->wait(this);
	_cgi->free_env();
    return (0);
}

void
INLINE_NAMESPACE::Response::fatal_error(void) {
    _body = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
    _body.append(create_html_error_page(500));
}

void
INLINE_NAMESPACE::Response::manage_response(void) {
    if (_error_value == FATAL_ERROR) {
        _error_value = 400;
        _body = "HTTP/1.1 400 Bad Request\r\n\r\n";
        _body.append(create_html_error_page(400));
        return;
    }
    if (_error_value == 200) {
        if (manage_cgi()) {
            if (_request->get_method() == M_GET)
                manage_response_get();
            else if (_request->get_method() == M_POST)
                manage_response_post();
            else if (_request->get_method() == M_DELETE)
                manage_response_delete();
        }
    }
    if (_error_value != 200) {
        manage_error_page();
    }

    DEBUG_3(CNOUT(BBLU << "Updating : creating header..." << CRESET))
    Header header(*this);
    std::string header_string;
    header.fill(*this);
    header_string = header.append();
    _body.insert(0, header_string);
    DEBUG_3(CNOUT(BBLU << "Updating : header created and insert into the body" << CRESET))
}

static std::vector<struct dirent> *
file_vector_(const std::string &path) {
    std::vector<struct dirent> *list = new std::vector<struct dirent>();
    DIR *dp;
    struct dirent *ep;

    dp = opendir(path.empty() ? "." : path.c_str());
    if (dp == NULL) {
        delete list;
        return (NULL);
    }
    while ((ep = readdir(dp))) {
        list->push_back(*ep);
    }
    closedir(dp);
    return (list);
}

std::string INLINE_NAMESPACE::Response::auto_index (void) {
    std::vector<struct dirent> *list = file_vector_(_request->get_construct_path());
    if (list == NULL) {
        delete list;
        return (create_html_error_page(404));
    }
    std::string index;

    index += "<html>\n";
    index += "<head><title>Indexito /</title></head>\n";
    index += "<h1>Index of " + _request->get_construct_path() + "</h1>\n";
    index += "<hr>\n";
    index += "<table width=\"100%\" border=\"0\">\n";

    for (std::vector<struct dirent>::iterator it = list->begin(); it != list->end(); ++it) {
        if (!strcmp(it->d_name, ".")) {
            continue;
        } else if (!strcmp(it->d_name, "..")) {
            index += "<tr><td><a href=\"";
            index += it->d_name;
            index += "/\">../</a></td></tr>\n";
        } else {
            std::string file = _request->get_construct_path() + "/" + it->d_name;
            struct stat s;
            stat(file.c_str(), &s);
            std::string date = ctime(&s.st_mtime);
            date.erase(date.size() - 1);

            index += "<tr>\n";
            index += "<td><a href=\"";
            
            index += "/";
            index += _request->get_path();
            if (!_request->get_path().empty()) {
                index += ((_request->get_path()[(_request->get_path().size() - 1)] == '/') ? "" : "/");
            }
            
            index += it->d_name;
            index += S_ISDIR(s.st_mode) ? "/" : "";
            index += "\">";
            index += it->d_name;
            index += S_ISDIR(s.st_mode) ? "/" : "";
            index += "</a></td>\n";
            index += "<td>";
            index += date;
            index += "</td>\n";
            index += "<td align=\"right\">";
            if (S_ISDIR(s.st_mode) == 0) {
                if (s.st_size > 1000) {
                    index += ITOA(s.st_size / 1000) + " Ko";
                } else if (s.st_size > 1000000) {
                    index += ITOA(s.st_size / 1000000) + " Mo";
                } else {
                    index += ITOA(s.st_size) + " octets";
                }
            } else {
                index += "-";
            }
            index += "</td>\n";
            index += "</tr>\n";
        }
    }

    index += "</able>\n";
    index += "</body>\n";
    index += "</html>\n";
    _body.append(index);
    _body.append("\r\n\r\n");

    delete list;
    return index;
}

bool
INLINE_NAMESPACE::Response::return_path_url(void)
{
	std::string ret;

	if (_location && !(ret = _location->return_path_matching(_error_value)).empty()) {
        _body.append(read_file(ret));
    } else if (_server && !(ret = _server->return_path_matching(_error_value)).empty()) {
        _body.append(read_file(ret));
	} else
	{
		_body.append("");
		return (false);
	}
	return (true);
}