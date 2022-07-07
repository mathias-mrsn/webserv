#include "../incs/webserv.hpp"
#include "../incs/Socket.hpp"

_BEGIN_NAMESPACE_WEBSERV

//!------------------------------CONSTRUCTOR----------------------------------

INLINE_NAMESPACE::Socket::Socket()
{
	_max_clients = MAX_CLIENT;
	_opt = true;

	//Initialize port for Listen_servers
	// for (std::vector<Server>::iterator it = SERVERS.begin(); it != SERVERS.end(); ++it)
	// 	_servers.push_back((*it).get_port());

	//to test mutiple servers
	for (int i = 0; i < NB_SERVER; i++)
	{
		COUT(BRED "YOOOOOOOOOOOOOOOOO" CRESET)
		_servers.push_back(8080 + i);
	}
}

INLINE_NAMESPACE::Socket::Socket(const Socket& copy)
{
	if (this != &copy)
	{
		*this = copy;
	}
}

//!------------------------------DESTRUCTOR-----------------------------------

INLINE_NAMESPACE::Socket::~Socket()
{

}

//!------------------------------OPERATOR-------------------------------------

Socket	&	INLINE_NAMESPACE::Socket::operator=(const Socket& copy)
{
	if (this != &copy)
	{
		(void)copy;
	}
	return(*this);
}

//!------------------------------GETTERS--------------------------------------

std::vector<Listen_server>&	INLINE_NAMESPACE::Socket::get_servers(void)
{
	return (this->_servers);
}

//!------------------------------FUNCTION-------------------------------------


void	INLINE_NAMESPACE::Socket::io_operation(INLINE_NAMESPACE::Listen_server &instance, std::string head_serv, int i)
{
	instance.set_sub_socket(instance.get_client_socket()[i]);
	if (FD_ISSET( instance.get_sub_socket() , &(instance.get_readfds())))
	{
		COUT("DANS IO OPERATION BITCH");
		//Check if it was for closing , and also read the
		//incoming head_serv
		if ((_bytes = recv( instance.get_sub_socket() , instance._buffer, 1024, 0)) == 0)
		{
			//Somebody disconnected , get his details and print
			getpeername(instance.get_sub_socket() , (struct sockaddr*)&instance.get_address() , \
				(socklen_t*)&instance._addrlen);
			printf("Host disconnected , ip %s , port %d \n" ,
				inet_ntoa(instance.get_address().sin_addr) , ntohs(instance.get_address().sin_port));
			//Close the socket and mark as 0 in list for reuse
			close( instance.get_sub_socket() );
			instance.set_client_socket(0, i);
		}
		//Echo back the head_serv that came in
		else
		{
			//set the string terminating NULL byte on the end
			//of the data read
			instance._buffer[_bytes] = '\0';
			std::cout << BBLU << instance._buffer << CRESET << std::endl; //BUG gustave added this line
			//head_serv.assign("HTTP/1.1 200 OK\nContent-Type: text/plain;charset=UTF-8\nContent-Length: 6\n\nsalut ca va!");
			COUT(UGRN << instance.get_sub_socket() << CRESET);
			if( send(instance.get_sub_socket(), head_serv.c_str(), strlen(head_serv.c_str()), 0) != (ssize_t)strlen(head_serv.c_str()) )
			{
			  perror("send");
			}
		}
	}
}

void	INLINE_NAMESPACE::Socket::accept_new_connection(INLINE_NAMESPACE::Listen_server &instance)
{
	//If something happened on the master socket ,
	//then its an incoming connection
	if (FD_ISSET(instance.get_master_socket(), &(instance.get_readfds())))
	{
		if ((_new_socket = accept(instance.get_master_socket(),
			(struct sockaddr *)&instance.get_address(), (socklen_t*)&instance._addrlen))<0)
		{
			perror("accept");
			exit(EXIT_FAILURE);
		}
		//To change socket attribute and render him in non blocking mode
		if(fcntl(_new_socket, F_SETFL, O_NONBLOCK) < 0)
			perror("fcntl");
		//inform user of socket number - used in send and receive commands
		printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , _new_socket , inet_ntoa(instance.get_address().sin_addr) , ntohs
		  (instance.get_address().sin_port));
		//add new socket to array of sockets
		for (int i = 0; i < _max_clients; i++)
		{
			//if position is empty
			if(instance.get_client_socket()[i] == 0 )
			{
				instance.set_client_socket(_new_socket, i);
				printf("Adding to list of sockets as %d\n" , i);	
				break;
			}

		}
	}
}

