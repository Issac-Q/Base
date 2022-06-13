#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "Message.h"
#include "MessageHandler.h"

#include <unistd.h>


static const int MAX_POOL_SIZE = 50;
Message* Message::sPool = NULL;
uint Message::sPoolSize = 0;

Message::Message()
: mObject(NULL)
{

}
Message::Message(int type)
: mObject(NULL)
, mType(type)
{

}

Message::Message(int type, int arg1, int arg2)
: mObject(NULL)
, mType(type)
, mArg1(arg1)
, mArg2(arg2)
{

}

Message::Message(int type, int arg1, int arg2, const void* object)
: mType(type)
, mArg1(arg1)
, mArg2(arg2)
, mObject(object)
{

}

bool Message::recycle()
{
    mType = 0;
    mArg1 = 0;
    mArg2 = 0;
    if (mObject) {
        delete mObject;
        mObject = NULL;
    }
    bool recycleResult = false;
    while(true) {
        if (sMutex.try_lock()) {
            if (sPoolSize < MAX_POOL_SIZE) {
                mNext = sPool;
                sPool = this;
                ++sPoolSize;
                recycleResult = true;
            }
            sMutex.unlock();
            break;
        }
    }
    return recycleResult;
}

Message* Message::obtain()
{
    Message* msg = NULL;
    sMutex.lock();
    if (sPoolSize) {
        msg = sPool;
        sPool = sPool->mNext;
        --sPoolSize;
    }
    sMutex.unlock();

    //pool is empty, new it
    if (!msg) {
        msg = new Message();
    }
    msg->mNext = NULL;
    return msg;
}

Message::~Message()
{
    if (mObject) {
        delete mObject;
        mObject = NULL;
        //printf("release customData\n");
    }
}
