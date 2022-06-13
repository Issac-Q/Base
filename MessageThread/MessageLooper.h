#ifndef MESSAGE_LOOPER_H
#define MESSAGE_LOOPER_H

#include <memory>
#include <pthread>
#include "MessageQueue.h"

class MessageLooper
{
public:
    MessageLooper();

    //attach a looper to the caller thread
    static void attach();
    static const MessageLooper* looper();

    void setQueue(std::shared_ptr<MessageQueue>* queue);
    std::shared_ptr<MessageQueue>* queue();
    void loop();
    void quit();

private:
    void setOwner(pthread_t tid);

private:
    pthread_t mOwnerId;
    std::shared_ptr<MessageQueue> mQueue;

    //key->looper of each thread
    static pthread_key_t sKey;
};

#endif