#ifndef MY_HANDLER_H
#define MY_HANDLER_H

#include "MessageHandler.h"

class MyHandler : public MessageHandler
{
protected:
    virtual void onHandleMessage(Message* msg);
};

#endif