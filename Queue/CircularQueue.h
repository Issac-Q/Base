#ifndef CIRCULAR_QUEUE_H
#define CIRCULAR_QUEUE_H

#include "BasicDefine.h"

typedef struct _QueueElement
{
    Element  _element;
    uint64_t _index;
    uint64_t _ts;   //timestamp

    _QueueElement()
    : _element()
    , _index(0)
    , _ts(0)
    {

    }
}QueueElement;

/**
 * It's thread safety circular queue, only support signal producer/consumer
 * unlock copy--especially used by large memory copy
 **/
class CircularQueue
{
public:
    CircularQueue(int queueLength);
    CircularQueue(int queueLength, Element* elements);
    ~CircularQueue();

    /**
     * @brief Push a element to the queue
     * @param e  element to push
     *        ms  millisecond to wait,
     *            -1 specify wait forever until the signals comes,
     *            0 specify not wait and return immediately anyway
     * @return true if sucess otherwise false 
     **/
    bool push(const QueueElement* e, int ms = -1);

    /**
     * @brief Pop a element frome the queue, 
     *        notice that a element won't be real poped from the queue 
     *        if the share consumers more then one
     * @param e  element that pop to
     *        ms  millisecond to wait,
     *            -1 specify wait forever until the signals comes,
     *            0 specify not wait and return immediately anyway
     * @return true if sucess otherwise false 
     **/
    bool pop(QueueElement* e, int ms = -1);

    /**
     * @brief Peek the queue rear and hold the rear index unchange
     * 
     * @param ms peek block time
     * @return QueueElement* peek result
     */
    QueueElement* peekRear(int ms = 0);

    /**
     * @brief update rear index after a peek finish
     *        you need guarantee the correctly calling
     */
    void updateRear();

    /**
     * @brief get queue full state
     * @return true if full otherwise false 
     **/
    bool full();

    /**
     * @brief get queue empty state
     * @return true if empty otherwise false 
     **/  
    bool empty();

    /**
     * Wake the block thread
     **/
    void wake();

    /**
     * remove all elements in the queue 
     **/
    void removeAll();


private:
    bool queueFull();
    bool queueEmpty();
    bool condWait(pthread_cond_t* cond, pthread_mutex_t* mutex, int ms);
    void copyElement(QueueElement* dst, const QueueElement* src);

private:
    pthread_mutex_t mMutex;
    pthread_cond_t mCond;
    int mFront;
    int mRear;
    int mLength;
    QueueElement* mQueue;

    //true: queue full; false: queue false
    bool mState;
    bool mFreeElement;

    bool mWait;
    bool mNoBlock;
};


#endif