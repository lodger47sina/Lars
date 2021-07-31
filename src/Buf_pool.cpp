#include "../include/Buf_pool.h"
#include <assert.h>
// 单例对象
Buf_pool *Buf_pool::m_instance = NULL;
// 用于保证创建单例的一个方法全局只执行一次
pthread_once_t Buf_pool::m_once = PTHREAD_ONCE_INIT;

/*
   pthread_mutex_lock(&mutex );
   pthread_mutex_unlock(&mutex );
   */

pthread_mutex_t Buf_pool::m_mutex = PTHREAD_MUTEX_INITIALIZER;

// buf_pool ->[m4K]---> io_buf...io_buf...io_buf
Buf_pool::Buf_pool()
{
    make_io_buf_list(m4K,5000);
    make_io_buf_list(m16K,1000);
    make_io_buf_list(m64K,500);
    make_io_buf_list(m256K,200);
    make_io_buf_list(m4M,50);
    make_io_buf_list(m8M,10);
}
// 从内存池中申请一块内存
io_buf* Buf_pool::alloc_buf(int N){
    int index;
    // 找到N最近的 一个刻度链表， 返回一个io_buf
    if(N <= m4K)
    {
        index = m4K;
    }else if(N <= m16K)
    {
        index = m16K;
    }else if(N <= m64K)
    {
        index = m64K;
    }else if(N <= m256K)
    {
        index = m256K;
    }else if(N <= m1M)
    {
        index = m1M;
    }else if(N <= m4M)
    {
        index = m4M;
    }else if(N <= m8M)
    {
        index = m8M;
    }else {
        return NULL;
    }
    // 如果该index内存已经没有了，需要额外的申请内存
    pthread_mutex_lock(&m_mutex);
    if(m_pool[index] == NULL)
    {
        // index链表为空，需要新申请index大小的io_buf
        if(m_total_mem + index/1024 >= MEM_LIMIT)
        {
            fprintf(stderr,"already use too many memory!\n");
            exit(1);
        }
        io_buf *new_buf = new io_buf(index);
        if(new_buf == NULL)
        {
            fprintf(stderr,"new io_buf error\n");
            exit(1);
        }
        m_total_mem += index/1024;
        pthread_mutex_unlock(&m_mutex);
        return new_buf;
    }
    // 如果index有内存，从pool中拆除一块内存返回
    io_buf *target = m_pool[index];
    m_pool[index] = target->next;
    pthread_mutex_unlock(&m_mutex);
    target->next = NULL;
    return target;
}
io_buf* Buf_pool::alloc_buf(){
    return alloc_buf(m4K);
}

// 重置一个io_buf放回pool中
void Buf_pool::revert(io_buf *buffer){
    // 将buffer放回pool中
    // index 是属于pool中那个链表的
    int index = buffer->capacity;
    buffer->length = 0;
    buffer->head = 0;

    pthread_mutex_lock(&m_mutex );
    // 断言，一定能找到index key
    assert(m_pool.find(index) != m_pool.end());
    // 将buffer设置为对应 buf链表的首节点
    buffer->next = m_pool[index];
    m_pool[index] = buffer;
    pthread_mutex_unlock(&m_mutex );
}
void Buf_pool::make_io_buf_list(int cap,int num){
    // 链表的头指针
    io_buf *prev;
    m_pool[cap] = new io_buf(cap);
    if(m_pool[cap] == NULL)
    {
        fprintf(stderr,"new io_buf %d error",cap);
        exit(1);
    }
    prev = m_pool[cap];
    for (int i = 1;i< num;++i) {
        prev->next = new io_buf(cap);
        if(prev->next == NULL)
        {
            fprintf(stderr,"new io_bf m1M error");
            exit(1);
        }
        prev = prev->next;
    }
    m_total_mem +=cap/1024*num;
}
