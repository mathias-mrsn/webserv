#ifndef MULTIPLEXING_HPP
#define MULTIPLEXING_HPP

#include "webserv.hpp"

#define TRUE 1
#define FALSE 0
#define PORT 8888
#define MAX_CONNECTIONS 100
#define MAX_CLIENT 30

class Multiplexing
{
	public:
		Multiplexing();
		Multiplexing(const Multiplexing & copy);

		~Multiplexing();

		Multiplexing & operator=(const Multiplexing & copy);

		void	initialize_server(void);
		void	run_server(std::string head_serv);
		void	manage_socket_set(void);
		void	accept_new_connection(void);
		void	io_operation(std::string head_serv, int i);

	private:
		int		_opt;
		int		_master_socket;
		int		_addrlen;
		int		_new_socket;
		int		_client_socket[MAX_CLIENT];
		int		_max_clients;
		int		_activity;
		int		_bytes;
		int		_sub_socket;
		int		_max_sub_socket;
		char	_buffer[1025];
		fd_set	_readfds;
		struct	sockaddr_in	_address;
};

#endif