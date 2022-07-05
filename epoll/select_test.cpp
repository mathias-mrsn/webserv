//Example code: A simple server side code, which echos back the received message.
//Handle multiple socket connections with select and fd_set on Linux
#include <stdio.h>
#include <string.h> //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h> //close
#include <arpa/inet.h> //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#include <iostream>
#include "../colors.h"

#define TRUE 1
#define FALSE 0
#define PORT 8888
#define MAX_CONNECTIONS 100
#define MAX_CLIENT 30

/* std::string	find_path(char * buffer)
{
	std::string path = "";
	int i = 5;
	while (buffer[i] != ' ')
	{
		path += buffer[i];
		i++;
	}
	return path;

} 

// ATTENTION IL NE FAUT PAS LE PREMIER / DANS LE PATH
std::string	read_open(std::string path)
{
	//std::cout << BRED << path << CRESET << std::endl;
	std::string content = "";
	std::ifstream file(path.c_str());
	if (file.is_open())
	{
		std::string line;
		while (getline(file, line))
		{
			content += line + "\n";
		}
		file.close();
	}
	else
	{
		content = "Error 404: File not found";
	}
	//std::cout << BRED << content << CRESET ;
	return content;
} */

int main(int argc , char *argv[])
{
	(void)argc;
	(void)argv;
	int opt = TRUE;
	int master_socket , addrlen , new_socket , client_socket[MAX_CLIENT] ,
		max_clients = MAX_CLIENT , activity, i , valread , sd;
	int max_sd;
	struct sockaddr_in address;
	
	char buffer[1025]; //data buffer of 1K
	
	//set of socket descriptors
	fd_set readfds;
	
	//a message
	//char *message = (char *)"ECHO Daemon v1.0 \r\n";
	//char *message = (char *)"HTTP/1.1 200 OK\nContent-Type: text/html;charset=UTF-8\nContent-Length: 1800\n\n<html>\n<body>\n\n<h2>HTML Buttons</h2>\n<p>HTML buttons are defined with the button tag:</p>\n\n<button>Click me</button>\n\n</body>\n</html>";
	char *message = (char *) "HTTP/1.1 200 OK\nContent-Type: text/plain;charset=UTF-8\nContent-Length: 12\n\nHello world!";

	//initialise all client_socket[] to 0 so not checked
	for (i = 0; i < max_clients; i++)
	{
		client_socket[i] = 0;
	}
	
	//create a master socket
	if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	//set master socket to allow multiple connections ,
	//this is just a good habit, it will work without this
	if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
		sizeof(opt)) < 0 )
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	//type of socket created
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( PORT );

	//bind the socket to localhost port 8888
	if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)
	{
	  perror("bind failed");
	  exit(EXIT_FAILURE);
	}
	printf("Listener on port %d \n", PORT);
	
	//try to specify maximum of 3 pending connections for the master socket
	if (listen(master_socket, MAX_CONNECTIONS) < 0)
	{
	  perror("listen");
	  exit(EXIT_FAILURE);
	}
	
	//accept the incoming connection
	addrlen = sizeof(address);
	puts("Waiting for connections ...");
	
	while(TRUE)
	{
	  //clear the socket set
	  FD_ZERO(&readfds);

	  //add master socket to set
	  FD_SET(master_socket, &readfds);
	  max_sd = master_socket;
	
	  //add child sockets to set
	  for ( i = 0 ; i < max_clients ; i++)
	  {
	    //socket descriptor
	    sd = client_socket[i];
	
	    //if valid socket descriptor then add to read list
	    if(sd > 0)
	      FD_SET( sd , &readfds);
	
	    //highest file descriptor number, need it for the select function
	    if(sd > max_sd)
	      max_sd = sd;
	  }

	  //wait for an activity on one of the sockets , timeout is NULL ,
	  //so wait indefinitely
	  activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);

	  if ((activity < 0) && (errno!=EINTR))
	  {
	    printf("select error");
	  }
	
	  //If something happened on the master socket ,
	  //then its an incoming connection
	  if (FD_ISSET(master_socket, &readfds))
	  {
	    if ((new_socket = accept(master_socket,
	        (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
	    {
	      perror("accept");
	      exit(EXIT_FAILURE);
	    }
	
	    //inform user of socket number - used in send and receive commands
	    printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , new_socket , inet_ntoa(address.sin_addr) , ntohs
	      (address.sin_port));
	
	    //send new connection greeting message
	    if( send(new_socket, message, strlen(message), 0) != (ssize_t)strlen(message) )
	    {
	      perror("send");
	    }
	
	    puts("Welcome message sent successfully");
	
	    //add new socket to array of sockets
	    for (i = 0; i < max_clients; i++)
	    {
	      //if position is empty
	      if( client_socket[i] == 0 )
	      {
	        client_socket[i] = new_socket;
	        printf("Adding to list of sockets as %d\n" , i);
	
	        break;
	      }
	    }
	  }
	(void)valread; // BUG a virer dici
	(void)buffer; // BUG a virer dici
	/* 
		!enfaite le code ci-dessous va permettre de recuperer les retours des clients (header) par exemple,
		!tu peux tester avec un client tel que localhost:8888/example_html/form.html
		! tu vas donc recuperer l'adress uri passer par le client et tu vas l'utiliser pour recuperer le contenu de la page
		! tu peux aussi voir que en retour il y a deux headers, a voir si avec firefox il y a en un ou plus 
		! le deuxieme header correspond a: favicon.ico qui est enfaite une icone symbolisant le site
	*/
	
	 //else its some IO operation on some other socket
	 for (i = 0; i < max_clients; i++)
	 {
	   sd = client_socket[i];
	
	   if (FD_ISSET( sd , &readfds))
	   {
	     //Check if it was for closing , and also read the
	     //incoming message
	     if ((valread = read( sd , buffer, 1024)) == 0)
	     {
	       //Somebody disconnected , get his details and print
	       getpeername(sd , (struct sockaddr*)&address , \
	         (socklen_t*)&addrlen);
	       printf("Host disconnected , ip %s , port %d \n" ,
	         inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
	
	       //Close the socket and mark as 0 in list for reuse
	       close( sd );
	       client_socket[i] = 0;
	     }
	
	     //Echo back the message that came in
	     else
	     {
	       //set the string terminating NULL byte on the end
	       //of the data read
	       buffer[valread] = '\0';
			//send(sd , buffer , strlen(buffer) , 0 );
			std::cout << BBLU << buffer << CRESET << std::endl; //BUG gustave added this line
	     }
	   }
	 }
	}
	
	return 0;
}
