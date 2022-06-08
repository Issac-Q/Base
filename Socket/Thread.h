#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>

#define THREAD_NAME_MAX_LENGTH 32

class Thread
{
public:
	Thread();
	virtual ~Thread();

    bool startThread(const char *name = NULL);
    virtual bool stopThread();
    bool terminate();

protected:
    void quit();
    const char* threadName();
    virtual void preRun() {};
    virtual void run() = 0;		
	virtual void postRun() {};

private:
    static void threadProc(void* p);
    void execRun();
    void loop();

private:
    bool mQuit;
    pthread_t          mTid;
    pthread_attr_t     mAttr;
    char               mThreadName[THREAD_NAME_MAX_LENGTH];
};

#endif