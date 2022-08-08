/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Select.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gmary <gmary@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/18 11:30:22 by mamaurai          #+#    #+#             */
/*   Updated: 2022/08/08 15:34:24 by gmary            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"



std::string	find_path(char * buffer)
{
	std::string path = "";
	int i = 5;
	while (buffer[i] != ' ')
	{
		path += buffer[i];
		i++;
	}
	return path;

} 

// ATTENTION IL NE FAUT PAS LE PREMIER / DANS LE PATH
std::string	read_open(std::string path)
{
	//std::cout << BRED << path << CRESET << std::endl;
	std::string content = "";
	std::ifstream file(path.c_str());
	if (file.is_open())
	{
		std::string line;
		while (getline(file, line))
		{
			content += line + "\n";
		}
		file.close();
	}
	else
	{
		content = "Error 404: File not found";
	}
	//std::cout << BRED << content << CRESET ;
	return content;
}

void
INLINE_NAMESPACE::Select::setup (void) {
	FD_ZERO(&_readfds);
	
	FOREACH_SERVER {
		INLINE_NAMESPACE::Socket	sock;
		sock.setup((*it)->get_port());
		for (int i = 0; i < MAX_CLIENT; i++)
			_client_socket[i] = 0;
		//fcntl(sock.get_master_socket(), F_SETFL, O_NONBLOCK); //BUG le pb provient peut etre de linit du master socket
		//FD_SET(sock.get_master_socket(), &_readfds); //BUG on le refait ces deux lignes juste apres
		_sockets.push_back(sock);
	}
}

void
INLINE_NAMESPACE::Select::start (void) {
	
	size_t size_total = 0;

	fd_set r_readfds;
	while (true) {
		r_readfds = _readfds; //je crois vraiment pas necessaire au final
		FD_ZERO(&_readfds);
		for (socket_type::iterator it = _sockets.begin(); it != _sockets.end(); ++it) {
			
			fcntl(it->get_master_socket(), F_SETFL, O_NONBLOCK);
			FD_SET(it->get_master_socket(), &_readfds);
			if (it->get_master_socket() > get_max_sub_socket())
				set_max_sub_socket(it->get_master_socket());
			
			for (int i = 0; i < MAX_CLIENT; i++)
			{
				//it->set_sub_socket(_client_socket[i]);
				if (_client_socket[i] > 0)
				{
					//CNOUT("add subsocket to set")
					FD_SET(_client_socket[i], &_readfds);
				}
				if (_client_socket[i] > get_max_sub_socket()) {
					set_max_sub_socket(_client_socket[i]);
				}
			}
		}

			//CNOUT("Selecting...")
			if (select(get_max_sub_socket() + 1, &_readfds, NULL, NULL, NULL) == SYSCALL_ERR) {
				throw Select::fSelectError();
			}
			new_request();
			char	buffer[10025]; // pas sur de l'emplacement
			int		bytes = 0;

			size_total += bytes;
			
			for (int i = 0; i < MAX_CLIENT; i++)
			{
				if (_client_socket[i] != 0 && FD_ISSET(_client_socket[i], &_readfds))
				{
					bytes = recv(_client_socket[i], buffer, 10024, 0);
					// write(1, buffer, bytes);
					if (bytes == SYSCALL_ERR)
					{
						throw Select::fRecvError();
					}
					else if (bytes == 0)
					{
						CNOUT("client disconnected = " << _client_socket[i])
						FD_CLR(_client_socket[i], &_readfds);
						if (_client_socket[i] > 0)
						{
							close(_client_socket[i]);
						}
						//it->set_client_socket(0, i);
						_client_socket[i] = 0;
					}
					else
					{
						size_total += bytes;
						buffer[bytes] = '\0';
						Request *request = new Request(buffer, bytes);




						
						CNOUT("+++++++++++++++++++++++++++++++++++++++++++++++++++++++=")
						write(1, request->get_body().c_str(), bytes);

						//CNOUT(UMAG << request->get_body().c_str() << CRESET)
						CNOUT("+++++++++++++++++++++++++++++++++++++++++++++++++++++++=")
						








						if (request->get_method() == M_POST) {
							CNOUT(BHMAG << "POST--------------------------------------------------------" << CRESET)
							while (bytes > 0) {
								if(_client_socket[i] != 0 && FD_ISSET(_client_socket[i], &_readfds)) {
									for (int i = 0; i < 10025; i++) {
										buffer[i] = '\0';
									}
									bytes = recv(_client_socket[i], buffer, 1024, 0);
									write(1, buffer, bytes);
									if (bytes == SYSCALL_ERR) {
										CNOUT(UMAG << "Error: " << strerror(errno) << CRESET)
										break ;
									} else if (bytes == 0) {
										CNOUT("client disconnected = " << _client_socket[i])
										FD_CLR(_client_socket[i], &_readfds);
										if (_client_socket[i] > 0)
										{
											close(_client_socket[i]);
										}
										//it->set_client_socket(0, i);
										_client_socket[i] = 0;
									} else {
										buffer[bytes] = '\0';
										//request->get_body() += buffer;
										request->add_body(buffer, bytes);
										size_total += bytes;
									}
								} else
									break;
							}
						}

						if (request->get_chunked() == true/*  || request->get_method() == M_POST */) {
							CNOUT(UMAG << "chunked" << CRESET)
							while (bytes > 0) {
								for (int i = 0; i < 10024; i++) {
									buffer[i] = '\0';
								}
								if(_client_socket[i] != 0 && FD_ISSET(_client_socket[i], &_readfds)) {
									bytes = recv(_client_socket[i], buffer, 10024, 0);
									
									if (bytes == SYSCALL_ERR) {
										break ;
									} else if (bytes == 0) {
										FD_CLR(_client_socket[i], &_readfds);
										if (_client_socket[i] > 0)
										{
											close(_client_socket[i]);
										}
										_client_socket[i] = 0;
									} else {
										buffer[bytes] = '\0';
										size_total += bytes;
										request->add_body(buffer, bytes);
									}
								}
								std::string buffer_s(buffer);
								if (buffer_s.find("\0\r\n\r\n") != std::string::npos) {
									break ;
								}
							}
						}
						Response response(*request); // BUG peut etre le pb
						response.manage_response();
						response.set_message_send(response.get_header());
						if (send(_client_socket[i], response.get_message_send().c_str(), response.get_message_send().length(), 0) == SYSCALL_ERR)
						{
							throw Select::fSendError();
						}

						// CNOUT(request->get_body().size())
						// write(1, request->get_body().c_str(), 2124);
					}
				}
			}
		
	}
}

void
INLINE_NAMESPACE::Select::new_request (void) {
	int _new_socket = 0;
	for (socket_type::iterator it = _sockets.begin(); it != _sockets.end(); ++it) {
		if (FD_ISSET(it->get_master_socket(), &_readfds))
		{
			int addrlen = it->get_addrlen();
			if ((_new_socket = accept(it->get_master_socket(), (struct sockaddr *)&(it->get_address()), (socklen_t*)&addrlen)) == SYSCALL_ERR) {
				throw Select::fAcceptError();
			}
			fcntl(_new_socket, F_SETFL, O_NONBLOCK);
		}
	}
	for (int i = 0; i < MAX_CLIENT; i++)
	{
		/*
			la deuxieme conditions permets deviter d'ajouter tj des sockets null pour ensuite rajouter sur le mem index une socket
		*/
		if (_client_socket[i] == 0 && _new_socket != 0) //BUG LA SECONDE CONDITION PEUT VRAIMENT TOUT DEFONCER VRAIMENT PAS SUR DE CETTTE AJOUT ATTENTION GUS
		{
			CNOUT("Adding \'" << _new_socket << "\' to client socket number " << i)
			_client_socket[i] = _new_socket;
			break;
		}
	}
}
