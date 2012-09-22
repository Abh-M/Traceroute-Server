//
//  ServerSoc.cpp
//  traceroute
//
//  Created by Abhineet on 07/09/12.
//  Copyright (c) 2012 Abhineet. All rights reserved.
//


#include "ServerSoc.h"
#include <string.h>
#include "Command.h"
#include "Globals.h"
#include <semaphore.h>
#include <ctime>
#include <time.h>
#include "Logger.h"
#include <sys/signal.h>

int remCon=0;
int maxCon;
sem_t mutex;

typedef struct connectionDetails {
    int connFD;
    int maxRate;
    int isStrictOn;
    struct sockaddr_in clientAddress;
    
}params;

typedef struct countdownDetails
{
    pthread_t parentThread;
    int socketDescriptor;
    time_t startTime;
    int port_no;
    char ipAddress[15];
}countdownParams;

static Socket *sharedInstance = NULL;

void sendMessageToClient(char *message,int clientSocketDes)
{
    char reply[1024];
    memset(&reply, 0, sizeof(reply));
    strcat(reply,message);
    size_t replySize = (size_t)sizeof(reply);
    if((send(clientSocketDes,reply,replySize, 0))==-1)
        cout<<"\nError sending message";
        
}


void traceRoute(char *command,int clientSocketDes)
{
    char message[1024];
    strcpy(message, SEPERATOR);
    sendMessageToClient(message, clientSocketDes);
    FILE *fp = popen(command, "r");
    char line[1024];
    char reply[1024];
    while (fgets(line, sizeof(line), fp)) {
        cout<<line<<"\n";
        strcpy(reply, line);
        strcat(reply, "\n");
        sendMessageToClient(reply, clientSocketDes);
    }
    pclose(fp);
    sendMessageToClient(message, clientSocketDes);
}




