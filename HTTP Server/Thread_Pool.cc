#include "Thread_Pool.hpp"

template <class T> threadpool<T>::threadpool(int thread_num){
    //initialize
    thread_number = thread_num;
    is_stop = false;
    all_threads = NULL;
    //judge
    if(thread_num <= 0){
        cout <<"thread_number <= 0, initialize the thread_number = 4" <<endl;
        thread_number = 4;
    }
	    
    all_threads = new pthread_t[thread_number];
    if(all_threads == NULL)
    	cout <<"can't init threadpool because thread array can't new" <<endl;
}


template <class T> threadpool<T>::~threadpool(){
    delete [] all_threads;
    stop();
}

//stop the thread pool
template <class T> void threadpool<T>::stop(){
    is_stop = true;
    queue_cond_locker.broadcast();
}

//start the thread pool
template <class T> void threadpool<T>::start(){
    for(int i = 0; i < thread_number; ++i){
        //printf("create the %dth pthread\n", i);
        if(pthread_create(all_threads + i, NULL, worker, this) != 0){
            //创建线程失败，清除成功申请的资源并抛出异常
            delete []all_threads;
            throw std::exception();
        }

        if(pthread_detach(all_threads[i])){
            //将线程设置为脱离线程，失败则清除成功申请的资源并抛出异常
            delete []all_threads;
            throw std::exception();
        }
    }
}

//添加任务进入任务队列
template <class T> bool threadpool<T>::append_task(T *task){
    //获取互斥锁
    pthread_mutex_lock(&queue_mutex_locker);
    
    bool is_signal = task_queue.empty();
    //添加进入队列
    task_queue.push(task);

    //唤醒等待任务的线程
    if(is_signal)
        queue_cond_locker.signal();    

    pthread_mutex_unlock(&queue_mutex_locker);

    return true;
}

//in the thread : input the threadpool
template <class T> void *threadpool<T>::worker(void *arg){
    threadpool *pool = (threadpool *)arg;
    pool->run();
    return pool;
}

//get the task from the queue of task
template <class T> T* threadpool<T>::getTask(){
    T *task = NULL;
    //获取互斥锁
    pthread_mutex_lock(&queue_mutex_locker);

    if(!task_queue.empty())
    {
        task = task_queue.front();
        task_queue.pop();
    }
    pthread_mutex_unlock(&queue_mutex_locker);
    return task;
}


template <class T> void threadpool<T>::run(){
    cout <<"thread : " <<(unsigned long)pthread_self() <<endl;
    while(!is_stop){
        T *task = getTask();
        cout <<(unsigned long)pthread_self() <<" : RUNNING" <<endl;
        //don't work
        if(task == NULL)
            queue_cond_locker.wait();
        //work
        else{
            task->doit();
		    delete task;
        }
    }
}