#ifndef LISTEN_SERVER_HPP
#define LISTEN_SERVER_HPP

#include "webserv.hpp"

_BEGIN_NAMESPACE_WEBSERV

class	Listen_server
{
	public:
		Listen_server(int port);
		Listen_server(void);
		~Listen_server(void);

		int		get_port(void);
		int		get_master_socket(void) const;
		struct	sockaddr_in	& get_address(void);
		
		void	set_master_socket(int new_socket);
	protected:
		int _port;
		int	_master_socket;
		struct sockaddr_in	_address;
};

_END_NAMESPACE_WEBSERV

#endif