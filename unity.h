#ifndef UNITY_H
#define UNITY_H
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#define OPEN_MAX 100
#define BACKLOG 5
#define SERV_PORT 8888
#define INFTIM 1000
#define MAXEVT 20
#define MAXFD 256
#define MAXCMD 10
#define BUFFLEN 1024
#define CLICENT_NUM 20
#define PRE 4

void getFileName(int num, char* filename)
{
	memset(filename, 0, sizeof(filename));
	filename[0] = 'a' + num;
	filename[1] = '.', filename[2] = 'c';
}

void readRes(int s)
{
	char buff[BUFFLEN];
	memset(buff, 0, BUFFLEN);
	recv(s, buff, BUFFLEN, 0);
	printf("%s\n", buff);
}

void writeReq(int s, char *str)
{
	char buff[BUFFLEN];
	memset(buff, 0, BUFFLEN);
	strcpy(buff, str);
	send(s, buff, strlen(buff), 0);
}

void showSpeed(double kb)
{
	if(kb < 1)
	{
		printf("%.1lf Byte/s\n", kb * 1024);
	}
	if(kb < 1024)
	{
		printf("%.1lf Kb/s\n", kb);
		return ;
	}
	if(kb < 1024 * 1024)
	{
		printf("%.1lf Mb/s\n", kb / 1024);
		return ;
	}
	if(kb < 1024 * 1024 * 1024)
	{
		printf("%.1lf Gb/s\n", kb / 1024 / 1024);
	}
}

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

void TransFile(const char* filename, int clicent_socket)
{
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
	    printf("%.2lf%%   ", (double)100 * sum_size / size);
	    showSpeed((double)sum_size / 1024.0 / (timer * 1e-6));

        bzero(buff, BUFFLEN);
        if(sum_size == size) break;
    }
	fclose(file);
	printf("File transfer completed!\n");
}

void recvFile(const char* filename, int s)
{
    FILE *file = fopen(filename, "wb");
    int n = 0;
    char buffer[BUFFLEN];
    bzero(buffer, BUFFLEN);
    int sum_size = 0;

    struct timeval start, end;
    gettimeofday(&start, NULL);

    while((n = recv(s, buffer, BUFFLEN, 0)) > 0)
    {
        fwrite(buffer, 1, n, file);

        gettimeofday(&end, NULL);
        sum_size += n;

        double timer = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
        printf("%.2lfkb Recived  ", (double)sum_size / 1024);
        showSpeed((double)sum_size / 1024.0 / (timer * 1e-6));

        bzero(buffer, BUFFLEN);
        if(n < BUFFLEN)
    	{
    		break;
    	}
    }
	fclose(file);
    printf("Recive completed!\n");
}


#endif