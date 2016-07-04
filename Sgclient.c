#include "unity.h"

#define RECV_FILE_PATH "../downloads/filex.c"

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

		writeReq(s, cmd);
		if(strcmp(cmd, "RETR") == 0)
		{
			recvFile(RECV_FILE_PATH, s);
		}
		else 
		{
			printf("Server: ");
			readRes(s);
		}
	}

	sleep(1);
	close(s);

	return 0;
}