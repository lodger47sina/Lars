#include <iostream>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include "../include/tcp_server.h"

void lars_hello()
{
    std::cout<<"lars Hello"<<std::endl;
}

// 构造函数
tcp_server::tcp_server(const char* ip,uint16_t port){
    // 优化方案 忽略信号  SIGHUB SIGPIPE
#if 0
    if(signal(SIGHUP,SIG_IGN) == SIG_ERR)
    {
        perror("signal ignore SIGHUB");
        exit(1);
    }
    if(signal(SIGPIPE,SIG_IGN) == SIG_ERR)
    {
        perror("signal ignore SIGPIPE");
        exit(1);
    }
#endif
   sigset_t myset,oldset;
   sigemptyset(&myset);
   sigaddset(&myset,SIGHUP);
   sigaddset(&myset,SIGPIPE);
   sigprocmask(SIG_BLOCK,&myset,&oldset);

   int sfd= socket(AF_INET,SOCK_STREAM|SOCK_CLOEXEC,IPPROTO_TCP);
   if(-1 == sfd)
       perror("socket");
   m_sockfd = sfd;
   struct sockaddr_in serv_addr;                                                                                               
   memset(&serv_addr,0,sizeof(serv_addr));
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_port = htons(port);
   serv_addr.sin_addr.s_addr = inet_addr(ip); 

   //绑定IP和端口
   int flag= 1;
   if(setsockopt(m_sockfd,SOL_SOCKET,SO_REUSEADDR,&flag,sizeof(flag)) < 0){
        fprintf(stderr,"setsockopt err");
        exit(1);
   }
   
   if(-1 == bind(m_sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr)))
       perror("bind");
   if(-1 == listen(m_sockfd,500))
       perror("listen");
   printf("Start Server......\n");
}

// 开始提供创建关键链接的服务
void tcp_server::do_accept(){
    int connfd;
    while(1)
    {
        // accept
        connfd = accept(m_sockfd,(struct sockaddr*)&m_connaddr,&m_addrlen);
        if(-1 == connfd)
        {
            if(errno == EINTR)
            {
                // 终端错误
                fprintf(stderr,"accept errno = EINTR\n");
                continue;
            }
            else if(errno == EAGAIN)
            {
                fprintf(stderr,"accept errno = EAGAIN");
                break;
            }
            else if(errno == EMFILE)
            {
                // 键列连接过多，资源不够
                fprintf(stderr,"accept errno = EMFILE");
                continue;
            }
            else{
                fprintf(stderr,"accept errno");
                exit(1);
            }
        }
        else{
            // accept success
            // 添加一些心跳机制
            // 添加消息队列
            // 协议给回显业务
            int writed;
            std::string data = "hello Lars";
            do{
                writed = write(connfd,data.c_str(),data.size());
            }while(writed == -1 && errno == EINTR); // 表示非阻塞失败
            if(writed > 0)
            {
                printf("write succ!\n");
            }
        }
        // 做回显业务
    }
}

// 析构函数，资源释放
tcp_server::~tcp_server(){
    close(m_sockfd);
}
