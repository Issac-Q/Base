#include "MessageThread.h"

MeesageThread::MeesageThread()
: mLooper(NULL)
{

}

MeesageThread::~MeesageThread()
{
    stopThread();
}

// MessageLooper* MeesageThread::looper()
// {
//     return &mLooper;
// }

// void MeesageThread::preRun()
// {
//     mLooper.setOwner(pthread_self());
// }

void MeesageThread::run()
{
    MessageLooper.attach();
    mLooper = MessageLooper.looper();
    if (mLooper) {
        mLooper->loop();
    }
}

bool MeesageThread::stopThread()
{
    if (mLooper) {
        mLooper->quit();
    }    
    Thread::stopThread();
}