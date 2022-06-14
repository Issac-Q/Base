#include "MessageQueue.h"
#include "Log.h"

MessageQueue::MessageQueue()
: mHead(NULL)
, mTail(NULL)
, mCachedSize(0)
, mMaxSize(UINT64_MAX)
, mWaitPushThreads(0)
, mWaitPopThreads(0)
, mQuitPush(false)
, mQuitPop(false)
{
    printf("MessageQueue::MessageQueue()\n");
    pthread_mutex_init(&mMutex, NULL);
    pthread_cond_init(&mPushCond, NULL);
    pthread_cond_init(&mPopCond, NULL);
}

MessageQueue::~MessageQueue()
{
    printf("MessageQueue::~MessageQueue()\n");
    while (mHead) {
        Message* next = mHead->mNext;
        delete mHead;
        mHead = next;
    }
    pthread_mutex_destroy(&mMutex);
    pthread_cond_destroy(&mPushCond);
    pthread_cond_destroy(&mPopCond);
}

void MessageQueue::pushMessage(Message* msg, uint64_t when)
{
    if (!msg) {
        printf("MessageQueue::pushMessage invalid operattion:you pushed a NULL message\n");
        return;
    }

    if (!msg->mHandler) {
        printf("MessageQueue::pushMessage invalid operattion:you pushed a message with NULL handler\n");
        return;
    }    

    msg->mWhen = when;
    pthread_mutex_lock(&mMutex);
    ++mWaitPushThreads;
    while (mCachedSize == mMaxSize && !mQuitPush) {
        //1. unlock 2.wake 3. lock
        //虚假唤醒有两种
        //1. 因为系统中断唤醒而不是pthread_cond_signal/brodcast唤醒
        //2. 唤醒到获取锁的的间隙有其他线程区使用了        
        pthread_cond_wait(&mPushCond, &mMutex);
    }
    --mWaitPushThreads;
    //means quit push
    if (mQuitPush && mQuitPushTid == pthread_self()) {
        pthread_mutex_unlock(&mMutex);
        return;
    }

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
        if (mWaitPopThreads == 1 || mCachedSize == 1) {
            pthread_cond_signal(&mPopCond);    //wake at leaest one blocked pop thread
        }
        else {
            pthread_cond_broadcast(&mPopCond); //wake all blocked pop thread
        }
    }
    pthread_mutex_unlock(&mMutex);
}

Message* MessageQueue::popMessage()
{
    pthread_mutex_lock(&mMutex);
    ++mWaitPopThreads;
    while (mCachedSize == 0 && !mQuitPop) {
        //1. unlock 2.wake 3. lock
        pthread_cond_wait(&mPopCond, &mMutex);
    }
    --mWaitPopThreads;
    Message* retMessage;
    //means quit pop
    if (mQuitPop && mQuitPopTid == pthread_self()) {
        retMessage = NULL;
        pthread_mutex_unlock(&mMutex);
        return retMessage;
    }

    retMessage = mHead;
    mHead = mHead->mNext;
    if (!mHead) {
        //queue becomes empty
        mTail = mHead;
    }
    --mCachedSize;
    
    //如果有block的生产者线程,按需唤醒它
    if (mWaitPushThreads) {
        if (mWaitPushThreads == 1 || mCachedSize == mMaxSize - 1) {
            pthread_cond_signal(&mPushCond);    //wake at leaest one blocked pop thread
        }
        else {
            pthread_cond_broadcast(&mPushCond); //wake all blocked pop thread
        }
    }
    pthread_mutex_unlock(&mMutex);
    return retMessage;
}

void MessageQueue::quitPush(pthread_t tid)
{
    pthread_mutex_lock(&mMutex);
    if (mQuitPush) {
        pthread_mutex_unlock(&mMutex);
        return;    
    }
    mQuitPush = true;
    if (mWaitPushThreads) {
        if (mWaitPushThreads == 1) {
            pthread_cond_signal(&mPushCond);    //wake at leaest one blocked pop thread
        }
        else {
            pthread_cond_broadcast(&mPushCond); //wake all blocked pop thread
        }
    }
    mQuitPushTid = tid;
    pthread_mutex_unlock(&mMutex);
}

void MessageQueue::quitPop(pthread_t tid)
{
    pthread_mutex_lock(&mMutex);
    if (mQuitPop) {
        pthread_mutex_unlock(&mMutex);
        return;
    }
    mQuitPop = true;
    if (mWaitPopThreads) {
        if (mWaitPopThreads == 1) {
            pthread_cond_signal(&mPopCond);    //wake at leaest one blocked pop thread
        }
        else {
            pthread_cond_broadcast(&mPopCond); //wake all blocked pop thread
        }
    }
    mQuitPopTid = tid;
    pthread_mutex_unlock(&mMutex);
}