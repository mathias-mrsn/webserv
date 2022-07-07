
#include "../incs/Listen_server.hpp"

INLINE_NAMESPACE::Listen_server::Listen_server(int port) :
 _port(port)
{
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
