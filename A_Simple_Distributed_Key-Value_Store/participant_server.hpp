#ifndef _PA_SERVER_
#define _PA_SERVER_

#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <map>
#include "Command_handle.hpp"
using namespace std;

/*
    * @author: WilliamWuLH
    * @version: 2020.5.25
    * @see: Participant
    * @see: Command
    * Participant 类, 做参与者的工作
*/

#define BUFFSIZE 2048

class Participant
{
    private:
        char IP[100];
        char PORT[50];
        int fd_participant;
   	    struct sockaddr_in participant_addr;

        map<string, string> KVstore;

        string LOG;
    
    public:
        Participant(){
            string PS_IP = "127.0.0.1";
            string PS_PORT = "8888";
            strcpy(IP, PS_IP.c_str());
            strcpy(PORT, PS_PORT.c_str());
            LOG = "";
        }

        Participant(string ip, string port){
            strcpy(IP, ip.c_str());
            strcpy(PORT, port.c_str());
            cout <<IP <<endl;
            cout <<PORT <<endl;
            LOG = "";
            LOG += "Participant IP : " + ip + "\r\n";
            LOG += "Participant PORT : " + port + "\r\n";
        }

        void init();

        void work();
};

#endif