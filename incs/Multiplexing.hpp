#ifndef MULTIPLEXING_HPP
#define MULTIPLEXING_HPP

#include "webserv.hpp"

#define TRUE 1
#define FALSE 0
#define PORT 8888
#define MAX_CONNECTIONS 100
#define MAX_CLIENT 30
#define NB_SERVER 2

/*Constructor*/

class	my_serv
{
	public:
		my_serv(int port);
		my_serv(void);
		~my_serv(void);

		int _port;
		int	_master_socket;
};

my_serv::my_serv(int port) : _port(port)
{
	std::cout << "Constructor my_serv called" << std::endl;
}

my_serv::my_serv(void)
{
	std::cout << "Default Constructor my_serv called" << std::endl;
}

my_serv::~my_serv()
{
	std::cout << "Destructor my_serv destructor called" << std::endl;
}

class Multiplexing : public my_serv
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
		void	function_a_virer(void); //TODO a virer

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
		std::vector<my_serv> servers; //TODO a enlever juste pour les test
};


#endif