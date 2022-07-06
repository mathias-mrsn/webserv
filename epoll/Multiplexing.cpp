#include "../incs/Multiplexing.hpp"

//!------------------------------CONSTRUCTOR----------------------------------

Multiplexing::Multiplexing()
{
	//type of socket created
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = INADDR_ANY;
	_address.sin_port = htons( PORT );

	_max_clients = MAX_CLIENT;
	_opt = true;
}

Multiplexing::Multiplexing(const Multiplexing& copy)
{
	if (this != &copy)
	{
		*this = copy;
	}
}

//!------------------------------DESTRUCTOR-----------------------------------

Multiplexing::~Multiplexing()
{

}

//!------------------------------OPERATOR-------------------------------------

Multiplexing	&	Multiplexing::operator=(const Multiplexing& copy)
{
	if (this != &copy)
	{
		(void)copy;
	}
	return(*this);
}

//!------------------------------FUNCTION-------------------------------------

void	Multiplexing::initialize_server()
{
	//initialise all client_socket[] to 0 so not checked
	for (int i = 0; i < _max_clients; i++)
		_client_socket[i] = 0;
	//create a master socket
	if ( (_master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	//set master socket to allow multiple connections ,
	//this is just a good habit, it will work without this
	if ( setsockopt(_master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&_opt,
		sizeof(_opt)) < 0 )
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	//bind the socket to localhost port 8888
	if (bind(_master_socket, (struct sockaddr *)&_address, sizeof(_address))<0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	printf("Listener on port %d \n", PORT);
	//try to specify maximum of 3 pending connections for the master socket
	if (listen(_master_socket, MAX_CONNECTIONS) < 0)
	{
	  perror("listen");
	  exit(EXIT_FAILURE);
	}
}

void	Multiplexing::manage_socket_set()
{
 	//clear the socket set
	FD_ZERO(&_readfds);
	//add master socket to set
	FD_SET(_master_socket, &_readfds);
	_max_sd = _master_socket;
	//add child sockets to set
	for (int i = 0 ; i < _max_clients ; i++)
	{
		//socket descriptor
		_sd = _client_socket[i];
		//if valid socket descriptor then add to read list
		if(_sd > 0)
		  FD_SET( _sd , &_readfds);
		//highest file descriptor number, need it for the select function
		if(_sd > _max_sd)
		  _max_sd = _sd;
	}
}

void	Multiplexing::accept_new_connection()
{
	//If something happened on the master socket ,
	//then its an incoming connection
	if (FD_ISSET(_master_socket, &_readfds))
	{
		if ((_new_socket = accept(_master_socket,
			(struct sockaddr *)&_address, (socklen_t*)&_addrlen))<0)
		{
			perror("accept");
			exit(EXIT_FAILURE);
		}	
		//inform user of socket number - used in send and receive commands
		printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , _new_socket , inet_ntoa(_address.sin_addr) , ntohs
		  (_address.sin_port));
		//add new socket to array of sockets
		for (int i = 0; i < _max_clients; i++)
		{
			//if position is empty
			if( _client_socket[i] == 0 )
			{
				_client_socket[i] = _new_socket;
				printf("Adding to list of sockets as %d\n" , i);	
				break;
			}
		}
	}
}

void	Multiplexing::io_operation(std::string message, int i)
{
	_sd = _client_socket[i];
	if (FD_ISSET( _sd , &_readfds))
	{
		//Check if it was for closing , and also read the
		//incoming message
		if ((_valread = recv( _sd , _buffer, 1024, 0)) == 0)
		{
			//Somebody disconnected , get his details and print
			getpeername(_sd , (struct sockaddr*)&_address , \
				(socklen_t*)&_addrlen);
			printf("Host disconnected , ip %s , port %d \n" ,
				inet_ntoa(_address.sin_addr) , ntohs(_address.sin_port));
			//Close the socket and mark as 0 in list for reuse
			close( _sd );
			_client_socket[i] = 0;
		}
		//Echo back the message that came in
		else
		{
			//set the string terminating NULL byte on the end
			//of the data read
			_buffer[_valread] = '\0';
			//send(sd , buffer , strlen(buffer) , 0 );
			std::cout << BBLU << _buffer << CRESET << std::endl; //BUG gustave added this line
			//message.assign("HTTP/1.1 200 OK\nContent-Type: text/plain;charset=UTF-8\nContent-Length: 6\n\nsalut ca va!");
			if( send(_sd, message.c_str(), strlen(message.c_str()), 0) != (ssize_t)strlen(message.c_str()) )
			{
			  perror("send");
			}
		}
	}
}

void	Multiplexing::run_server(std::string message)
{
	//accept the incoming connection
	_addrlen = sizeof(_address);
	while(TRUE)
	{
		manage_socket_set();
		//wait for an activity on one of the sockets , timeout is NULL ,
		//so wait indefinitely
		_activity = select( _max_sd + 1 , &_readfds , NULL , NULL , NULL);

		if ((_activity < 0) && (errno!=EINTR))
		  printf("select error");
		accept_new_connection();

		//else its some IO operation on some other socket
		for (int i = 0; i < _max_clients; i++)
		  io_operation(message, i);
	}
}

int main(int argc , char *argv[])
{
	(void)argc;
	(void)argv;
	Multiplexing	server;

	//a message
	//char *message = (char *)"HTTP/1.1 200 OK\nContent-Type: text/html;charset=UTF-8\nContent-Length: 1800\n\n<html>\n<body>\n\n<h2>HTML Buttons</h2>\n<p>HTML buttons are defined with the button tag:</p>\n\n<button>Click me</button>\n\n</body>\n</html>";
	//char *message = (char *) "HTTP/1.1 200 OK\nContent-Type: text/plain;charset=UTF-8\nContent-Length: 12\n\nHello world!";
	std::string message = "HTTP/1.1 200 OK\nContent-Type: text/plain;charset=UTF-8\nContent-Length: 12\n\nHello world!";
	
	//std::string message = "HTTP/1.1 200 OK\nContent-Type: text/html;charset=UTF-8\nContent-Length: 1800\n\n<html>\n<body>\n\n<h2>HTML Buttons</h2>\n<p>HTML buttons are defined with the button tag:</p>\n\n<button>Click me</button>\n\n</body>\n</html>";

	server.initialize_server();
	server.run_server(message);

	return 0;
}
