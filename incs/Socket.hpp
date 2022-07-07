#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "webserv.hpp"

#define TRUE 1
#define FALSE 0
#define PORT 8888
#define MAX_CONNECTIONS 100
#define MAX_CLIENT 30
#define NB_SERVER 2

/*Constructor*/
_BEGIN_NAMESPACE_WEBSERV

class Socket // : public Listen_server
{
	public:
		Socket();
		Socket(const Socket & copy);

		~Socket();

		Socket & operator=(const Socket & copy);

		void	initialize_socket(INLINE_NAMESPACE::Listen_server &instance); // Initialize one server
		void	initialize_servers(void); // initialise all of the servers
		void	run_servers(std::string head_serv); // Run one serveur

		void	manage_socket_set(INLINE_NAMESPACE::Listen_server &instance);
		void	accept_new_connection(INLINE_NAMESPACE::Listen_server &instance);
		void	io_operation(INLINE_NAMESPACE::Listen_server &instance, std::string head_serv, int i);
		std::vector<Listen_server>&	get_servers(void);

	private:
		int		_opt;
		//int		_master_socket;
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
		//struct	sockaddr_in	_address;
		std::vector<Listen_server>	_servers; //TODO a enlever juste pour les test
};

_END_NAMESPACE_WEBSERV

#endif