#include <iostream>
#include "../include/tcp_server.h"

int main()
{
    tcp_server server("127.0.0.1",8899);
    server.do_accept();
    return 0;
}

