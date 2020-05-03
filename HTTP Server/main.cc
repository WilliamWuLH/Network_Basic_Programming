#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<string.h>
#include<fcntl.h>
#include <getopt.h> //getopt_long()头文件位置
#include "http_handle.hpp"
#include "Thread_Pool.cc"

int main(int argc, char** argv)
{
    int number_thread = 8;
    int fd_server, fd_client;
   	struct sockaddr_in servaddr, client;

    //设置服务端的sockaddr_in
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8888);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    cout <<servaddr.sin_addr.s_addr <<endl;
    cout <<servaddr.sin_port <<endl;

    const char *optstring="n:v";
    int c, index;
    struct option opts[]={{"ip",required_argument,NULL,'i'},
                          {"port",required_argument,NULL,'p'},
                          {"number-thread",required_argument,NULL,'n'},
                          {0,0,0,0}};

    while((c=getopt_long(argc,argv,optstring,opts,&index))!=-1)
    {
        switch(c)
        {
        case 'i':
            printf("IP : %s\n",optarg);
            servaddr.sin_addr.s_addr = inet_addr(optarg);
            cout <<servaddr.sin_addr.s_addr <<endl;
            break;
        case 'p':
            printf("PORT : %s\n",optarg);
            servaddr.sin_port = htons(atoi(optarg));
            cout <<servaddr.sin_port <<endl;
            break;
        case 'n':
            printf("number-thread : %s\n",optarg);
            number_thread = atoi(optarg);
            cout <<number_thread <<endl;
            break;
        default:
            printf("c is %d\n",c);
            break;
        }
    }

	//创建socket
	fd_server = socket(AF_INET, SOCK_STREAM, 0);
    if(fd_server < 0){
		printf("socket error\n");
		exit(1);
    }

    //绑定
    int ret = bind(fd_server, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if(ret < 0){
		printf("bind error\n");
		exit(1);
    }

    //监听
    ret = listen(fd_server, 10);
    if(ret < 0){
		printf("listen error\n");
		exit(1);
    }

    //创建线程池
    threadpool<ClientTask> pool(number_thread);
    pool.start();  //线程池开始运行

    while(1)
    {
    	//printf("new connection\n");
		socklen_t len = sizeof(client);

		//接受连接
		fd_client = accept(fd_server, (struct sockaddr *)&client, &len);
        ClientTask *client = new ClientTask(fd_client);

		//向线程池添加任务
		pool.append_task(client);
    }

    return 0;
}