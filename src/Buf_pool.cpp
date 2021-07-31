#include "../include/Buf_pool.h"
// 单例对象
Buf_pool *Buf_pool::m_instance = NULL;
// 用于保证创建单例的一个方法全局只执行一次
pthread_once_t Buf_pool::m_once = PTHREAD_ONCE_INIT;

// buf_pool ->[m4K]---> io_buf...io_buf...io_buf
Buf_pool::Buf_pool(){
    // 链表的头指针
    io_buf *prev;
    // 开辟4K Buf内存池
    m_pool[m4K] = new io_buf(m4K);
    if(m_pool[m4K] == NULL)
    {
        fprintf(stderr,"new io_buf m4K error");
        exit(1);
    }

    prev = m_pool[m4K];
    // 4K的buf 预先开辟5000个,约20M供开发者使用
    for (int i = 1;i< 5000;++i) {
        prev->next = new io_buf(m4K);
        if(prev->next == NULL)
        {
            fprintf(stderr,"new io_bf m4K error");
            exit(1);
        }
        prev = prev->next;
    }
    // =========以上将4K大小的io_buf全部开辟完毕
}
// 从内存池中申请一块内存
io_buf* Buf_pool::allc_buf(int N){
return nullptr;
}
io_buf* Buf_pool::allc_buf(){
return nullptr;
}

// 重置一个io_buf放回pool中
void Buf_pool::revert(io_buf *buffer){

}
