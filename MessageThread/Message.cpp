#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "Message.h"
// #include "MessageHandler.h"

#include <unistd.h>


static const int MAX_POOL_SIZE = 50;
Message* Message::sPool = NULL;
Message::PoolGC Message::sPoolGC;
uint64_t Message::sPoolSize = 0;
Message::SpinLock Message::sSpinlock;

//构造函数调用本类其他的构造函数,参数列表不能再初始化变量了,
//因为一个构造函数一定是完整的,不会继续(可能重复)使用参数初始化列表初始化对象
Message::Message()
: Message(0, 0, 0, NULL)
{

}
Message::Message(int type)
: Message(type, 0, 0, NULL)
{

}

Message::Message(int type, int arg1, int arg2)
: Message(type, arg1, arg2, NULL)
{

}

//参数最多的这个是实际上的构造函数,其他的都是委派它实现构造
Message::Message(int type, int arg1, int arg2, void* object)
: mType(type)
, mArg1(arg1)
, mArg2(arg2)
, mObject(object)
{
    printf("Message::Message()\n");
}

Message::~Message()
{
    printf("Message::~Message()\n");
    if (mObject) {
        delete mObject;
        mObject = NULL;
        //printf("release customData\n");
    }
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
    sSpinlock.lock();
    if (sPoolSize < MAX_POOL_SIZE) {
        mNext = sPool;
        sPool = this;
        ++sPoolSize;
        recycleResult = true;
    }
    sSpinlock.unlock();
    return recycleResult;
}

Message* Message::obtain()
{
    Message* msg = NULL;
    sSpinlock.lock();
    if (sPoolSize) {
        msg = sPool;
        sPool = sPool->mNext;
        --sPoolSize;
    }
    sSpinlock.unlock();

    //pool is empty, new it
    if (!msg) {
        msg = new Message();
    }
    msg->mNext = NULL;
    return msg;
}

