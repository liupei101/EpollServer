#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#define OPEN_MAX 100
#define BACKLOG 5
#define SERV_PORT 8888
#define INFTIM 1000
#define MAXEVT 20
#define MAXFD 256
#define BUFFLEN 1024
#define PRE 4
#define MAXCMD 10

void setnonblocking(int s_s)
{
	int opts = fcntl(s_s, F_GETFL);
	if(opts < 0)
	{
		perror("Set NonBlocking Error!");
		return ;
	}

	opts |= O_NONBLOCK;
	if(fcntl(s_s, F_SETFL, opts) < 0)
	{
		perror("Set opts Error!");
		return ;
	}
}
void dispatchCmd(const char buff[], char *s1, char *s2)
{
	int i, len = strlen(buff);
	memset(s1, 0, sizeof s1);
	memset(s2, 0, sizeof s2);

	for(i = 0;i < PRE;i ++) s1[i] = buff[i];
	i ++;
    for(;i < len;s2[i - PRE - 1] = buff[i], i ++) ;
}
void TransFile(int clicent_socket)
{
	char *filename = "test.c";
	FILE *file = fopen(filename, "rb");
	fseek(file, 0, 2);
	long size = ftell(file);
	fseek(file, 0, 0);
	int n = 0;
	char buff[BUFFLEN];
	bzero(buff, BUFFLEN);
	int sum_size = 0;

    struct timeval start, end;
    gettimeofday(&start, NULL);

    while((n = fread(buff, 1, BUFFLEN, file)) > 0) 
    {
        send(clicent_socket, buff, n, 0);

		gettimeofday(&end, NULL);
        sum_size += n;

	    double timer = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
	    printf("%.1lf%%    %.1lf kb/s\n", (double)100 * sum_size / size, (double)sum_size / 1024.0 / (timer * 1e-6));

        bzero(buff, BUFFLEN);
        if(sum_size == size) break;
    }
	fclose(file);
	printf("File transfer completed!\n");
}
void doResponse(int clicent_socket, const char buff[])
{
	// client request for downloading file
	char msg[BUFFLEN], arg1[MAXCMD], arg2[MAXCMD];
	memset(msg, 0, sizeof(msg));

	dispatchCmd(buff, arg1, arg2);

	if(strcmp(arg1, "RETR") == 0)
	{
		printf(">>> FROM Client(%d) : %s\n", clicent_socket, buff);
		// transFile to client
		TransFile(clicent_socket);
	}
	else if(strcmp(arg1, "USER") == 0)
	{
		printf(">>> FROM Client(%d) : %s\n", clicent_socket, buff);
		sprintf(msg, "password for %s: ", arg2);
		send(clicent_socket, msg, strlen(msg), 0);
	}
	else if(strcmp(arg1, "PASS") == 0)
	{
		printf(">>> FROM Client(%d) : %s\n", clicent_socket, buff);
		// No checking for password!
		sprintf(msg, "Login successfully!");
		send(clicent_socket, msg, strlen(msg), 0);
	}
	else 
	{
		printf(">>> FROM Client(%d) : %s\n", clicent_socket, buff);
		sprintf(msg, "Invalid command!\n");
		send(clicent_socket, msg, strlen(msg), 0);
	}
}

void handle_message(int epfd, struct epoll_event* ev)
{
	char buff[BUFFLEN];
	memset(buff, 0, sizeof(buff));

	int clicent_socket = (*ev).data.fd;
	int len = recv(clicent_socket, buff, BUFFLEN, 0);

	if(len <= 0)
	{
		printf("client(%d): Connect Error or closed!\n", clicent_socket);
		// delete invalid connection from epoll
		epoll_ctl(epfd, EPOLL_CTL_DEL, clicent_socket, ev);
		close(clicent_socket);
	}
	else 
	{
		doResponse(clicent_socket, buff);
	}
}
void handle_connect(int epfd, struct epoll_event* ev)
{
	char buff[BUFFLEN];
	struct sockaddr_in clientaddr;
	socklen_t socklen = sizeof(struct sockaddr_in);
	memset(buff, 0, sizeof(buff));

	int listenfd = (*ev).data.fd;
	int clicent_socket = accept(listenfd, (struct sockaddr*)&clientaddr, &socklen);
	setnonblocking(clicent_socket);

	(*ev).data.fd = clicent_socket;
	epoll_ctl(epfd, EPOLL_CTL_ADD, clicent_socket, ev);

	// for new connection , send "Welcome"
	sprintf(buff, "ID(%d): Connection completed, Welcome!", clicent_socket);
	send(clicent_socket, buff, strlen(buff), 0);
}
int main()
{
	int i, listenfd, connfd, sockfd, epfd, nfds;
	char buff[BUFFLEN];
	
	struct epoll_event ev, events[MAXEVT];
	epfd = epoll_create(MAXFD);

	struct sockaddr_in serveraddr;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	setnonblocking(listenfd);

	ev.data.fd = listenfd;
	ev.events = EPOLLIN | EPOLLET;

	epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);

	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERV_PORT);

	bind(listenfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	listen(listenfd, BACKLOG);

	printf("Server is running\n\n");

	while(1)
	{
		// when a connection (create | sendMsg | close), epoll_wait can check it!
		nfds = epoll_wait(epfd, events, MAXEVT, -1);
		printf("Now connecting number: %d\n", nfds);
		for(i = 0;i < nfds;i ++)
		{
			if(events[i].data.fd == listenfd)
			{
				handle_connect(epfd, &events[i]);
			}
			else 
			{
				handle_message(epfd, &events[i]);
			}
		}
	}
	return 0;

}
