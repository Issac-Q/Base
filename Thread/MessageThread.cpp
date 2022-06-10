#include "MessageThread.h"

MeesageThread::MeesageThread()
: mLooper()
{

}

MeesageThread::~MeesageThread()
{
    stopThread();
}


MessageLooper* MeesageThread::looper()
{
    return &mLooper;
}

void MeesageThread::preRun()
{
    mLooper.setOwner(pthread_self());
}

void MeesageThread::run()
{
    mLooper.loop();
}

bool MeesageThread::stopThread()
{
    mLooper.quit();
    Thread::stopThread();
}