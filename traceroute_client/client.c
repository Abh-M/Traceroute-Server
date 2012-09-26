//
//  main.c
//  demo
//
//  Created by Abhineet on 20/08/12.
//  Copyright (c) 2012 Abhineet. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <memory.h>



#define PORT 1216
int terminateSenderThread = -1;



void  *sendThread(void *arg)
{
	int clientFD = *(int*)arg;
	char command[512];


	while (1) {
		memset(&command, 0, sizeof(command));
		fgets(command, sizeof(command),stdin);
		command[strlen(command)-1]='\0';

		ssize_t len = (ssize_t)sizeof(command);
		ssize_t res = send(clientFD,&command,len,0);
		if(res==-1){printf("\nError in sending");}


	}

	return NULL;
}







int main(int argc, const char * argv[])
{

	int port_no = PORT;
	char ip_addr[15];
	int use_localhost = 1;

	const char *seperator = "=";
	int i;
	for(i=1;i<argc;i++)
		//0 is command
	{
		const char *arg = argv[i];
		char *param = strtok((char *)arg, seperator);
		char *val = NULL;
		while ((val = strtok(NULL, seperator))!=NULL) {

			if(strcmp(param, "ip")==0)
				strcpy(ip_addr,val);
			else if((strcmp(param, "port"))==0)
			{
				port_no = atoi(val);
				use_localhost = 0;
			}
		}
	}

	int clientFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	struct sockaddr_in sockAddr;
	pthread_t rthread,sthread;
	void *rstatus = NULL;
	if (clientFD!=-1) {
		memset(&sockAddr, 0, sizeof(sockAddr));
		sockAddr.sin_family = AF_INET;
		sockAddr.sin_port = htons(port_no);
		if(!use_localhost)
			inet_pton(AF_INET,ip_addr,&(sockAddr.sin_addr.s_addr));
		else if(use_localhost)
			sockAddr.sin_addr.s_addr = INADDR_ANY;
		printf("Socket created");
	}
	else
	{
		exit(1);
	}


	int res = connect(clientFD, (struct sockaddr*)&sockAddr, sizeof(sockAddr));
	if(res>=0)
	{
		printf("\nConnected to server");

		pthread_create(&sthread, NULL, sendThread, &clientFD);
		char buff[1024];
		int len=0;
		ssize_t bytes = 0;
		int byte_count = -1;
		while (1) {

			//			size_t buffSize = (size_t)sizeof(buff);
			//			memset(&buff, 0,buffSize);
			//			len = (size_t)sizeof(buff);
			//			bytes = recv(clientFD, &buff,len, 0);
			//
			//			if(strlen(buff)>0)
			//				printf("%s",buff);
			//			fflush(stdout);
			//			if((strcmp(buff, "terminate"))==0 || (strcmp(buff, "\nConnection Time Out")==0))
			//			{
			//				pthread_cancel(sthread);
			//				break;
			//			}

			memset(buff, 0, sizeof(buff));


			if( (byte_count = recv (clientFD, buff, sizeof(buff), MSG_WAITALL)) == -1)
			{
				printf("Error receiving msg");
			}
			else
			{
				buff[byte_count]='\0';

				if((strcmp(buff, "terminate"))==0 || (strcmp(buff, "\nConnection Time Out")==0))
				{
					pthread_cancel(sthread);
					break;
				}
				printf("%s",buff);
				fflush(stdout);


			}
		}
		close(clientFD);
		printf("\nShutting down");

		return 0;
	}
}



