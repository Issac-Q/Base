#ifndef MQ_THREAD_H
#define MQ_THREAD_H

#include <list>
#include "Thread.h"
#include "Message.h"

//-1: no limit
#define MAX_MESSAGE_SIZE 100
#define MSG_TYPE_EXIT -1

class MQThreadListener
{
public:
    virtual void onHandleMessage(Message& msg) = 0;
};

class MQThread : public Thread
{
public:
    MQThread();
    ~MQThread();

    virtual bool stopThread() final;

    void registerListener(MQThreadListener* listener);
    void pushMessage(Message& msg, bool preemptible = false);
    bool handleMessage(Message& msg);

protected:
    virtual void run();

private:
    Message& popMessage();

private:
    pthread_mutex_t mMutex;
    pthread_cond_t mCond;

    std::list<Message*> mMsgQueue;
    MQThreadListener* mListener;
};


#endif