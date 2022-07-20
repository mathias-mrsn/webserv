/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Select.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gmary <gmary@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/18 11:30:22 by mamaurai          #+#    #+#             */
/*   Updated: 2022/07/20 16:05:08 by gmary            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

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
		r_readfds = _readfds;
		//BUG ici ajoute function
		//socket_type::iterator it = _sockets.begin();
		FD_ZERO(&_readfds);
		for (socket_type::iterator it = _sockets.begin(); it != _sockets.end(); ++it) {
			// FD_ZERO(&_readfds);
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

			CNOUT("Selecting...")
			if (select(get_max_sub_socket() + 1, &_readfds, NULL, NULL, NULL) == SYSCALL_ERR) {
				throw Select::fSelectError();
			}
			new_request();
			char	buffer[1025]; // pas sur de l'emplacement
			int		bytes = 0;
			for (int i = 0; i < MAX_CLIENT; i++)
			{
				//it->set_sub_socket(_client_socket[i]);
				// if (FD_ISSET(it->get_sub_socket(), &r_readfds))
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
						//[ ] pq utiliser FD_CLR ?????
						if (_client_socket[i] > 0)
						{
							//FD_CLR(it->get_sub_socket(), &_readfds);
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
						//send et il y aura du parsing ici
						std::string str = "HTTP/1.1 200 OK\nContent-Type: text/plain;charset=UTF-8\nContent-Length: 12\n\nHello world!";
						if (send(_client_socket[i], str.c_str(), str.length(), 0) == SYSCALL_ERR)
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
			//fcntl(it->get_sub_socket(), F_SETFL, O_NONBLOCK); //BUG TODO surement faire fcntl de new_socket plustot que de sub socket
			fcntl(_new_socket, F_SETFL, O_NONBLOCK);
		}
	}
	for (int i = 0; i < MAX_CLIENT; i++)
	{
		if (_client_socket[i] == 0)
		{
			CNOUT("Adding \'" << _new_socket << "\' to client socket number " << i)
			_client_socket[i] = _new_socket;
			break;
		}
	}
}

/* void
INLINE_NAMESPACE::Select::new_request (Socket & it) {
	if (FD_ISSET(it.get_master_socket(), &_readfds))
	{
		int addrlen = it.get_addrlen();
		int _new_socket;
		if ((_new_socket = accept(it.get_master_socket(), (struct sockaddr *)&(it.get_address()), (socklen_t*)&addrlen)) == SYSCALL_ERR) {
			throw Select::fAcceptError();
		}
		fcntl(it.get_sub_socket(), F_SETFL, O_NONBLOCK);
		for (int i = 0; i < MAX_CLIENT; i++)
		{
			if (it.get_client_socket(i) == 0)
			{
				CNOUT("Adding \'" << _new_socket << "\' to client socket number " << i)
				it.set_client_socket(_new_socket, i);
				break;
			}
		}
	}
} */