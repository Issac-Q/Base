#include "MessageQueue.h"

MessageQueue::MessageQueue()
: mHead(NULL)
, mTail(NULL)
, mCachedSize(0)
, mMaxSize(UINT64_MAX)
, mWaitPushThreads(0)
, mWaitPopThreads(0)
{
    pthread_mutex_init(&mMutex, NULL);
    pthread_cond_init(&mCond, NULL);
}

MessageQueue::~MessageQueue()
{
    while (mHead) {
        Message* next = mHead->mNext;
        delete mHead;
        mHead = next;
    }
    pthread_mutex_destroy(&mMutex);
    pthread_cond_destroy(&mCond);    
}

void MessageQueue::setMaxSize(uint64_t maxSize)
{
    if (maxSize != 0) {
        mMaxSize = maxSize;
    }
}

void MessageQueue::pushMessage(Message* msg, uint64_t when)
{
    if (!msg) {
        printf("MessageQueue::pushMessage invalid operattion:you pushed a NULL message\n");
        return;
    }

    msg->mWhen = when;
    pthread_mutex_lock(&mMutex);
    ++mWaitPushThreads;
    while (mCachedSize == mMaxSize) {
        //1. unlock 2.wake 3. lock
        //虚假唤醒有两种
        //1. 因为系统中断唤醒而不是pthread_cond_signal/brodcast唤醒
        //2. 唤醒到获取锁的的间隙有其他线程区使用了        
        pthread_cond_wait(&mCond, &mMutex);
    }
    --mWaitPushThreads;

    //append a message to tail for most non-preemptive message
    if (mTail && when >= mTail->mWhen) {        
        mTail->mNext = msg;
        mTail = msg;
    }
    else {
        //有趣的二级指针迭代法
        //普通方式使用间接方式:node->next访问和修改节点中继指针
        //二级指针使用直接方式:pp = &(node->next); *pp访问和修改中继指针
        //优点1.不用关心是否插入在头部还是中间; 2.不用维护pre指针
        Message** next = &mHead;
        while (*next) {
            Message* node = *next;
            if (when < node->mWhen) {
                break;
            }
            next = &(node->mNext);
        }
        msg->mNext = *next;
        *next = msg;
        if (!mTail) {
            //queue becomes no empty
            mTail = msg;
            //这种在临界点一次性唤醒所有线程的方式不可行,原因有两点:
            //1. 这个时候只有一条消息, 除了那个幸运的线程,其他的线程唤醒了也没消息可用,立马又陷入block
            //2. 等消息多了之后,由于只会在这里唤醒这一次,所以其他的线程再也没机会被唤醒了
            //解决方法:1. 每次都调用pthread_cond_broadcast饱和唤醒
            //        2. 记录block的生产者和消费者,按需唤醒
            // pthread_cond_broadcast(&mCond);
        }
    }
    ++mCachedSize;

    //如果有block的消费者线程,按需唤醒它
    if (mWaitPopThreads) {
        if (mWaitPopThreads == 1) {
            pthread_cond_signal(&mCond);    //wake at leaest one blocked pop thread
        }
        else {
            pthread_cond_broadcast(&mCond); //wake all blocked pop thread
        }
    }    
    pthread_mutex_unlock(&mMutex);
}

Message* MessageQueue::popMessage()
{
    pthread_mutex_lock(&mMutex);
    ++mWaitPopThreads;
    while (!mHead) {
        //1. unlock 2.wake 3. lock
        pthread_cond_wait(&mCond, &mMutex);
    }
    --mWaitPopThreads;

    Message* retMessage = mHead;
    mHead = mHead->mNext;
    if (!mHead) {
        //queue becomes empty
        mTail = mHead;
    }
    --mCachedSize;
    
    //如果有block的生产者线程,按需唤醒它
    if (mWaitPushThreads) {
        if (mWaitPushThreads == 1) {
            pthread_cond_signal(&mCond);    //wake at leaest one blocked pop thread
        }
        else {
            pthread_cond_broadcast(&mCond); //wake all blocked pop thread
        }
    }
    pthread_mutex_unlock(&mMutex);
    return retMessage;
}