//Countdown to time out and then kill the parent thread
void *countdown(void *arg)
{
    
    countdownParams details = *(countdownParams*)arg;
    double diff = difftime(time(NULL), details.startTime);
    while (diff<TIMEOUT_INTERVAL) {
        diff = difftime(time(NULL), details.startTime);
        cout<<"\n Diff..... "<<diff<<"...."<<details.socketDescriptor;
    }
    
    if(diff>=TIMEOUT_INTERVAL)
    {
        char mess[1024];
        strcpy(mess, "Connection Time Out");
        sendMessageToClient(mess, details.socketDescriptor);
        close(details.socketDescriptor);
        
        sem_wait(&mutex);
        if(remCon<maxCon)
            remCon++;
        cout<<"\n REMAINING CONNECTIONS : "<<remCon;
        sem_post(&mutex);

        int result = pthread_cancel(details.parentThread);
        if(result==0)
            automaticTimeOutLog(details.ipAddress, details.port_no);
        
    }
    pthread_exit(NULL);

    
    //countdown till 30 if the thread is not killed the it will terminate the client connection and the thread
    return NULL;
}




 void * handleRequest(void *arg)
{
    params details = *(params*)arg;
    int connFD = details.connFD;
    int numberOfTraceroutes = 0;
    char *ipaddress =(char*)malloc(15);
    int isStrictOn = details.isStrictOn;
    ipaddress = inet_ntoa(details.clientAddress.sin_addr);
    //LOG
    clientConnectedLog(ipaddress,details.clientAddress.sin_port);
    time_t startTime = NULL;
    char mess[1024];
    
    strcpy(mess, "\n>>> ");
    sendMessageToClient(mess, connFD);
    

    pthread_t countThread;
    countdownParams connDetails;
    strcpy(connDetails.ipAddress, ipaddress);
    connDetails.port_no = details.clientAddress.sin_port;
    connDetails.socketDescriptor = connFD;
    connDetails.parentThread = pthread_self();
    connDetails.startTime = time(NULL);

    pthread_create(&countThread, NULL, countdown,(void*)&connDetails);


    
    while (1) {
        char buff[1024];
        size_t buffSize = (size_t)sizeof(buff);
        memset(&buff, 0,buffSize);
        recv(connFD, &buff, buffSize,0);
        
        cout<<"\n Client "<<connFD<<" : "<<buff;

        if(buff==NULL || strlen(buff)<=0){ continue;}
        Command *cmd = new Command(buff);

        if(strcmp(cmd->command, "help")==0)
        {
            //kill previous timer
            if((pthread_cancel(countThread))!=0)
                cout<<"\n Cannot kill thread";

            //Send help file
            FILE *fp = fopen("/Users/abhineet/Desktop/traceroute/traceroute/help.txt", "r");

            char line[1024];
            //char reply[1024];
            while (fgets(line, sizeof(line), fp)) {
                //cout<<line<<"\n";
                strcpy(mess, line);
                strcat(mess, "\n");
                sendMessageToClient(mess, connFD);


            }
         
            
            //start new timer
            connDetails.startTime = time(NULL);
            pthread_create(&countThread, NULL, countdown, (void*)&connDetails);

        }
        else if(strcmp(cmd->command, "traceroute")==0)
        {
            //kill previous timer
            if((pthread_cancel(countThread))!=0)
                cout<<"\n Cannot kill thread";

            
            char *tracerouteCommands[100];
            int totalTracerouteCommands=0;
            

            
            /*determine command type*/
            if(strcmp(cmd->args[0], "me")==0)
            {
                //This is traceroute me command
                char *command = (char*)malloc(1024);
                strcat(command, cmd->command);
                strcat(command, " ");
                strcat(command, ipaddress);
                tracerouteCommands[totalTracerouteCommands++]=command;

            }
            else if((strstr(cmd->args[0],".trt"))!=NULL)
            {
                char pathPrefix[1024];
                strcpy(pathPrefix, "/Users/abhineet/Desktop/traceroute/traceroute/");

                
                char *filname = strcat(pathPrefix, cmd->args[0]);

                FILE *batchFile = fopen(filname, "r");
                if(batchFile!=NULL)
                {
                    char line[1024];
                    while (fgets(line, sizeof(line), batchFile)) {

                        Command *c = new Command(line);
                        
                        char *command = (char*)malloc(1024);
                        strcat(command, c->command);
                        strcat(command, " ");
                        strcat(command, c->args[0]);
                        tracerouteCommands[totalTracerouteCommands++]=command;
                    }
                    fclose(batchFile);
                }
                else{
                    //File does not exist
                    //Send error message to client
                    char reply[1024];
                    strcat(reply, "File not found");
                    sendMessageToClient(reply, connFD);
                }
            }
            else
            {
                
                //normal traceroute
                char *command = (char*)malloc(1024);
                strcat(command, cmd->command);
                strcat(command, " ");
                strcat(command, cmd->args[0]);
                if(isStrictOn && (strcmp(cmd->args[0], "me"))!=0)
                {
                    //LOG
                    strictviolatedLog(ipaddress, details.clientAddress.sin_port, command);
                    char mess[1024];
                    strcpy(mess, "\nCannot telnet to host other than yourself\n");
                    sendMessageToClient(mess, connFD);
                }
                else
                    tracerouteCommands[totalTracerouteCommands++]=command;
                    


                
            }
            
             
            int index=0;
            while(totalTracerouteCommands>0)
            {
                if(numberOfTraceroutes==0)
                {
                    startTime = time(NULL);
                    
                }
                
                
                double diffInSec = difftime(time(NULL), startTime);
                
                
                
                if((numberOfTraceroutes+1<=details.maxRate) && (diffInSec<=60))
                {
                    numberOfTraceroutes++;
                    cout<<"\n Executing "<<numberOfTraceroutes<<"  "<<diffInSec/60<<":"<<(int)diffInSec%60;
                    //LOG
                    simpleTrtLog(ipaddress,cmd->args[0],details.clientAddress.sin_port);
                    traceRoute(tracerouteCommands[index++], connFD);
                    
                }
                else if ((numberOfTraceroutes+1>details.maxRate) && (diffInSec<=60))
                {
                    totalTracerouteCommands=1;
                    strcpy(mess, "Rate limit excedded");
                    sendMessageToClient(mess, connFD);
                    rateLimitExceededLog(ipaddress, details.clientAddress.sin_port, tracerouteCommands[index]);
                }
                else if((numberOfTraceroutes+1>details.maxRate) && (diffInSec>60))
                {
                    numberOfTraceroutes=0;
                    startTime=time(NULL);
                    cout<<"\n Executing "<<numberOfTraceroutes<<"  "<<diffInSec/60<<":"<<(int)diffInSec%60;
                    //LOG
                    simpleTrtLog(ipaddress,cmd->args[0],details.clientAddress.sin_port);
                    traceRoute(tracerouteCommands[index++], connFD);

                }
                else if((numberOfTraceroutes+1<=details.maxRate) && (diffInSec>60))
                {
                 
                    numberOfTraceroutes=0;
                    startTime=time(NULL);
                    cout<<"\n Executing "<<numberOfTraceroutes<<"  "<<diffInSec/60<<":"<<(int)diffInSec%60;
                    //LOG
                    simpleTrtLog(ipaddress,cmd->args[0],details.clientAddress.sin_port);
                    traceRoute(tracerouteCommands[index++], connFD);
                }
                else{
                    
                }

            
            
                
                totalTracerouteCommands--;
                                                  
            }
            
            //start new timer
            connDetails.startTime = time(NULL);
            pthread_create(&countThread, NULL, countdown, (void*)&connDetails);

        


        }
        else if(strcmp(cmd->command, "quit")==0)
        {
            
            
            //close connection
            strcpy(mess, "terminate");
            sendMessageToClient(mess, connFD);

            //LOG
            clientDisconnetedLog(ipaddress,details.clientAddress.sin_port);
            if(close(connFD)==0)
              cout<<"\nClient "<<connFD<<" closed";
            else
                cout<<"\n Error closing client"<<connFD;
            
            sem_wait(&mutex);
            remCon = (remCon+1>=maxCon)?maxCon:++remCon;
            cout<<"\n REMAINING CONNECTIONS : "<<remCon;

            sem_post(&mutex);

            pthread_exit((void*)&connFD);


        }
        else
        {
            //kill previous timer
            if((pthread_cancel(countThread))!=0)
                cout<<"\n Cannot kill thread";

            strcpy(mess, "\nInvalid Command\n");
            sendMessageToClient(mess, connFD);
            
            
            //start new timer
            connDetails.startTime = time(NULL);
            pthread_create(&countThread, NULL, countdown, (void*)&connDetails);




        }


        strcpy(mess, "\n>>> ");
        sendMessageToClient(mess, connFD);

        
        
    }
    

    return NULL;
}


