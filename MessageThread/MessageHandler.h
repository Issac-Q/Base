#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include <memory>
#include "Message.h"
#include "MessageQueue.h"

class MessageLooper;

#define DECLARE_CONSTRUCTOR(className) \
    className()\
    : MessageHandler()\
    {\
\
    };\
\
    className(MessageLooper* looper)\
     : MessageHandler(looper) \
    {\
\
    };

class MessageHandler {
public:
    MessageHandler();
    MessageHandler(MessageLooper* looper);
    virtual ~MessageHandler();

    Message* obtainMessage();   //Cannot be null
    void sendMessage(Message* msg);
    void sendMessage(int type);
    void sendMessage(int type, int arg1, int arg2);
    void sendMessage(int type, int arg1, int arg2, void* customdata);
    void sendMessageDelayed(Message* msg, uint64_t delayMillis);
    void handleMessage(Message* msg);

    //wake up block when thread exit
    void quit(pthread_t threadID);
    //detach the thread id
    void detach(pthread_t threadID);

protected:
    virtual void onHandleMessage(Message* msg) = 0;

private:
    std::shared_ptr<MessageQueue> mQueue;
};

#endif