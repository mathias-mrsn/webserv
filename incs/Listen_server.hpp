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
	protected:
		int _port;
		int	_master_socket;
		struct sockaddr_in	_address;
};

_END_NAMESPACE_WEBSERV

#endif