int Socket::createSocket()

{
    this->portNumber = PORT_NUMBER;
    this->maximum_users = MAX_USERS;
    this->reqPerMinPerUser = REQ_PER_SEC;
    this->strictDestination = STRICT_DEST;
    this->sock_fd = socket(PF_INET, SOCK_STREAM, 0);
    return (this->sock_fd==-1)?NO:YES;
}

int Socket::bindSocket()
{
 
    memset(&this->bindAddress,0, sizeof(this->bindAddress));
    this->bindAddress.sin_port = htons(this->portNumber);
    this->bindAddress.sin_addr.s_addr = INADDR_ANY;
    this->bindAddress.sin_family = AF_INET;
    int result = bind(this->sock_fd, (struct sockaddr*)&this->bindAddress, sizeof(this->bindAddress));
    return (result==-1)?NO:YES;
    
}

void Socket::startServerProcess()
{

    remCon = this->maximum_users;
    maxCon = this->maximum_users;
    if(this->bindSocket() == YES)
    {
        //Log server started
        serverStartedLog();
        while((listen(this->sock_fd, this->maximum_users))==0)
        {

            struct sockaddr_in tempAddress;
            memset(&tempAddress,0, sizeof(tempAddress));
            params details;

            socklen_t sockAddrLen = sizeof(tempAddress);
            int connFD = accept(this->sock_fd, (struct sockaddr*)&tempAddress,&sockAddrLen);
            if(connFD!=-1 && remCon>0)
            {
                            
                sem_wait(&mutex);
                    remCon--;
                cout<<"\n REMAINING CONNECTIONS : "<<remCon;

                sem_post(&mutex);
                

                details.connFD = connFD;
                details.maxRate = this->reqPerMinPerUser;
                details.clientAddress = tempAddress;
                details.isStrictOn = this->strictDestination;
                
                pthread_create(&pids[totalThreads], NULL, handleRequest,(void*)&details);
            }
            else if(connFD!=-1 && remCon<=0)
            {
             
                //LOG
                simultaneousConnectionLimitExccededLog(inet_ntoa(details.clientAddress.sin_addr),details.clientAddress.sin_port);
                close(connFD);
            }
                
        }
        
        for (int i=0; i<totalThreads; i++) {
            pthread_join(pids[i], NULL);
        }
    }
        
    
}

Socket* Socket::shared()
{
    
    sharedInstance = (sharedInstance==NULL)?(new Socket()):sharedInstance;
    sharedInstance->totalThreads = 0;
    sem_init(&mutex, 0, 1);
    return sharedInstance;
}

