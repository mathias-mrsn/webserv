/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Select.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamaurai <mamaurai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/18 11:30:22 by mamaurai          #+#    #+#             */
/*   Updated: 2022/07/23 17:17:55 by mamaurai         ###   ########.fr       */
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
	
	for (FOREACH_SERVER) {
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
	fd_set r_readfds;
	int		accept_fd = 0;
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
			char	buffer[1025]; // pas sur de l'emplacement
			int		bytes = 0;
			for (int i = 0; i < MAX_CLIENT; i++)
			{
				if (_client_socket[i] != 0 && FD_ISSET(_client_socket[i], &_readfds))
				{
					bytes = recv(_client_socket[i], buffer, 1024, 0);
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
						//print
						buffer[bytes] = '\0';
						CNOUT(BBLU << buffer << CRESET)
						Request *request = new Request(buffer);
						// CNOUT(*request);
						//send et il y aura du parsing ici
						// std::string hello = "HTTP/1.1 200 OK\nContent-Type: text/html;charset=UTF-8\nContent-Length: 1800\n\n";
						std::string hello = "HTTP/1.1 200 OK\nContent-Type: text/plain;charset=UTF-8\nContent-Length: 12\n\nHello world!<h1>HELLO</h1>";
						// COUT("\n yess= " << find_path(buffer))
						hello.append(read_open(find_path(buffer)));
						// COUT(BRED << hello << CRESET)
						//if (send(_client_socket[i], str.c_str(), str.length(), 0) == SYSCALL_ERR)
						if (send(_client_socket[i], hello.c_str(), hello.length(), 0) == SYSCALL_ERR)
						{
							throw Select::fSendError();
						}
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
