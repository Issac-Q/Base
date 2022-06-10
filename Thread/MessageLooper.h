#ifndef MESSAGE_LOOPER_H
#define MESSAGE_LOOPER_H

#include <memory>
#include "MessageQueue.h"

class MessageLooper
{
public:
    MessageLooper();

    void setOwner(pthread_t tid)
    void setQueue(std::shared_ptr<MessageQueue>* queue);
    std::shared_ptr<MessageQueue>* queue();
    void loop();
    void quit();
    
private:
    pthread_t mOwnerId;
    std::shared_ptr<MessageQueue> mQueue;
};

#endif