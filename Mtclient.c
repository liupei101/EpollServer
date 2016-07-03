#include "unity.h"

void client_run(int num)
{
	int i;
	struct sockaddr_in server;
	char buff[BUFFLEN];

	int s = socket(AF_INET, SOCK_STREAM, 0);	

	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(SERV_PORT);
    
	connect(s, (struct sockaddr*)&server, sizeof(server));

	// welcome msg!
	readRes(s);

	// send something!
	writeReq(s, "USER xxx");
	readRes(s);

	writeReq(s, "PASS 12345");
	readRes(s);

	writeReq(s, "RETR");

	char filename[10];
	getFileName(num, filename);

	recvFile(filename, s);

	close(s);
}

int main(int argc, char const *argv[])
{
	int i, n;
	scanf("%d", &n);
	if(n > 26) n = 26;
	for(i = 0;i < n;i ++)
	{
		client_run(i);
	}
	// close(s);
	return 0;
}