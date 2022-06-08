#include "QueueUser.h"

QueueUser::QueueUser()
: mQueue()
{

}

QueueUser::~QueueUser()
{

}

void QueueUser::setQueue(std::shared_ptr<CircularQueue>& queue)
{
    mQueue = queue;
}

std::shared_ptr<CircularQueue>& QueueUser::queue()
{
    return mQueue;
}

void QueueUser::wakeupSelfIfNeed()
{
    if (mQueue.get()) {
        mQueue->wake();
    }  
}