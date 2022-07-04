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
    //this exit way to call clean function
    pthread_exit(0);
}

void MeesageThread::onStop()
{
    if (mLooper) {
        mLooper->quit();    //wakeup from block
    }    
}