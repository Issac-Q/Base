#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include <inttypes.h>
#include <pthread.h>
#include "Message.h"

/**
 * A thread-safe message queue--support mutiple producers/consumers
 * a message continue do (new =>handle =>delete), need to optimize a message pool(reuse Message object)
 * queue is implemented by single linked list
 **/
class MessageQueue
{
public:
    MessageQueue();
    ~MessageQueue();

    //default is UINT64_MAX, that means almost no limit
    //0 will not effect
    void setMaxSize(uint64_t maxSize);
    void pushMessage(Message* msg, uint64_t when);
    Message* popMessage();

private:
    Message* mHead;
    Message* mTail;  //optimization insert message

    pthread_mutex_t mMutex;
    pthread_cond_t mCond;

    uint64_t mCachedSize;
    uint64_t mMaxSize;
    uint32_t mWaitPushThreads;
    uint32_t mWaitPopThreads;
};


#endif