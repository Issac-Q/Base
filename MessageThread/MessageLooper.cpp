#include "MessageLooper.h"
#include "MessageHandler.h"

//C++静态成员变量都要在这里定义（调用构造函数构造）才能使用
MessageLooper::ThreadLocal MessageLooper::sThreadLocal;
MessageLooper::RWLock MessageLooper::sRWLock;
MessageLooper* MessageLooper::sMainLooper = NULL;

MessageLooper::MessageLooper()
: MessageLooper(NULL)
{
    
}

MessageLooper::MessageLooper(std::shared_ptr<MessageQueue>* queue)
: mOwnerId(0)
, mQueue(*queue)
{
    printf("MessageLooper::MessageLooper\n");
    // setQueue(queue);
}

MessageLooper::~MessageLooper()
{
    printf("MessageLooper::~MessageLooper\n");
}

// void MessageLooper::setQueue(std::shared_ptr<MessageQueue>* queue)
// {
//     if (!mQueue && queue) {
//         mQueue = *queue;
//     }
// }

std::shared_ptr<MessageQueue>* MessageLooper::queue()
{
    return &mQueue;
}

/*****static start*****/
//一个thread必须attach一个looper，但是一个looper可以attach到多个messagequeue
//实现多消费者
void MessageLooper::attach(std::shared_ptr<MessageQueue>* queue)
{
    if (!sThreadLocal.get()) {
        MessageLooper* looper;
        if (queue) {
            looper =  new MessageLooper(NULL);
        }
        else {
            std::shared_ptr<MessageQueue> queue(new MessageQueue());
            looper = new MessageLooper(&queue);
        }
        sThreadLocal.set(looper);
    }
}

void MessageLooper::attachMainLooper()
{    
    sRWLock.rdlock();
    bool toAttach = !sMainLooper;
    sRWLock.unlock();

    if (toAttach) {
        attach();
        sRWLock.wrlock();        
        sMainLooper = looper();
        sRWLock.unlock();
    }   
}

MessageLooper* MessageLooper::looper()
{
    return (MessageLooper*)sThreadLocal.get();
}

MessageLooper* MessageLooper::mainLooper()
{
    sRWLock.rdlock();
    MessageLooper* looper = sMainLooper;
    sRWLock.unlock();
    return looper;
}
/*****static end*****/

void MessageLooper::setOwner(pthread_t tid)
{
    mOwnerId = tid;
}

void MessageLooper::loop()
{
    if (mQueue) {
        setOwner(pthread_self());
        for (;;) {
            Message* msg = mQueue->popMessage();
            if (!msg) {
                printf("loop quit\n");
                return;
            }
            msg->mHandler->handleMessage(msg);
            //try to recycle
            if (!msg->recycle()) {
                delete msg;
                msg = NULL;
            }       
        }
    }
}

void MessageLooper::quit()
{
    if (mQueue) {
        mQueue->quitPop(mOwnerId);
    }
}