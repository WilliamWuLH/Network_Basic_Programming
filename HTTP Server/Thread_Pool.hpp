#ifndef _THREAD_POOL_
#define _THREAD_POOL_

#include <queue>
#include <stdio.h>
#include <exception>
#include <errno.h>
#include <pthread.h>
#include <iostream>
using namespace std;

/*条件变量 locker*/
class cond_locker
{
private:
    pthread_mutex_t m_mutex;
    pthread_cond_t m_cond;

public:
    // 初始化 m_mutex and m_cond
    cond_locker()
    {
        if(pthread_mutex_init(&m_mutex, NULL) != 0)
            printf("mutex init error");
        
        if(pthread_cond_init(&m_cond, NULL) != 0)
        {   
            //条件变量初始化是被，释放初始化成功的mutex
            pthread_mutex_destroy(&m_mutex);
            printf("cond init error");
        }
    }

    // destroy mutex and cond
    ~cond_locker()
    {
        pthread_mutex_destroy(&m_mutex);
        pthread_cond_destroy(&m_cond);
    }
    //等待条件变量
    bool wait()
    {
        int ans = 0;
        pthread_mutex_lock(&m_mutex);
        ans = pthread_cond_wait(&m_cond, &m_mutex);
        pthread_mutex_unlock(&m_mutex);
        return ans == 0;
    }
    //唤醒等待条件变量的线程
    bool signal()
    {
	    return pthread_cond_signal(&m_cond) == 0;
    }

    //唤醒all等待条件变量的线程
    bool broadcast()
    {
        return pthread_cond_broadcast(&m_cond) == 0;
    }
};

template <class T> class threadpool
{
    private:
        //线程池的线程数
        int thread_number;
        //线程数组
        pthread_t *all_threads;
        //任务队列
        std::queue<T *> task_queue;
        //互斥锁
        pthread_mutex_t queue_mutex_locker;
        //cond
        cond_locker queue_cond_locker;
        //是否结束线程
        bool is_stop;
    
    public:
        threadpool(int thread_num = 8);

        ~threadpool();

        bool append_task(T *task);

        void start();

        void stop();
    
    private:
        //线程运行的函数。执行run()函数
        static void *worker(void *arg);

        void run();

        T *getTask();
};

#endif
