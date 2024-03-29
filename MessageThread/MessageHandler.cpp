#include <sys/time.h>
#include "MessageHandler.h"
#include "MessageLooper.h"

MessageHandler::MessageHandler()
: MessageHandler(MessageLooper::looper())
{
    
}

MessageHandler::MessageHandler(MessageLooper* looper)
{
    if (looper) {
        mQueue = *(looper->queue());
        //一个thread可以有多个handler, 一个handler只能跟一个thread联系，多个thread的多个handler可以联系到同一个messagequeue，这就是多生产者
        mOwnerId = pthread_self();
    }
}

MessageHandler::~MessageHandler()
{
    if (mQueue) {
        mQueue->quitPush(mOwnerId);
    }
}

Message* MessageHandler::obtainMessage()
{
    return Message::obtain();
}

void MessageHandler::sendMessage(Message* msg)
{
    sendMessageDelayed(msg, 0);
}

void MessageHandler::sendMessage(int type)
{
    Message* msg = Message::obtain();
    msg->mType = type;
    sendMessage(msg);
}

void MessageHandler::sendMessage(int type, int arg1, int arg2)
{
    Message* msg = Message::obtain();
    msg->mType = type;
    msg->mArg1 = arg1;
    msg->mArg2 = arg2;
    sendMessage(msg);
}

void MessageHandler::sendMessage(int type, int arg1, int arg2, void* object)
{
    Message* msg = Message::obtain();
    msg->mType = type;
    msg->mArg1 = arg1;
    msg->mArg2 = arg2;
    msg->mObject = object;
    sendMessage(msg);
}

void MessageHandler::sendMessageDelayed(Message* msg, uint64_t delayMillis)
{
    if (mQueue) {
        struct timeval t;
        gettimeofday(&t, NULL);
        uint64_t currentMs = t.tv_sec * 1000 + t.tv_usec / 1000;
        msg->mHandler = this;
        mQueue->pushMessage(msg, currentMs + delayMillis);
    }
}

void MessageHandler::handleMessage(Message* msg)
{
    onHandleMessage(msg);
}

void MessageHandler::quit()
{
    if (mQueue) {
        mQueue->quitPush(mOwnerId);
    } 
}