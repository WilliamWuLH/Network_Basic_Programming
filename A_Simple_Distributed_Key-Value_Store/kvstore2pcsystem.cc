#include <iostream>
#include <fstream>
#include <sstream>
#include <getopt.h> //getopt_long()头文件位置
#include <vector>
#include "participant_server.hpp"
#include "coordinator_server.hpp"

using namespace std;

/*
    * @author: WilliamWuLH
    * @version: 2020.5.24
    * @see: Coordinator
    * @see: Participant
    * 主函数，判断执行 Coordinator, 还是 Participant
*/

static int max_participant_num = 1000;

int main(int argc, char** argv){
    string config_path = "";
    string mode;
    string coordinator_ip;
    string coordinator_port;
    int participant_num = 0;
    vector<string> participant_ip(max_participant_num);
    vector<string> participant_port(max_participant_num);

    const char *optstring="n:v";
    int c, index;
    struct option opts[]={{"config_path",required_argument,NULL,'p'},
                          {0,0,0,0}};

    while((c=getopt_long(argc,argv,optstring,opts,&index))!=-1)
    {
        switch(c)
        {
        case 'p':
            // printf("config_path : %s\n",optarg);
            config_path = optarg;
            break;
        default:
            printf("c is %d\n",c);
            break;
        }
    }

    fstream f(config_path);
    string line;
    if( f ){
        while( getline(f, line) ){
            if(line.size() > 0 && line[0] != '!'){
                stringstream s;
                string msg;
                s << line;
                while(s >> msg){
                    if(msg == "mode"){
                        s >> mode;
                    }
                    else if(msg == "coordinator_info"){
                        s >> msg;
                        int i=0;
                        while(msg[i] != ':'){
                            coordinator_ip += msg[i];
                            i++;
                        }
                        i++;
                        while(i != (int)msg.length()){
                            coordinator_port += msg[i];
                            i++;
                        }
                    }
                    else if(msg == "participant_info"){
                        s >> msg;
                        int i=0;
                        while(msg[i] != ':'){
                            participant_ip[participant_num] += msg[i];
                            i++;
                        }
                        i++;
                        while(i != (int)msg.length()){
                            participant_port[participant_num] += msg[i];
                            i++;
                        }
                        participant_num++;
                    }
                }
            }
        }
    }
    else{
        cout <<"ERROR" <<endl;
    }
    // cout <<"mode : " <<mode <<endl;
    // cout <<"coordinator_ip : " <<coordinator_ip <<endl;
    // cout <<"coordinator_port : " <<coordinator_port <<endl;
    // for(int i=0; i<participant_num; i++){
    //     cout <<"participant_ip : " <<participant_ip[i] <<endl;
    //     cout <<"participant_port : " <<participant_port[i] <<endl;
    // }
    

    if(mode == "coordinator"){
        // cout <<"coordinator working" <<endl;
        Coordinator c1(coordinator_ip, coordinator_port);
        c1.init();
        c1.insert_participant(participant_num, participant_ip, participant_port);
        c1.work();
    }
    else if(mode == "participant"){
        // cout <<"participant working" <<endl;
        Participant p1(participant_ip[0], participant_port[0]);
        p1.init();
        p1.work();
    }

}