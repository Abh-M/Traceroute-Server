//
//  Logger.cpp
//  traceroute
//
//  Created by Abhineet on 16/09/12.
//  Copyright (c) 2012 Abhineet. All rights reserved.
//

#include "Logger.h"
#include "Globals.h"
#include <stdlib.h>
#include <cstring>
using namespace std;
FILE *log;


char* getTimeStamp()
{

    time_t currTime = time(NULL);
    struct tm *now = localtime(&currTime);
    char *timeString = (char *)malloc(80);
    strftime(timeString, 80, "%m/%d/%y %H:%M:%S", now);
    return timeString;
}


int initLogger()
{
    log = NULL;
    log = fopen(LOG_FILE, "a");
    return (log==NULL)?FAILURE:SUCCESS;
    

}

void serverStartedLog()
{
    if(initLogger())
    {
        char *logMessage = (char*)malloc(256);
        strcat(logMessage, SEPERATOR);
        strcat(logMessage, getTimeStamp());
        strcat(logMessage, " : ");
        strcat(logMessage, "Server started");
        if((fprintf(log, "\n%s",logMessage)<0))
            cout<<"Error logging";
        fclose(log);
    }
}

void clientConnectedLog(char *ipaddress,int port_no)
{
    if(initLogger())
    {
        if((fprintf(log, "\n%s : Client Connected | IP: %s | Port: %d ",getTimeStamp(),ipaddress,port_no)<0))
            cout<<"Error logging";
        fclose(log);
    }
}
void simpleTrtLog(char *ipaddress,char* ipOrhostname, int port_no)
{
    if(initLogger())
    {
        if((fprintf(log, "\n%s : Traceroute %s | from IP: %s PORT: %d ",getTimeStamp(),ipOrhostname,ipaddress,port_no)<0))
            cout<<"Error logging";
        fclose(log);
    }

}

void clientDisconnetedLog(char *ipaddress, int port_no)
{
    if(initLogger())
    {
        if((fprintf(log, "\n%s : Client Disconnected | from IP: %s PORT: %d ",getTimeStamp(),ipaddress,port_no)<0))
            cout<<"Error logging";
        fclose(log);
    }
    

}
void clinetForceDisconnectLog(char *ipaddress, int port_no)
{
    
}
void simultaneousConnectionLimitExccededLog(char *ipaddress,int port_no)
{
    if(initLogger())
    {
        if((fprintf(log, "\n%s : Maximum connections excceded  | Disconnecting | Client IP: %s PORT: %d ",getTimeStamp(),ipaddress,port_no)<0))
            cout<<"Error logging";
        fclose(log);
    }

    
}
void strictviolatedLog(char *ipaddress, int port_no, char *command)
{
    if(initLogger())
    {
        if((fprintf(log, "\n%s : Strict violated  | Client IP: %s PORT: %d COMMAND: %s",getTimeStamp(),ipaddress,port_no,command)<0))
            cout<<"Error logging";
        fclose(log);
    }

    
}


void automaticTimeOutLog(char *ipaddress, int port_no)
{
    if(initLogger())
    {
        if((fprintf(log, "\n%s : Connection Timeout  | Client IP: %s PORT: %d",getTimeStamp(),ipaddress,port_no)<0))
            cout<<"Error logging";
        fclose(log);
    }
    
    
}

void rateLimitExceededLog(char *ipaddress, int port_no, char *command)
{
    if(initLogger())
    {
        if((fprintf(log, "\n%s : Rate Limit Exceeded  | Client IP: %s PORT: %d COMMAND: %s",getTimeStamp(),ipaddress,port_no,command)<0))
            cout<<"Error logging";
        fclose(log);
    }

}