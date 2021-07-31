#pragma once
#include <pthread.h>
#include <ext/hash_map>
#include "io_buf.h"

typedef __gnu_cxx::hash_map<int,io_buf*> poll_t;

// 定义一些内存的刻度
enum MEM_CAP{
    m4K = 4096,
    m16K = 16384,
    m64K = 65536,
    m256K = 262144,
    m1M = 1048576,
    m4M = 4194304,
    m8M = 8388608,
};

// 总内存池的大小限制 单位是Kb
#define MEM_LIMIT (5U * 1024 * 1024)

class Buf_pool
{
    Buf_pool();
    Buf_pool(const Buf_pool&);
    const Buf_pool& operator=(const Buf_pool&);
    // 单例对象
    static Buf_pool *m_instance;
    // 用于保证创建单例的一个方法全局只执行一次
    static pthread_once_t m_once;
public:
    //=====================  buf_pool 属性============
    // 存放所有io_buf的map句柄
    poll_t m_pool;
    // 当前内存池总体大小
    uint64_t m_total_mem;
public:
    // 初始化单例对象
    static void init()
    {
        m_instance = new Buf_pool();
    }
    static Buf_pool* GetInstance(){
        // 保证 init方法在进程的声明周期只执行一次
        pthread_once(&m_once,init);
        return m_instance;
    }
    // 从内存池中申请一块内存
    io_buf* allc_buf(int N);
    io_buf* allc_buf();

    // 重置一个io_buf放回pool中
    void revert(io_buf *buffer);
};

