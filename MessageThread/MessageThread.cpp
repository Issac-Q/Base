#include "MessageThread.h"

MeesageThread::MeesageThread()
: mLooper(NULL)
{

}

MeesageThread::~MeesageThread()
{
    stopThread();
}

void MeesageThread::run()
{
    MessageLooper::attach();
    mLooper = MessageLooper::looper();
    if (mLooper) {
        mLooper->loop();
    }
}

void MeesageThread::postRun()
{
    pthread_exit(0);
}

bool MeesageThread::stopThread()
{
    if (mLooper) {
        mLooper->quit();
    }    
    Thread::stopThread();
}