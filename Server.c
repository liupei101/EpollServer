#include "unity.h"
#include "userManager.h"

#define SEND_FILE_PATH "../downloads/file.c"
#define N 1024

struct user_info user[N];
void userInfoInit()
{
	int i;
	for(i = 0;i < N;i ++)
	{
		init(&user[i]);
	}
}

void doWarning(int clicent_socket)
{
	char buff[BUFFLEN];
	memset(buff, 0, sizeof(buff));
	sprintf(buff, "Welcome ID(%d): Connection completed, Please Login!", clicent_socket);
	send(clicent_socket, buff, strlen(buff), 0);
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
		if(!isLogin(&user[clicent_socket])) 
		{
			doWarning(clicent_socket);
			return ;
		}
		// transFile to client
		TransFile(SEND_FILE_PATH, clicent_socket);
	}
	else if(strcmp(arg1, "USER") == 0)
	{
		printf(">>> FROM Client(%d) : %s\n", clicent_socket, buff);
		if(strlen(arg2) == 0)
		{
			sprintf(msg, "Input Error, No username!");
			send(clicent_socket, msg, strlen(msg), 0);
			return ;
		}
		setRecvUsername(&user[clicent_socket], arg2);
		sprintf(msg, "Password for %s", arg2);
		send(clicent_socket, msg, strlen(msg), 0);
	}
	else if(strcmp(arg1, "PASS") == 0)
	{
		printf(">>> FROM Client(%d) : %s\n", clicent_socket, buff);
		// No checking for password!
		if(!isRecvUserName(&user[clicent_socket]))
		{
			sprintf(msg, "Input Error, No Response!");
			send(clicent_socket, msg, strlen(msg), 0);
			return ;
		}
		decrypt(arg2, KEY);
		if(checkoutPassword(&user[clicent_socket], arg2))
		{
			setLogin(&user[clicent_socket]);
			sprintf(msg, "Login successfully!");
			send(clicent_socket, msg, strlen(msg), 0);
		}
		else 
		{
			sprintf(msg, "Username or Password Error!");
			send(clicent_socket, msg, strlen(msg), 0);
		}
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
		init(&user[clicent_socket]);
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
	struct sockaddr_in clientaddr;
	socklen_t socklen = sizeof(struct sockaddr_in);

	int listenfd = (*ev).data.fd;
	int clicent_socket = accept(listenfd, (struct sockaddr*)&clientaddr, &socklen);
	setnonblocking(clicent_socket);

	(*ev).data.fd = clicent_socket;
	epoll_ctl(epfd, EPOLL_CTL_ADD, clicent_socket, ev);

	// for new connection , send "Welcome"
	if(isReset(&user[clicent_socket])) doWarning(clicent_socket);
}

int main()
{
	int i, listenfd, connfd, sockfd, epfd, nfds;
	userInfoInit();

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	setnonblocking(listenfd);

	struct epoll_event ev, events[MAXEVT];

	epfd = epoll_create(MAXFD);
	ev.data.fd = listenfd;
	ev.events = EPOLLIN | EPOLLET;
	epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);

	struct sockaddr_in serveraddr;

	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERV_PORT);
	bind(listenfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	listen(listenfd, BACKLOG);

	printf("***** Server is running *****\n\n");

	while(1)
	{
		// when a connection (create | sendMsg | close), epoll_wait can check it!
		nfds = epoll_wait(epfd, events, MAXEVT, -1);
		printf("Connections: %d\n", nfds);
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