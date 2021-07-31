#pragma once

#include <arpa/inet.h>
class tcp_server
{
public:
    // 构造函数
    tcp_server(const char* ip,uint16_t port);
    // 开始提供创建关键链接的服务
    void do_accept();
    // 析构函数，资源释放
    ~tcp_server();
private:
    int m_sockfd; // 套接字 listen fd
    struct sockaddr_in m_connaddr; // 客户端连接地址
    socklen_t m_addrlen; // 客户端链接地址长度
};

