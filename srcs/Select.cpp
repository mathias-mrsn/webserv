/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Select.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamaurai <mamaurai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/18 11:30:22 by mamaurai          #+#    #+#             */
/*   Updated: 2022/09/21 11:11:30 by mamaurai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

void
INLINE_NAMESPACE::Select::webserv_log_input(Request &request) {

    COUT(BWHT << "Incoming Request   ➜ " << CRESET)
    if (request.get_method() == M_GET)
        COUT(BGRN << "GET " << CRESET)
    else if (request.get_method() == M_POST)
        COUT(BGRN << "POST " << CRESET)
    else if (request.get_method() == M_DELETE)
        COUT(BGRN << "DELETE " << CRESET)
    else
        COUT(BRED << "ERROR " << CRESET)

    COUT(BYEL << "/" << request.get_construct_path() << " " << CRESET)
    if (!request.get_params("Host").empty()) {
        COUT(BYEL << "Host: " << request.get_params("Host") << CRESET)
    }
    COUT(std::endl)
    DEBUG_2(COUT(BMAG << "[Request send by client]" << CRESET << std::endl))
    DEBUG_2(COUT(BWHT << request.get_body() << CRESET << std::endl))
    DEBUG_2(COUT(BMAG << "[End of request]" << CRESET << std::endl))
}

void
INLINE_NAMESPACE::Select::webserv_log_output(Response &response) {
    std::map<short, std::string> errors = init_error_map();

    COUT(BWHT << "Outcoming Response ➜ " << CRESET)
    COUT(BYEL << "HTTP/1.1 " << CRESET)
    if (response.get_error_value() == 200)
        COUT(BGRN << ITOA(response.get_error_value()) << CRESET)
    else
        COUT(BRED << ITOA(response.get_error_value()) << CRESET)
    for (std::map<short, std::string>::iterator it = errors.begin(); it != errors.end(); ++it)
        if (it->first == response.get_error_value()) {
            COUT(BYEL << " " << it->second << CRESET)
            break;
        }
    COUT(std::endl)
    DEBUG_2(COUT(BMAG << "[Response send by server]" << CRESET << std::endl))
    DEBUG_2(COUT(BWHT << response.get_body() << CRESET << std::endl))
    DEBUG_2(COUT(BMAG << "[End of response]" << CRESET << std::endl))
}

void
INLINE_NAMESPACE::Select::setup(void) {
    FD_ZERO(&_readfds);

    DEBUG_3(CNOUT(BBLU << "Updating : servers are setting up..." << CRESET))
    FOREACH_SERVER {
		for (std::vector<int>::iterator it2 = (*it)->get_port().begin(); it2 != (*it)->get_port().end(); ++it2) {
	        INLINE_NAMESPACE::Socket sock;
	        sock.setup((*it2));
	        for (int i = 0; i < MAX_CLIENT; i++)
	            _client_socket[i] = 0;
	        DEBUG_3(CNOUT(BBLU << "Updating : server " << (*it2) << " is ready to be used" << CRESET))
	        _sockets.push_back(sock);
		}
    }
}

void
INLINE_NAMESPACE::Select::disconnect_client(int i) {
	FD_CLR(_client_socket[i], &_readfds);
    if (_client_socket[i] > 0) {
        close(_client_socket[i]);
    }
    _client_socket[i] = 0;
}

void
INLINE_NAMESPACE::Select::_init_socket (void) {
    FD_ZERO(&_readfds);
    FD_ZERO(&_writefds);
    for (socket_type::iterator it = _sockets.begin(); it != _sockets.end(); ++it) {
        if (fcntl(it->get_master_socket(), F_SETFL, O_NONBLOCK))
            DEBUG_5(CNOUT(BRED << "Error : fcntl() failed (l." << __LINE__ << ")" << CRESET))
        FD_SET(it->get_master_socket(), &_readfds);
        if (it->get_master_socket() > get_max_sub_socket())
            set_max_sub_socket(it->get_master_socket());
        FD_SET(it->get_master_socket(), &_writefds);
        for (int i = 0; i < MAX_CLIENT; i++) {
            if (_client_socket[i] > 0) {
                FD_SET(_client_socket[i], &_readfds);
                FD_SET(_client_socket[i], &_writefds);
            }
            if (_client_socket[i] > get_max_sub_socket()) {
                set_max_sub_socket(_client_socket[i]);
            }
        }
    }
}

void
INLINE_NAMESPACE::Select::start(void) {
    size_t size_total = 0;

	sigset_t set;
    memset(&set, 0, sizeof(sigset_t));
	sigaddset(&set, SIGPIPE);
	int retcode = sigprocmask(SIG_BLOCK, &set, NULL);
	if (retcode == -1)
		throw std::runtime_error("sigprocmask()");

    while (true) {
        _init_socket();

        // DEBUG_3(CNOUT(BBLU << "Updating : selecting..."))
        int select_ret = select(get_max_sub_socket() + 1, &_readfds, &_writefds, NULL, NULL);
        if (g_exit) {
            return;
        } if (select_ret == SYSCALL_ERR) {
            throw Select::fSelectError();
        }
        new_request();
        char buffer[10025];
        int bytes = 0;

        for (int i = 0; i < MAX_CLIENT; i++) {
            size_total = bytes;
            if (_client_socket[i] != 0 && FD_ISSET(_client_socket[i], &_readfds)) {
                bytes = recv(_client_socket[i], buffer, 10024, 0);
                if (bytes == SYSCALL_ERR) {
                    DEBUG_5(CNOUT(BRED << "Error : recv() failed (l." << __LINE__ << ")" << CRESET))
					disconnect_client(i);
                    break;
                } else if (bytes == 0) {
                    DEBUG_3(CNOUT(BBLU << "Updating : client disconnected = " << _client_socket[i] << "#" << CRESET))
                    disconnect_client(i);
                } else {
                    size_total += bytes;
                    buffer[bytes] = '\0';
                    Request *request = new Request(buffer, bytes);

					if (request->get_method() == M_POST && bytes >= 10024) {
                        DEBUG_3(CNOUT(BBLU << "Updating : POST Request is parsing..."))
                        while (bytes > 0) {				
                            if (_client_socket[i] != 0 && FD_ISSET(_client_socket[i], &_readfds)) {
                                for (int j = 0; j < 10025; j++) {
                                    buffer[j] = '\0';
                                }
                                bytes = recv(_client_socket[i], buffer, 10024, 0);
                                DEBUG_3(CNOUT(BBLU << "Updating : recv has read " << bytes << " bytes" << CRESET))
								if (bytes == SYSCALL_ERR) {
                                    DEBUG_5(CNOUT(BRED << "Error : recv() failed (l." << __LINE__ << ")" << CRESET))
									disconnect_client(i);
									CNOUT(UMAG <<  "------------------------------->" << CRESET)
									break;
                                } else if (bytes == 0) {
                                    DEBUG_3(CNOUT(BBLU << "Updating : client disconnected = " << _client_socket[i] << "#" << CRESET))
									disconnect_client(i);
                                } else {
                                    buffer[bytes] = '\0';
                                    request->add_body(buffer, bytes);
                                    size_total += bytes;
									if (bytes < 10024)
										break;
                                }
                            } else
                                break;
                        }
                    }

                    if (request->get_chunked() && bytes >= 10024) {
                        DEBUG_3(CNOUT(BBLU << "Updating : chunked Request is parsing..."))
                        while (bytes > 0) {
                            for (int j = 0; j < 10024; j++) {
                                buffer[j] = '\0';
                            }
                            if (_client_socket[i] != 0 && FD_ISSET(_client_socket[i], &_readfds)) {
                                bytes = recv(_client_socket[i], buffer, 10024, 0);
                                DEBUG_3(CNOUT(BBLU << "Updating : recv has read " << bytes << " bytes" << CRESET))
                                if (bytes == SYSCALL_ERR) {
									DEBUG_5(CNOUT(BRED << "Error : recv() failed (l." << __LINE__ << ")" << CRESET))
									disconnect_client(i);
                                    break;
                                } else if (bytes == 0) {
                                    DEBUG_3(CNOUT(BBLU << "Updating : client disconnected = " << _client_socket[i] << CRESET))
									disconnect_client(i);
                                } else {
                                    buffer[bytes] = '\0';
                                    size_total += bytes;
                                    request->add_body(buffer, bytes);
									if (bytes < 10024)
										break;
                                }
                            }
                            std::string buffer_s(buffer);
                            if (buffer_s.find("\0\r\n\r\n") != std::string::npos) {
                                break;
                            }
                        }
                        //TODO reparse chunked body
                    }

                    DEBUG_3(CNOUT(BBLU << "Updating : Request has been parsed" << CRESET))
                    DEBUG_1(webserv_log_input(*request);)
                    if (request->get_error_value() != 413 && request->get_body().size() > 0)
                        request->max_body_size_check(size_total);
                    DEBUG_3(CNOUT(BBLU << "Updating : creating response..." << CRESET))
                    Response response(request);
                    response.manage_response();
                    response.set_message_send(response.get_body());

                    DEBUG_3(CNOUT(BBLU << "Updating : Response has been created" << CRESET))
                    DEBUG_1(webserv_log_output(response);)
                    DEBUG_3(CNOUT(BBLU << "Updating : sending the response..." << CRESET))
                    size_t start = 0;
                    std::string tmp;
                    int	nb_piece = calculate_size_piece_file(response.get_message_send().size());
                    for (int j = 0; j < nb_piece; j++)
                    {
                        tmp = response.get_message_send().substr(start, response.get_message_send().size() / nb_piece);
                        if (FD_ISSET(_client_socket[i], &_writefds))
                        {
                        	send(_client_socket[i], tmp.c_str(), tmp.size(), 0);
                        	if (bytes == SYSCALL_ERR) {
                        	    DEBUG_5(CNOUT(BRED << "Error : send() failed (l." << __LINE__ << ")" << CRESET))
                        	    disconnect_client(i);
                        	    break;
                        	}
                        	usleep(50000);
                        	start += tmp.size();
                        }
                    }
                    tmp = response.get_message_send().substr(start);
					if (FD_ISSET(_client_socket[i], &_writefds))
                    {
						send(_client_socket[i], tmp.c_str(), tmp.size(), 0);
                  		if (bytes == SYSCALL_ERR) {
                        	DEBUG_5(CNOUT(BRED << "Error : send() failed (l." << __LINE__ << ")" << CRESET))
                        	disconnect_client(i);
                        	break;
                 		}
					}
                    
                    delete request;
                    DEBUG_3(CNOUT(BBLU << "Updating : Response has been sent" << CRESET))
                }
            }
        }

    }
}

void
INLINE_NAMESPACE::Select::new_request(void) {
    int _new_socket = 0;
    for (socket_type::iterator it = _sockets.begin(); it != _sockets.end(); ++it) {
        if (FD_ISSET(it->get_master_socket(), &_readfds)) {
            int addrlen = it->get_addrlen();
            if ((_new_socket = accept(it->get_master_socket(), (struct sockaddr *) &(it->get_address()),
                                      (socklen_t *) &addrlen)) == SYSCALL_ERR) {
                throw Select::fAcceptError();
            }
            if (fcntl(_new_socket, F_SETFL, O_NONBLOCK)) {
                DEBUG_5(CNOUT(BRED << "Error : fcntl() failed (l." << __LINE__ << ")" << CRESET))
            }
        }
    }
    for (int i = 0; i < MAX_CLIENT; i++) {
        if (_client_socket[i] == 0 && _new_socket != 0)
        {
            DEBUG_3(CNOUT(BBLU << "Updating : adding \'" << _new_socket << "\' to client socket number " << i << CRESET))
            _client_socket[i] = _new_socket;
            break;
        }
    }
}
