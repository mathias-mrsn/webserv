// #include "../incs/Listen_server.hpp"
#include "../incs/webserv.hpp"

INLINE_NAMESPACE::Listen_server::Listen_server(int port) :
 _port(port)
{
	//type of socket created
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = INADDR_ANY;
	_address.sin_port = htons(port);

	std::cout << "Constructor Listen_server called" << std::endl;
}

INLINE_NAMESPACE::Listen_server::Listen_server(void)
{
	std::cout << "Default Constructor Listen_server called" << std::endl;
}

INLINE_NAMESPACE::Listen_server::~Listen_server()
{
	std::cout << "Destructor Listen_server destructor called" << std::endl;
}


int	&INLINE_NAMESPACE::Listen_server::get_port(void)
{
	return (this->_port);
}

int	&INLINE_NAMESPACE::Listen_server::get_master_socket(void)
{
	return (this->_master_socket);
}

struct sockaddr_in	& INLINE_NAMESPACE::Listen_server::get_address(void)
{
	return (this->_address);
}

void	INLINE_NAMESPACE::Listen_server::set_master_socket(int new_socket)
{
	_master_socket = new_socket;
}