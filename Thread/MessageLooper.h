#ifndef MESSAGE_LOOPER_H
#define MESSAGE_LOOPER_H

#include "MessageQueue.h"

class MessageLooper
{
public:
    MessageLooper();

    MessageQueue* queue();
    void loop();
    void quit();
    
private:
    MessageQueue mQueue;
};

#endif