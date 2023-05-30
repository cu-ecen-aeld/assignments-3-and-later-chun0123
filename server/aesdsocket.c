#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <syslog.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define BUFFER_SIZE 1024
#define PORT "9000"

const char *DATA_FILE = "/var/tmp/aesdsocketdata";

int sock_fd= 0;
int server_fd = 0;
ssize_t count = 0;
struct addrinfo *servinfo = NULL;
FILE *out_file = NULL;
FILE *client_data = NULL;

volatile sig_atomic_t stop_server = 0;

// Signal handler for SIGINT and SIGTERM
void signal_handler(int signal)
{
	if (signal == SIGINT || signal == SIGTERM)
	{
		syslog(LOG_INFO, "Caught signal, exiting");
		stop_server = 1;
	}
	if (client_data)
	{
		fclose(client_data);
	}
	if (server_fd)
	{
		close(server_fd);
	}
	if (out_file)
	{
		fclose(out_file);
	}
	if (sock_fd)
	{
		close(sock_fd);
	}
	if (servinfo)
	{
		freeaddrinfo(servinfo);
	}
	remove(DATA_FILE);
	exit(0);
}


int main(int argc, char *argv[])
{
	int status;
	char buffer[BUFFER_SIZE];
	
	struct addrinfo hints;
	struct sockaddr their_addr;
	socklen_t sin_size;
	struct sigaction sa = {0};
	const int option=1;
	
	char *input = NULL;
	size_t len = 0;
	size_t size;

	// Register signal handler for SIGINT and SIGTERM
	sa.sa_handler = signal_handler; 
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);
	
	// Create a socket
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo("localhost", PORT, &hints, &servinfo);

	if ((sock_fd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
	{
		return (-1);
	}

	setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

	// Bind socket to port
	if ((status = bind(sock_fd, servinfo->ai_addr, servinfo->ai_addrlen)) != 0)
	{
		return (-1);
	}

	// Check if -d argument is present and fork daemon
	if (argc == 2 && strcmp(argv[1], "-d") == 0 && fork()) 
	{
		exit(0);
	}
	
	// Listen for incoming connections
	if (listen(sock_fd, 10) == -1) 
	{
		perror("Listen for connection");
		exit(1);
	}

	while(1) 
	{
		// Accept incoming connection
		sin_size = sizeof their_addr;
		server_fd = accept(sock_fd, (struct sockaddr *)&their_addr, &sin_size);
		if (server_fd == -1) 
		{
			perror("Accept connection");
 			continue;
		}

		struct sockaddr_in *client_addr = (struct sockaddr_in *)&their_addr;
 		syslog(LOG_INFO, "Accepted connection from %s\n", inet_ntoa(client_addr->sin_addr));

		client_data = fdopen(server_fd, "rb");

		// Receive data and append to file
		if(client_data)
		{
			input = NULL;
			len = 0;
			if ((count = getline(&input, &len, client_data)) != -1)
			{
				out_file = fopen(DATA_FILE, "a+");

				fputs(input, out_file);
				fflush(out_file);
				free(input);	
                   
				size = 0;

				// Send response
				fseek(out_file, 0, SEEK_SET);
				while ((size = fread(buffer, sizeof(char), BUFFER_SIZE, out_file)) > 0)
				send(server_fd, buffer, size, 0);
				fclose(out_file);
				out_file = NULL;
					
				fclose(client_data);
				client_data = NULL;
				close(server_fd);
				server_fd = 0;
				syslog(LOG_INFO, "Closed connection from %s\n", inet_ntoa(client_addr->sin_addr));	
			}
		}
	}
	
	syslog(LOG_INFO, "Exiting");
	return 0;
}
