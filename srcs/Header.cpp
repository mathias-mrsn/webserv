/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Header.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamaurai <mamaurai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/23 18:01:44 by mamaurai          #+#    #+#             */
/*   Updated: 2022/09/23 18:01:48 by mamaurai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

static const std::string
get_instant_time_ (void) {

    char buf[1000];
    time_t now = time(NULL);
    struct tm tm = *gmtime(&now);
    strftime(buf, sizeof buf, "%a, %d %b %Y %H:%M:%S %Z", &tm);
    return (buf);
}

static const std::string
find_content_type_ (std::map<std::string, std::string> & content_type, const std::string & ext) {

    for (std::map<std::string, std::string>::iterator it = content_type.begin(); it != content_type.end(); it++) {
        if (it->first == ext)
            return (it->second);
    }
    return ("");
}

void
INLINE_NAMESPACE::Header::fill(Response & reponse)
{
    std::map<short, std::string> error_pages = webserv::init_error_map();
    std::map<short, std::string>::const_iterator it = error_pages.begin();
    std::map<std::string, std::string> content_type = init_content_type();
    std::string type = find_content_type_(content_type, get_file_extension(reponse.get_request()->get_construct_path()));

    for (; it != error_pages.end(); ++it) {
        if (it->first == _status_code)
            break;
    }
    _status = "HTTP/1.1 " + ITOA(it->first) + " " + it->second;
    _date = "Date: " + get_instant_time_();
    _server = "Server: ";
    if (reponse.get_server()) {
	    _server += reponse.get_server()->get_server_name();
    } else {
        _server += "unknown";
    }
    _content_length = "Content-Length: " + ITOA(reponse.get_body().size());
    _content_type += type + ";charset=UTF-8";
    if (reponse.get_cgi() && !reponse.get_cgi()->get_param("Set-Cookie").empty()) {
        _cookie = "Set-Cookie: " + reponse.get_cgi()->get_param("Set-Cookie");
    }
}

std::string
INLINE_NAMESPACE::Header::append (void) {
    std::string header;

    header = _status + "\r\n";
    header += _date + "\r\n";
    header += _server + "\r\n";
    header += _content_type + "\r\n";
    header += _content_length + "\r\n";
    header += _cookie + "\r\n";
	header += "\r\n";

	return (header);
}