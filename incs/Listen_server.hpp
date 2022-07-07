#ifndef LISTEN_SERVER_HPP
#define LISTEN_SERVER_HPP

#include "webserv.hpp"

#define MAX_CLIENT 30

_BEGIN_NAMESPACE_WEBSERV

class	Listen_server
{
	public:
		Listen_server(int port);
		Listen_server(void);
		~Listen_server(void);

		int		&get_port(void);
		int		&get_master_socket(void);
		int		*get_client_socket(void);
		int		&get_sub_socket(void);
		int		&get_max_sub_socket(void);
		struct	sockaddr_in	& get_address(void);
		fd_set	&get_readfds(void);
		
		void	set_master_socket(int new_socket);
		void	set_client_socket(int client_socket, int index);
		void	set_sub_socket(int sub_socket);
		void	set_max_sub_socket(int max_sub_socket);

		char	_buffer[1025];
		int		_addrlen;
	private:
		int					 _port;
		int					_master_socket;
		int					_client_socket[MAX_CLIENT];
		int					_sub_socket;
		int					_max_sub_socket;
		struct sockaddr_in	_address;
		fd_set	_readfds;

};

_END_NAMESPACE_WEBSERV

#endif