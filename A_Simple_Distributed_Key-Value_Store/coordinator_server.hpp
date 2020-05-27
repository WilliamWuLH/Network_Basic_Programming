#ifndef _CO_SERVER_
#define _CO_SERVER_

#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <vector>
#include <pthread.h>
#include <semaphore.h>
#include "Command_handle.hpp"

using namespace std;

/*
    * @author: WilliamWuLH
    * @version: 2020.5.24
    * @see: Coordinator
    * @see: Command
    * Coordinator 类, 做协调者的工作
*/

class Coordinator{
    private:
        char IP[100];
        char PORT[50];
        int fd_coordinator;
   	    struct sockaddr_in coordinator_addr;

        int participant_num;
        vector<string> participant_ip;
        vector<string> participant_port;

        string LOG;
    
    public:
        Coordinator() {
            string PS_IP = "127.0.0.1";
            string PS_PORT = "8888";
            strcpy(IP, PS_IP.c_str());
            strcpy(PORT, PS_PORT.c_str());
            LOG = "";
        };

        Coordinator(string ip, string port){
            strcpy(IP, ip.c_str());
            strcpy(PORT, port.c_str());
            cout <<IP <<endl;
            cout <<PORT <<endl;
            LOG += "Coordinator IP : " + ip + "\r\n";
            LOG += "Coordinator PORT : " + port + "\r\n";
        }

        void init();

        void insert_participant(int num, vector<string> &pip, vector<string> &pport);

        void work();

        string send_participant(int status, string msg);
};

#endif