#include "MessageLooper.h"

MessageLooper::MessageLooper()
: mOwnerId(0)
, mQueue()
{

}

void MessageLooper::setOwner(pthread_t tid)
{
    mOwnerId = tid;
}

void MessageLooper::setQueue(std::shared_ptr<MessageQueue>* queue)
{
    if (queue) {
        mQueue = *queue;
    }
}

std::shared_ptr<MessageQueue>* MessageLooper::queue()
{
    if (mQueue) {
        return &mQueue;
    }
    else {
        return NULL;
    }
}

void MessageLooper::loop()
{
    if (mQueue) {
        for (;;) {
            Message* msg = mQueue->popMessage();
            if (!msg) {
                //means quit loop
                return;
            }
            msg->mHandler->handleMessage(msg);
            //todo 回收message
            delete msg;
        }
    }
}

void MessageLooper::quit()
{
    if (mQueue) {
        mQueue->quitPop(mOwnerId);
    }
}