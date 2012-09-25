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


#define PATH_PREFIX "/Users/abhineet/Desktop/traceroute/traceroute/"
#define HELP_FILE "help.txt"
#define LOG_FILE "server_log.log"

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



#define PROMPT "\n>> "
#define TIMEOUT_INTERVAL 600
#define END_OF_MESSAGE  ""//"kEndofMessage"

#endif
