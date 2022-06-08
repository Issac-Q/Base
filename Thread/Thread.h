#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>

#define THREAD_NAME_MAX_LENGTH 128

class Thread
{
public:
	Thread();
	virtual ~Thread();

    bool startThread(const char *name = NULL);
    //you need stop when terminate other thread
    //you must make sure the cancel point
    bool terminate();
    //This way make sure your thread not block
    bool stopThread();

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
    void exitSelf();

private:
    bool mQuit;
    pthread_t          mTid;
    pthread_attr_t     mAttr;
    char               mThreadName[THREAD_NAME_MAX_LENGTH];
};

#endif