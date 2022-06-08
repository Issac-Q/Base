#include "MessageLooper.h"

MessageLooper::MessageLooper()
: mQueue()
{

}

MessageQueue* MessageLooper::queue()
{
    return &mQueue;
}

void MessageLooper::loop()
{
    for (;;) {
        Message* msg = mQueue.popMessage();
        if (!msg) {
            //means quit loop
            return;
        }
        msg->mHandler->handleMessage(msg);
        //todo 回收message
        delete msg;
    }
}

void MessageLooper::quit()
{
    mQueue.quit();
}