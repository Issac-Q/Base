#ifndef MY_HANDLER_H
#define MY_HANDLER_H

#include "MessageHandler.h"

class MyHandler : public MessageHandler
{
public:
    MyHandler()
    : MyHandler(NULL)
    {

    };

    MyHandler(MessageLooper* looper)
     : MessageLooper(looper) 
    {

    };

protected:
    virtual void onHandleMessage(Message* msg);
};

#endif