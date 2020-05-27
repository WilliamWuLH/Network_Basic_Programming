#include "coordinator_server.hpp"

/*
    * @author: WilliamWuLH
    * @version: 2020.5.24
    * @see: Coordinator
    * 初始化 Coordinator，建立 socket，绑定 ip 和 port
*/

void Coordinator::init(){
    coordinator_addr.sin_family = PF_INET;
    coordinator_addr.sin_port = htons(atoi(PORT));
    coordinator_addr.sin_addr.s_addr = inet_addr(IP);
    // cout <<coordinator_addr.sin_addr.s_addr <<endl;
    // cout <<coordinator_addr.sin_port <<endl;

    //创建socket
    fd_coordinator = socket(PF_INET, SOCK_STREAM, 0);
    if(fd_coordinator < 0){
        printf("socket error\n");
        exit(1);
    }

    int ret = 0;
    int reuse = 1;
    ret = setsockopt(fd_coordinator, SOL_SOCKET, SO_REUSEADDR,(const void *)&reuse , sizeof(int));
    if (ret < 0) {
        perror("setsockopt1");
        _exit(-1);
    }
    ret = setsockopt(fd_coordinator, SOL_SOCKET, SO_REUSEPORT,(const void *)&reuse , sizeof(int));
    if (ret < 0) {
        perror("setsockopt2");
        _exit(-1);
    }

    //绑定
    ret = bind(fd_coordinator, (struct sockaddr *)&coordinator_addr, sizeof(coordinator_addr));
    if(ret < 0){
        printf("bind error\n");
        exit(1);
    }

    //监听
    ret = listen(fd_coordinator, 10);
    if(ret < 0){
        printf("listen error\n");
        exit(1);
    }

    LOG += "Coordinator socket finish \r\n";
}

/*
    * @author: WilliamWuLH
    * @version: 2020.5.24
    * @see: Coordinator
    * 添加 participant 的信息，如数目，ip 和 port
*/

void Coordinator::insert_participant(int num, vector<string> &pip, vector<string> &pport){
    participant_num = num;
    participant_ip = pip;
    participant_port = pport;
    // for(int i=0; i<participant_num; i++){
    //     cout <<"participant_ip : " <<participant_ip[i] <<endl;
    //     cout <<"participant_port : " <<participant_port[i] <<endl;
    // }
}

/*
    * @author: WilliamWuLH
    * @version: 2020.5.25
    * @see: Coordinator
    * @see: Command
    * Coordinator 核心工作，2PC 协议
*/

void Coordinator::work(){
    while(1){
        string dbcommand = "";
        string to_send = "";
        Command command;

        // getline(std::cin, dbcommand);
        // dbcommand = command.RESP_produce(dbcommand);

        struct sockaddr_in client_addr;
    	LOG += "new connection\r\n";
		socklen_t len = sizeof(client_addr);

	    //接受连接
	    int fd_client = accept(fd_coordinator, (struct sockaddr *)&coordinator_addr, &len);
        if(fd_client < 0){
            continue;
        }
        char buffer[2048];
        int size = 0;
        size = read(fd_client, buffer, 2048 - 1);
        if(size <= 0){
            // break;
            continue;
        }
        else if(size > 0){
            buffer[size] = '\0';
            string dbcommand = buffer;
            LOG += "Coordinator command : " + dbcommand + "\r\n";
            string ans = send_participant(1, dbcommand);
            if(ans == "COMMIT"){
                ans = send_participant(2, "COMMIT");
                command.RESP_handle(dbcommand);
                if(command.operation == "SET"){
                    to_send = "+OK\r\n";
                    LOG += "Coordinator respond : " + to_send;
                }
                else if(command.operation == "GET"){
                    to_send = command.RESP_produce(ans);
                    LOG += "Coordinator respond : " + to_send;
                }
                else if(command.operation == "DEL"){
                    to_send = ":";
                    to_send += ans + "\r\n";
                    LOG += "Coordinator respond : " + to_send;
                }
            }
            else{
                send_participant(2, "ABORT");
                to_send = "-ERROR\r\n";
                LOG += "Coordinator respond : " + to_send;
            }
        }
        if( send(fd_client, to_send.c_str(), to_send.size(), 0) != (int)to_send.size()){
            cout <<"send ERROR" <<endl;
        }
    }
}

/*
    * @author: WilliamWuLH
    * @version: 2020.5.25
    * @see: Coordinator
    * 向每个 participant 发送信息
*/

string Coordinator::send_participant(int status, string msg){
    int PREPARED_count = 0;
    string ans = "";
    for(int i=0; i<participant_num; i++){
        struct sockaddr_in participant_addr;
        int fd_participant;
        char buf[BUFSIZ];

        memset(&participant_addr,0,sizeof(participant_addr));
        participant_addr.sin_family=PF_INET;
        participant_addr.sin_addr.s_addr=inet_addr(participant_ip[i].c_str());
        participant_addr.sin_port=htons(atoi(participant_port[i].c_str()));

        if((fd_participant = socket(PF_INET,SOCK_STREAM,0)) < 0){
            perror("socket error");
            exit(1);
        }
	    
        if(connect(fd_participant,(struct sockaddr *)&participant_addr,sizeof(struct sockaddr))<0){
            // perror("connect error");
            cout <<i <<" : connect error" <<endl;
            participant_ip.erase(participant_ip.begin() + i);
            participant_port.erase(participant_port.begin() + i);
            participant_num--;
            i--;
            continue;
        }

	    LOG += "connected to participant \r\n";

        if(status == 1){
            if( send(fd_participant, msg.c_str(), msg.size(), 0) != (int)msg.size()){
                cout <<"ERROR" <<endl;
            }
            int len = recv(fd_participant,buf,BUFSIZ,0);
            buf[len] = '\0';
            string b = buf;
            LOG += "participant : " + b + "\r\n";
            if(b == "PREPARED")
                PREPARED_count++;
        }
        else if(status == 2){
            if( send(fd_participant, msg.c_str(), msg.size(), 0) != (int)msg.size()){
                cout <<"ERROR" <<endl;
            }
            int len = recv(fd_participant,buf,BUFSIZ,0);
            buf[len] = '\0';
            string b = buf;
            LOG += "participant : " + b + "\r\n";
            if(b == "DONE"){
                ans = "DONE";
            }
            else{
                ans = b.substr(5, b.length()-5);
            }
        }
        close(fd_participant);
        // cout <<PREPARED_count <<endl;
    }
    if(status == 1){
        if(PREPARED_count == participant_num)
            return "COMMIT";
        else
            return "ABORT";
    }
    else if(status == 2){
        return ans;
    }
    return ans;
}