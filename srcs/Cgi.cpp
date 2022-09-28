/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamaurai <mamaurai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/02 14:43:52 by mamaurai          #+#    #+#             */
/*   Updated: 2022/09/27 13:58:02 by mamaurai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cgi.hpp"

void
INLINE_NAMESPACE::Cgi::wait (Response * res) {
    int status;

    read_output(_fd);
    waitpid(-1, &status, 0);
    if (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_FAILURE) {
        DEBUG_5(CNOUT(BRED << "Error : CGI failed" << CRESET));
        res->set_error_value(502);
    }
    close (_fd);
    DEBUG_3(CNOUT(BBLU << "Updating : CGI has been successfully executed" << CRESET));
    DEBUG_4(CNOUT(BMAG << "[CGI]" << CRESET))
    DEBUG_4(CNOUT(BWHT << _output << CRESET))

    std::string header;
    std::string content;
    size_t ret_find;

    if ((ret_find = _output.find("\r\n\r\n")) != std::string::npos) {
        header = _output.substr(0, ret_find);
        content = _output.substr(ret_find + 4, _output.length());
    }
    else {
        header = _output;
        content = "";
    }
    _output = content;

    string_vector headers = delimiter_spliter(header);

    for (string_vector::const_iterator it = headers.begin(); it != headers.end(); it++) {
        if ((ret_find = (*it).find("Set-Cookie: ")) != std::string::npos) {
            _params["Set-Cookie"] = (*it).substr(ret_find + 12);
        }
    }
    res->set_body(_output);
}

void
INLINE_NAMESPACE::Cgi::manage_output (__attribute__((unused))Response * res) {
    uint64_t ret;
    if ((ret = _output.find("\r\n\r\n")) != std::string::npos) {
        _output.erase(0, ret + 4);
        res->set_body(_output);
    }
    res->set_body(_output);
    close(_fd);
}

int
INLINE_NAMESPACE::Cgi::read_output(int fd) {
    char buffer[BUFFER_SIZE];
    int ret;

    while ((ret = read(fd, buffer, BUFFER_SIZE)) > 0) {
        _output.append(buffer, ret);
    }
    if (ret == SYSCALL_ERR)
        return (SYSCALL_ERR);
    return (0);
}

void
INLINE_NAMESPACE::Cgi::start (__attribute__((unused))Response * res) {
    pid_t pid;
    int pip1[2];
    int pip2[2];

    // res->set_error_value(512);
    DEBUG_3(CNOUT(BBLU << "Updating : starting CGI..." << CRESET));
    if (pipe(pip1) == SYSCALL_ERR || pipe(pip2) == SYSCALL_ERR) {
        DEBUG_5(CNOUT(BRED << "Error : pipe() failed (l." << __LINE__ << ")" << CRESET))
        return;
    }
    if (write(pip1[1], _request->get_content().c_str(), _request->get_content().length()) == SYSCALL_ERR) {
        DEBUG_5(CNOUT(BRED << "Error : write() failed (l." << __LINE__ << ")" << CRESET))
        return;
    }
    if ((pid = fork()) == SYSCALL_ERR) {
        return;
    } else if (pid == 0) {
        close(pip1[1]);
        if (dup2(pip1[0], 0) == SYSCALL_ERR) {
            DEBUG_5(CNOUT(BRED << "Error : dup2() failed (l." << __LINE__ << ")" << CRESET))
            return;
        }
        close(pip1[0]);

        close(pip2[0]);
        if (dup2(pip2[1], 1) == SYSCALL_ERR) {
            DEBUG_5(CNOUT(BRED << "Error : dup2 failed (l." << __LINE__ << ")" << CRESET))
            return;
        }
        close(pip2[1]);

        if (execve(_exec.c_str(), (char *[2]){const_cast<char *>(_exec.c_str()), NULL}, this->_env) == SYSCALL_ERR) {
            close(pip1[0]);
            close(pip2[1]);
            close(pip1[1]);
            close(pip2[0]);
            close(0);
            close(1);
            close(2);
            delete this->get_request();
            free_env();
            delete this;
            throw   std::exception();
        }
    } else {

        close(pip1[0]);
        close(pip2[1]);
        close(pip1[1]);
        _fd = pip2[0];
    }
}

void
INLINE_NAMESPACE::Cgi::free_env (void) {
	for (char **it = _env; *it; it++) {
		free(*it);
	}
	free(_env);
}

void
INLINE_NAMESPACE::Cgi::init (void) {
    string_vector env_vars = create_env();

    DEBUG_3(CNOUT(BBLU << "Updating : environment variables for CGI created" << CRESET))
    if (!(_env = static_cast<char **>(malloc(sizeof(char *) * (env_vars.size() + 1)))))
        return ;
    for (uint64_t i = 0; i < env_vars.size(); i++) {
        if ((_env[i] = strdup(env_vars[i].c_str())) == NULL) {
            for (uint64_t j = 0; j < i; j++)
                free(_env[j]);
            free(_env);
            return ;
        }
    }
    _env[env_vars.size()] = NULL;
}

string_vector
INLINE_NAMESPACE::Cgi::create_env (void) const {
    string_vector envs(24);

    envs[0] = "CONTENT_LENGTH=" + ITOA(_request->get_params("Content-Length"));
    envs[1] = "CONTENT_TYPE=" + _request->get_params("Content-Type"); // mostly text/html
    envs[2] = "GATEWAY_INTERFACE=CGI/1.1"; // CGI/1.1
    envs[3] = "PATH_INFO=" + _request->get_path(); // path to the file
    envs[4] = "PATH_TRANSLATED=" + _request->get_path(); // path to the file
    envs[5] = "QUERY_STRING=" + _request->get_query_string(); // ?arg1=value1&arg2=value2
    envs[6] = "REDIRECT_STATUS=" + ITOA(_request->get_error_value()); // mostly 200
    switch (_request->get_method()) {
        case M_GET:
            envs[7] = "REQUEST_METHOD=GET";
            break;
        case M_POST:
            envs[7] = "REQUEST_METHOD=POST";
            break;
        case M_DELETE:
            envs[7] = "REQUEST_METHOD=DELETE";
            break;
        default:
            envs[7] = "REQUEST_METHOD=";
            break;
    }
    envs[8] = "SCRIPT_FILENAME=" + _request->get_construct_path(); // /home/mamaurai/www/index.html
    envs[9] = "SERVER_PROTOCOL=HTTP/1.1"; // usually HTTP/1.1
    envs[10] = "SERVER_PORT=" + ITOA(_request->get_params("Host").substr(_request->get_params("Host").find(":"))); // 8080 for exemple
    envs[11] = "AUTH_TYPE=Basic";
    envs[12] = "SERVER_NAME=" + _request->get_location()->get_server()->get_server_name();
    envs[13] = "SERVER_SOFTWARE=webserv/beta";
    envs[14] = "HTTP_ACCEPT=" + _request->get_params("Accept"); // Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8
    envs[15] = "HTTP_COOKIE=" + _request->get_params("Cookie");
    envs[16] = "HTTP_ACCEPT_LANGUAGE=" + _request->get_params("Accept-Language"); // Accept-Language: en-US,en;q=0.5
    envs[17] = "HTTP_REFERER=" + _request->get_params("Referer");
    envs[18] = "HTTP_USER_AGENT=" + _request->get_params("User-Agent"); // Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:45.0) Gecko/20100101 Firefox/45.0
    envs[19] = "HTTP_ACCEPT_ENCODING=" + _request->get_params("Accept-Encoding"); // Accept-Encoding: gzip, deflate
    envs[20] = "HTTP_ACCEPT_CHARSET=" + _request->get_params("Accept-Charsets"); // Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7
    envs[21] = "DOCUMENT_ROOT=" + _request->get_location()->get_root(); // /home/mamaurai/www
    envs[22] = "REQUEST_URI=" + _request->get_path() + ((!_request->get_query_string().empty()) ? ("?" + _request->get_query_string()) : ""); // /index.html?arg1=value1&arg2=value2
    envs[23] = "UPLOAD_PATH=" + _request->get_location()->get_upload_path(); // /home/mamaurai/www/uploads

    return (envs);
}
