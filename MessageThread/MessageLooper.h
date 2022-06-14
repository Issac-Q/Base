#ifndef MESSAGE_LOOPER_H
#define MESSAGE_LOOPER_H

#include <memory>
#include <pthread.h>
#include "MessageQueue.h"

class MessageLooper
{
public:
    MessageLooper();
    MessageLooper(std::shared_ptr<MessageQueue>* queue);
    ~MessageLooper();

    //attach a looper to the caller thread
    static void attach();
    static void attachMainLooper();
    static MessageLooper* looper();
    static MessageLooper* mainLooper();

    void setQueue(std::shared_ptr<MessageQueue>* queue);
    std::shared_ptr<MessageQueue>* queue();
    void loop();
    void quit();

private:
    void setOwner(pthread_t tid);

private:
    pthread_t mOwnerId;
    std::shared_ptr<MessageQueue> mQueue;

    class ThreadLocal
    {
    public:
        ThreadLocal()
        {
            pthread_key_create(&key, &clean);
        }
        ~ThreadLocal()
        {
            pthread_key_delete(key);
        }

        void set(const void* value)
        {
            pthread_setspecific(key, value);
        }

        MessageLooper* get()
        {
            (MessageLooper*)pthread_getspecific(key);
        }
    private:
        static void clean(void* looper)
        {
            printf("clean\n");
            delete (MessageLooper*)looper;
        }        
        pthread_key_t key;
    };

    //key->looper of each thread
    static ThreadLocal sThreadLocal;

    class RWLock
    {
    public:
        RWLock()
        {
            pthread_rwlock_init(&rwlock, NULL);
        }
        ~RWLock()
        {
            pthread_rwlock_destroy(&rwlock);
        }

        void rdlock()
        {
            pthread_rwlock_rdlock(&rwlock);
        }

        void wrlock()
        {
            pthread_rwlock_wrlock(&rwlock);
        }

        void unlock()
        {
            pthread_rwlock_unlock(&rwlock);
        }
    private:
        pthread_rwlock_t rwlock;
    };

    static MessageLooper* sMainLooper;
    static RWLock sRWLock;  
};

#endif