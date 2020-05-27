#include "participant_server.hpp"

/*
    * @author: WilliamWuLH
    * @version: 2020.5.24
    * @see: Participant
    * 初始化 Participant，建立 socket，绑定 ip 和 port
*/

void Participant::init(){
    participant_addr.sin_family = PF_INET;
    participant_addr.sin_port = htons(atoi(PORT));
    participant_addr.sin_addr.s_addr = inet_addr(IP);
    // cout <<participant_addr.sin_addr.s_addr <<endl;
    // cout <<participant_addr.sin_port <<endl;

    //创建socket
    fd_participant = socket(PF_INET, SOCK_STREAM, 0);
    if(fd_participant < 0){
        printf("socket error\n");
        exit(1);
    }

    int ret = 0;
    int reuse = 1;
    ret = setsockopt(fd_participant, SOL_SOCKET, SO_REUSEADDR,(const void *)&reuse , sizeof(int));
    if (ret < 0) {
        perror("setsockopt1");
        _exit(-1);
    }
    ret = setsockopt(fd_participant, SOL_SOCKET, SO_REUSEPORT,(const void *)&reuse , sizeof(int));
    if (ret < 0) {
        perror("setsockopt2");
        _exit(-1);
    }

    //绑定
    ret = bind(fd_participant, (struct sockaddr *)&participant_addr, sizeof(participant_addr));
    if(ret < 0){
        printf("bind error\n");
        exit(1);
    }

    //监听
    ret = listen(fd_participant, 10);
    if(ret < 0){
        printf("listen error\n");
        exit(1);
    }

    LOG += "Participant socket finish \r\n";
}

/*
    * @author: WilliamWuLH
    * @version: 2020.5.25
    * @see: Participant
    * @see: Command
    * Participant 核心工作，2PC 协议
*/

void Participant::work(){
    int status = 1;
    Command command;
    while(1)
    {
        struct sockaddr_in coordinator_addr;
    	LOG += "Participant : new connection\r\n";
		socklen_t len = sizeof(coordinator_addr);

		//接受连接
		int fd_coordinator = accept(fd_participant, (struct sockaddr *)&coordinator_addr, &len);
        if(fd_coordinator < 0){
            continue;
        }
        
        string to_send = "NO";
        char buffer[BUFFSIZE];
        int size = 0;
        size = read(fd_coordinator, buffer, BUFFSIZE - 1);
        if(size <= 0){
            // break;
            continue;
        }
        else if(size > 0){
            buffer[size] = '\0';
            string buf = buffer;
            // cout <<buf <<endl;
            //Prepared phase
            if(status == 1 && buf[0] == '*'){
                command.RESP_handle(buf);
                //SET
                if(command.operation == "SET" || command.operation == "GET" || command.operation == "DEL"){
                    to_send = "PREPARED";
                    LOG += "Participant Prepared phase : PREPARED \r\n";
                }
                else{
                    LOG += "Participant Prepared phase ERROR \r\n";
                    to_send = "NO";
                }
            }
            //Commit-Or-Abort phase
            else if(status == 2 && buf == "COMMIT"){
                //SET
                if(command.operation == "SET"){
                    KVstore[command.key[0]] = command.value;
                    LOG += "Participant SET " + command.key[0] + ":" + command.value + "\r\n";
                    to_send = "DONE";
                }
                //GET
                else if(command.operation == "GET"){
                    string get_value = "";
                    if(KVstore.count(command.key[0]) == 0)
                        get_value = "nil";
                    else{
                        get_value = KVstore[command.key[0]];
                    }
                    LOG += "Participant DONE" + get_value + "\r\n";
                    to_send = "DONE " + get_value;
                }
                //DEL
                else if(command.operation == "DEL"){
                    int count = 0;
                    for(int i=0; i<command.key_number; i++){
                        if(KVstore.count(command.key[i])){
                            count++;
                            KVstore.erase(command.key[i]);
                            LOG += "Participant DEL : " + command.key[i] + "\r\n";
                        }
                    }
                    stringstream ss;
                    ss << count;
                    string slen;
                    slen = ss.str();
                    to_send = "DONE " + slen;
                    LOG += "Participant DONE" + slen + "\r\n";
                }
            }
            else if(status == 2 && buf == "ABORT"){
                to_send = "DONE";
                LOG += "Participant ABORT DONE \r\n";
            }
            else{
                LOG += "Participant UNKNOW ERROR\r\n";
                to_send = "NO";
            }
        }
        // cout <<to_send <<endl;
        if( send(fd_coordinator, to_send.c_str(), to_send.size(), 0) != (int)to_send.size()){
            LOG += "send ERROR\r\n";
        }
        else{
            if(status == 1){
                status = 2;
                LOG += "Participant status 2\r\n";
            }
            else if(status == 2){
                status = 1;
                LOG += "Participant status 1\r\n";
            }
        }
    }
}
