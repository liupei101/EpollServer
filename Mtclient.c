#include <pthread.h>
#include "unity.h"
#define BASE_NUM 9000
#define CLIENT_MAX_NUM 20

int portVal[CLIENT_MAX_NUM];
// for initialiaze
void init()
{
	int i;
	for(i = 0;i < CLIENT_MAX_NUM;i ++) portVal[i] = i;
	
	srand(time(NULL));
}

void *client_run(void *args)
{
	int i, threads_id = *((int*)args);
	struct sockaddr_in server, local;
	char buff[BUFFLEN];

	int client_socket = socket(AF_INET, SOCK_STREAM, 0);

	memset(&local, 0, sizeof(local));	
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = htonl(INADDR_ANY);
	local.sin_port = htons(BASE_NUM + threads_id);

	printf("thread : %d | PORT : %d\n", threads_id, BASE_NUM + threads_id);

	bind(client_socket, (struct sockaddr*)&local, sizeof(local));

	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(SERV_PORT);
    
	connect(client_socket, (struct sockaddr*)&server, sizeof(server));

	// welcome msg!
	readRes(client_socket);

	sleep(RAND_INT());

	// send something!
	writeReq(client_socket, "USER liupei");
	readRes(client_socket);

	sleep(RAND_INT());

	writeReq(client_socket, "PASS 12345");
	readRes(client_socket);

	sleep(RAND_INT());
	writeReq(client_socket, "RETR");

	char filename[STR_MAX_LEN];
	getFileName(threads_id, filename);

	recvFile(filename, client_socket);

	sleep(RAND_INT());
	close(client_socket);

	pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
	pthread_t pt[CLIENT_MAX_NUM];
	int i, n;
	scanf("%d", &n);
	if(n > CLIENT_MAX_NUM) n = CLIENT_MAX_NUM;

	init();

	for(i = 0;i < n;i ++)
	{
		int ret = pthread_create(&pt[i], NULL, client_run, &portVal[i]);
		if(ret != 0)
		{
			puts("threads creating failed!");
		}
	}

	for(i = 0;i < n;i ++)
	{
		pthread_join(pt[i], NULL);
	}

	return 0;
}