#ifndef _CLIENTTASK_
#define _CLIENTTASK_

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <sys/wait.h>
using namespace std;

#define BUFFSIZE 2048

class ClientTask
{
    private:
        int fd_client;
    
    public:
        ClientTask() {}

        ClientTask(int conn){
            fd_client = conn;
        }

        ~ClientTask() {}

	    void doit();

        void response(int status);

	    void response_get(string path);

	    void response_post(string Name, string ID);
};

#endif