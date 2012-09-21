//
//  Globals.h
//  traceroute
//
//  Created by Abhineet on 07/09/12.
//  Copyright (c) 2012 Abhineet. All rights reserved.
//


#include <stdio.h>

#ifndef traceroute_Globals_h
#define traceroute_Globals_h


#define PORT_NUMBER 1216
#define MAX_USERS 2
#define STRICT_DEST 0
#define REQ_PER_SEC 4
#define TINY_BUFF 56
#define SMALL_BUFF 128
#define LARGE_BUFF 256
#define BIG_BUFF 512


#define ARG_PORT "--port"
#define ARG_STRICT_DESTINATION "--strict"
#define ARG_REQUEST_RATE  "--rate"
#define ARG_MAX_UESERS "--max_users"


#define SUCCESS 1
#define FAILURE 0

#define LOG_FILE "/Users/abhineet/Desktop/traceroute/traceroute/server_log.log"

#define SEPERATOR "\n-----------------------------------------------------------\n"
#define INVALID_COMMAND "\nInvalid Command\n"

typedef enum{
    TRCT_HOST_OR_IP,
    TRCT_ME,
    TRCT_FILE,
    HELP,
    QUIT,
    INVALID,
}CommandType;



typedef enum
{
    server_start=0,
    server_stop,
    
    traceroute_details,
    traceroute_too_many_req,
    traceroute_file,
    
    client_connected,
    client_disconnected,
    client_terminated,
    
    too_many_connections,
    strict_voilated
    
    
}EVENT;

#define TIMEOUT_INTERVAL 20

#endif
