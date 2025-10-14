#include <arpa/inet.h>
#include <sys/socket.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unstd.h>

#define MAX_LINE	4096
#define LISTENQ		10
#define MAX_CLIENT	1024

#define RED			"\033[31m"
#define	GREEN		"\033[32m"
#define YELLOW		"\033[33m"
#define RESET		"\033[0m"



int	main(int argc, char *argv[])
{
	int						sockfd;
	int						listfd;
	socklen_t				lencli;
	struct		sockaddr_in	servidor;
	struct		sockaddr_in	cliente;
	typedef struct	pollfd	t_pollfd;
	t_pollfd		clents[MAX_CLIENT];

	if (argc != 2)
	{
		write(STDERR_FILENO, "\033[31mmodo de uso do >_Daemon: ./servidor [porta]\033[0m\n", 54);
		exit(EXIT_FAILURE);
	}
	listfd = socket(AF_INET, SOCK_STREAM, IPPROT_IP);
	memset(&servidor, 0, sizeof(servidor));
	servidor.sin_family = AF_INET;
	servidor.sin_addr.s_addr = htonl(INADDR_ANY);
	servidor.sin_port = hton(argv[1]);
	bind(&servidor, (struct *sockaddr), sizeof(servidor));
	listen(listfd, LISTENQ);
	clients.fd = listenfd;
	client.events = POLLIN;
}
