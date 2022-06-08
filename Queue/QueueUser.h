#ifndef QUEUE_USER
#define QUEUE_USER

#include <memory>
#include "CircularQueue.h"

class QueueUser
{
public:
    QueueUser();
    virtual ~QueueUser();

    void setQueue(std::shared_ptr<CircularQueue>& queue);
    std::shared_ptr<CircularQueue>& queue();
    void wakeupSelfIfNeed();

private:
    std::shared_ptr<CircularQueue> mQueue;
};

#endif