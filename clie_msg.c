#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define MAX_LINE	4096
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define RESET   "\033[0m"


void	ft_chat(int connfd)
{
	char	buff_rd[MAX_LINE];
	char	buff_wr[MAX_LINE];
	int		br;

	while ((br = read(connfd, buff_rd, MAX_LINE)))
	{
		write(STDOUT_FILENO, buff_rd, br);
		if (fgets(buff_wr, sizeof(buff_wr), stdin))
			write(connfd, buff_wr, strlen(buff_wr));
	}
}

int	main(int argc, char *argv[])
{
	int						sockfd;
	struct sockaddr_in		servaddr;

	if (argc != 3)
	{
		write(STDERR_FILENO, RED "modo de uso do >_Daemon: ./servidor [porta] [ip]" RESET "\n", 59);
		exit(EXIT_FAILURE);
	}
	sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(argv[1]));
	inet_pton(AF_INET, argv[2], &servaddr.sin_addr);
	if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
			perror("em connect: conexão recusada!");
			exit(EXIT_FAILURE);
	}
	ft_chat(sockfd);
	close(sockfd);
	exit(EXIT_SUCCESS);
}