void	INLINE_NAMESPACE::Socket::manage_socket_set(INLINE_NAMESPACE::Listen_server &instance)
{
 	//clear the socket set
	FD_ZERO(&(instance.get_readfds()));
	//set in non blocking mode
	fcntl(instance.get_master_socket(), F_SETFL, O_NONBLOCK);
	//add master socket to set
	FD_SET(instance.get_master_socket(), &(instance.get_readfds()));
	instance.set_max_sub_socket(instance.get_master_socket());
	//add child sockets to set
	COUT("NB FOIS MANAGE");
	for (int i = 0 ; i < _max_clients ; i++)
	{
		//socket descriptor
		instance.set_sub_socket(instance.get_client_socket()[i]);
		//if valid socket descriptor then add to read list
		if (instance.get_sub_socket() > 0)
		{
			COUT("ON AJOUTE LE SUBSOCKET AU SET BITCH");
			FD_SET( instance.get_sub_socket() , &(instance.get_readfds()));
		}
		  
		//highest file descriptor number, need it for the select function
		if(instance.get_sub_socket() > instance.get_max_sub_socket())
		  instance.set_max_sub_socket(instance.get_sub_socket()); 
	}
}

void	INLINE_NAMESPACE::Socket::run_servers(std::string head_serv)
{
	//accept the incoming connection
	//std::vector<INLINE_NAMESPACE::Listen_server>::iterator it = get_servers().begin();
	while(true)
	{
		for (std::vector<INLINE_NAMESPACE::Listen_server>::iterator it = get_servers().begin(); it !=  get_servers().end(); it++)
		{
			
			(*it)._addrlen = sizeof((*it).get_address());
			manage_socket_set(*it);
			//wait for an activity on one of the sockets , timeout is NULL ,
			//so wait indefinitely
			//TODO Need to implement tmp var to create chuck successfully
			_activity = select( (*it).get_max_sub_socket() + 1 , &((*it).get_readfds()), NULL , NULL , NULL);

			if ((_activity < 0) && (errno!=EINTR))
			  printf("select error");
			accept_new_connection(*it);

			//else its some IO operation on some other socket
			for (int i = 0; i < _max_clients; i++)
			  io_operation(*it, head_serv, i);
			
		}
	}
}

void	INLINE_NAMESPACE::Socket::initialize_socket(INLINE_NAMESPACE::Listen_server &instance)
{
	// [ ] boucle a faire
	//ininstanceialise all client_socket[] to 0 so not checked
	for (int i = 0; i < _max_clients; i++)
		instance.set_client_socket(0, i);
	//create a master socket	
	instance.set_master_socket(socket(AF_INET , SOCK_STREAM , 0));
	if ( instance.get_master_socket() == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	COUT(BBLU << instance.get_master_socket() << CRESET);
	COUT(BBLU << instance.get_port() << CRESET);

	//set master socket to allow multiple connections ,
	//this is just a good habit, instance will work without this
	if ( setsockopt(instance.get_master_socket(), SOL_SOCKET, SO_REUSEADDR, (char *)&_opt,
		sizeof(_opt)) < 0 )
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	//bind the socket to localhost port 8888
	if (bind(instance.get_master_socket(), (struct sockaddr *)&(instance.get_address()), sizeof(instance.get_address()))<0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	printf("Listener on port %d \n", instance.get_port());
	//try to specify maximum of 3 pending connections for the master socket
	if (listen(instance.get_master_socket(), MAX_CONNECTIONS) < 0)
	{
	  perror("listen");
	  exit(EXIT_FAILURE);
	}
}

void		INLINE_NAMESPACE::Socket::initialize_servers()
{
	for (std::vector<INLINE_NAMESPACE::Listen_server>::iterator it = get_servers().begin(); it !=  get_servers().end(); it++)
	{
		initialize_socket(*it);
	}
}

_END_NAMESPACE_WEBSERV

int main(int argc , char *argv[])
{
	(void)argc;
	(void)argv;
	INLINE_NAMESPACE::Socket	server;


	//a head_serv
	//char *head_serv = (char *)"HTTP/1.1 200 OK\nContent-Type: text/html;charset=UTF-8\nContent-Length: 1800\n\n<html>\n<body>\n\n<h2>HTML Buttons</h2>\n<p>HTML buttons are defined with the button tag:</p>\n\n<button>Click me</button>\n\n</body>\n</html>";
	//char *head_serv = (char *) "HTTP/1.1 200 OK\nContent-Type: text/plain;charset=UTF-8\nContent-Length: 12\n\nHello world!";
	std::string head_serv = "HTTP/1.1 200 OK\nContent-Type: text/plain;charset=UTF-8\nContent-Length: 12\n\nHello world!";
	
	//std::string head_serv = "HTTP/1.1 200 OK\nContent-Type: text/html;charset=UTF-8\nContent-Length: 1800\n\n<html>\n<body>\n\n<h2>HTML Buttons</h2>\n<p>HTML buttons are defined with the button tag:</p>\n\n<button>Click me</button>\n\n</body>\n</html>";

	server.initialize_servers();
	server.run_servers(head_serv);

	return 0;
}
