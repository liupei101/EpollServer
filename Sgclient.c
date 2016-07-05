#include "unity.h"

#define RECV_FILE_PATH "../downloads/filex.c"

void handleFileTransfer(int s, char *cmd)
{
	char buff[BUFFLEN];
	memset(buff, 0, sizeof(buff));

	writeReq(s, "AUTH");
	recv(s, buff, BUFFLEN, 0);
	if(strcmp(buff, "SACK") == 0) // hava auth for RETR cmd
	{
		writeReq(s, "RETR");
		recvFile(RECV_FILE_PATH, s);
	}
	else if(strcmp(buff, "SNAK") == 0)
	{
		printf("Server: no authorization!\n");
	}
	else ;
}

int main(int argc, char const *argv[])
{
	struct sockaddr_in server;
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(SERV_PORT);

	int s = socket(AF_INET, SOCK_STREAM, 0);	
	connect(s, (struct sockaddr*)&server, sizeof(server));

	// welcome msg!
	readRes(s);

	char cmd[BUFFLEN];
	while(1)
	{
		printf("Client >>> ");
		gets(cmd);
		if(strcmp(cmd, "EXIT") == 0) break;
		if(strcmp(cmd, "RETR") == 0)
		{
			handleFileTransfer(s, cmd);
			continue;
		}

		writeReq(s, cmd);
		printf("Server: ");
		readRes(s);
	}

	sleep(1);
	close(s);

	return 0;
}