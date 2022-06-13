#include "MessageLooper.h"

{
    pthread_key_create(&MessageLooper::sKey);
}

MessageLooper::MessageLooper()
: mOwnerId(0)
, mQueue()
{

}

void MessageLooper::setQueue(std::shared_ptr<MessageQueue>* queue)
{
    if (!mQueue && queue) {
        mQueue = *queue;
    }
}

std::shared_ptr<MessageQueue>* MessageLooper::queue()
{
    return &mQueue;
}

void MessageLooper::beginLoop()
{
    mOwnerId = pthread_self();
}

/*****static start*****/
void MessageLooper::attach()
{
    if (!pthread_getspecific(sKey)) {
        MessageLooper* looper = new MessageLooper();
        std::shared_ptr<MessageQueue> queue(new MessageQueue());
        looper->setQueue(&queue);
        pthread_setspecific(sKey, looper);
    }
}

const MessageLooper* MessageLooper::looper()
{
    return (MessageLooper*)pthread_getspecific(sKey);
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