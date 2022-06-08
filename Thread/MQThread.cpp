#include "MQThread.h"
#include <stdio.h>

MQThread::MQThread()
: mMsgQueue()
, mListener(nullptr)
{
    pthread_mutex_init(&mMutex, NULL);
    pthread_cond_init(&mCond, NULL);
}

MQThread::~MQThread()
{
    pthread_mutex_destroy(&mMutex);
    pthread_cond_destroy(&mCond);
}

bool MQThread::stopThread()
{
    //push a exit message to break the loop
    Message* msg = new Message(MSG_TYPE_EXIT);
    pushMessage(*msg, false);

    Thread::stopThread();
}


void MQThread::registerListener(MQThreadListener* listener)
{
    mListener = listener;
}

/**
 * Producer: 1. check queue full --> wait
 *           2. push_back
 *           3. check queue empty before push --> notify
 **/
void MQThread::pushMessage(Message& msg, bool preemptible)
{
    pthread_mutex_lock(&mMutex);
    while (mMsgQueue.size() == MAX_MESSAGE_SIZE) {
        // printf("message queue block because of queue full\n");
        pthread_cond_wait(&mCond, &mMutex);
    }

    if (preemptible) {
        mMsgQueue.push_front(&msg);
    }
    else {
        mMsgQueue.push_back(&msg);
    }

    if (mMsgQueue.size() == 1) {
        pthread_cond_signal(&mCond);
    }
    pthread_mutex_unlock(&mMutex);
}

/**
 * Consumer: 1. check queue empty --> wait
 *           2. pop_front
 *           3. check queue full before pop --> notify
 **/
Message& MQThread::popMessage()
{
    pthread_mutex_lock(&mMutex);
    while (mMsgQueue.size() == 0) {
        // printf("message queue block because of queue empty\n");
        pthread_cond_wait(&mCond, &mMutex);
    }

    Message& msg = *mMsgQueue.front();
    mMsgQueue.pop_front();

    if (mMsgQueue.size() == MAX_MESSAGE_SIZE - 1) {
        pthread_cond_signal(&mCond);
    }
    pthread_mutex_unlock(&mMutex);
    return msg;
}


bool MQThread::handleMessage(Message& msg)
{
    // printf("handleMessage => type:%d, arg1:%d, arg2:%d\n", msg.type(), msg.arg1(), msg.arg2());
    int msgType = msg.type();
    if (mListener) {
        mListener->onHandleMessage(msg);
    }
    //TODO handle custom message
    msg.handle();
    delete &msg;
    return msgType != MSG_TYPE_EXIT;
}

void MQThread::run()
{
    // printf("message queue loop start\n");
    while(handleMessage(popMessage()));
    quit(); //Active exit
    // printf("message queue loop end\n");
}