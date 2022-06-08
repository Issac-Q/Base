#include <CircularQueue.h>
#include <string.h>
#include <unistd.h>

CircularQueue::CircularQueue(int queueLength)
: mFront(0)
, mRear(0)
, mLength(queueLength)
, mQueue(0)
, mState(false)
, mFreeElement(true)
, mNoBlock(false)
{
    pthread_mutex_init(&mMutex, NULL);
    pthread_condattr_t cattr;
    pthread_condattr_setclock(&cattr, CLOCK_MONOTONIC);
    pthread_cond_init(&mCond, &cattr);

    int byteSize = sizeof(QueueElement) * mLength;
    mQueue = (QueueElement*)malloc(byteSize);
    memset(mQueue, 0, byteSize);
}

CircularQueue::CircularQueue(int queueLength, Element* elements)
: mFront(0)
, mRear(0)
, mLength(queueLength)
, mQueue(0)
, mState(false)
, mFreeElement(false)
, mNoBlock(false)
{
    pthread_mutex_init(&mMutex, NULL);
    pthread_condattr_t cattr;
    pthread_condattr_setclock(&cattr, CLOCK_MONOTONIC);
    pthread_cond_init(&mCond, &cattr);

    int byteSize = sizeof(QueueElement) * mLength;
    mQueue = (QueueElement*)malloc(byteSize);
    int elementSize = sizeof(Element);
    for (int i = 0; i < mLength; i++) {
        memcpy(&(mQueue[i]._element), elements + i, elementSize);
    }
}

CircularQueue::~CircularQueue()
{
    pthread_mutex_destroy(&mMutex);
    pthread_cond_destroy(&mCond);

    if (mQueue) {
        if (mFreeElement) {
            for (int i = 0; i < mLength; i++) {
                if (mQueue[i]._element._data) {
                    free(mQueue[i]._element._data);
                    mQueue[i]._element._data = NULL;
                }
            }
        }
        free(mQueue);
        mQueue = NULL;
    }
}

bool CircularQueue::push(const QueueElement* e, int ms)
{
    if (!e) {
        // printf("warnning: push a NULL ptr\n");
        return false;
    }

    pthread_mutex_lock(&mMutex);

    //1. queue full and not wait, return false right now
    if (queueFull() && (ms == 0 || mNoBlock)) {
        pthread_mutex_unlock(&mMutex);
        return false;
    }

    //2. queue full and wait for ms
    while (queueFull()) {
        if(condWait(&mCond, &mMutex, ms)) {
            //wait fail because time out or error
            return false;
        }
    }

    if (mNoBlock) {
        pthread_mutex_unlock(&mMutex);
        return false;
    }

    //3. wait successed and begin no lock copy
    pthread_mutex_unlock(&mMutex);
    QueueElement *dstE = &mQueue[mRear];
    copyElement(dstE, e);

    //4. lock again after copy to awaken the block accesser and update mFront to finish one pop
    pthread_mutex_lock(&mMutex);
    if (queueEmpty()) {
        pthread_cond_broadcast(&mCond);
    }
    mRear = (mRear + 1) % mLength;
    if (mRear == mFront) {
        mState = true;
        // printf("queue full\n");
    }
    pthread_mutex_unlock(&mMutex);
    return true;
}

bool CircularQueue::pop(QueueElement* e, int ms)
{
    if (!e) {
        // printf("warnning: pop into a NULL ptr\n");
        return false;
    }

    pthread_mutex_lock(&mMutex);

    //1. queue empty and not wait, return false right now
    if (queueEmpty() && (ms == 0 || mNoBlock)) {
        pthread_mutex_unlock(&mMutex);
        return false;
    }

    //2. queue empty and wait for ms
    while (queueEmpty()) {
        if(condWait(&mCond, &mMutex, ms)) {
            //wait fialed because time out or error
            return false;
        }
    }

    if (mNoBlock) {
        pthread_mutex_unlock(&mMutex);
        return false;
    }

    //3. wait successed and begin no lock copy
    pthread_mutex_unlock(&mMutex);
    QueueElement *srcE = &mQueue[mFront];
    copyElement(e, srcE);

    //4. lock again after copy to awaken the block accesser and update mFront to finish one pop
    pthread_mutex_lock(&mMutex);
    if (queueFull()) {
        pthread_cond_broadcast(&mCond);
    }        
    mFront = (mFront + 1) % mLength;
    if (mFront == mRear) {
        mState = false;
        // printf("queue empty\n");
    }
    pthread_mutex_unlock(&mMutex);
    return true;
}

QueueElement* CircularQueue::peekRear(int ms)
{
    pthread_mutex_lock(&mMutex);

    //1. queue empty and not wait, return false right now
    if (queueEmpty() && (ms == 0 || mNoBlock)) {
        pthread_mutex_unlock(&mMutex);
        return nullptr;
    }

    //2. queue empty and wait for ms
    while (queueEmpty()) {
        if(condWait(&mCond, &mMutex, ms)) {
            //wait fialed because time out or error
            return nullptr;
        }
    }

    if (mNoBlock) {
        pthread_mutex_unlock(&mMutex);
        return nullptr;
    }

    //3. wait successed and begin no lock peek
    pthread_mutex_unlock(&mMutex);
    QueueElement *srcE = &mQueue[mFront];
    return srcE;
}

void CircularQueue::updateRear()
{
    //4. lock again after end of peek to awaken the block accesser and update mFront to finish one pop
    pthread_mutex_lock(&mMutex);
    if (queueEmpty()) {
        pthread_mutex_unlock(&mMutex);
        return;        
    }

    if (queueFull()) {
        pthread_cond_broadcast(&mCond);
    }
    mFront = (mFront + 1) % mLength;
    if (mFront == mRear) {
        mState = false;
        // printf("queue empty\n");
    }
    pthread_mutex_unlock(&mMutex);
}

bool CircularQueue::queueFull()
{
    return mState && mRear == mFront;
}

bool CircularQueue::queueEmpty()
{
    return !mState && mRear == mFront;
}

// bool CircularQueue::queueFull()
// {
//     return (mRear + 1) % mLength == mFront;
// }

// bool CircularQueue::queueEmpty()
// {
//     return mRear == mFront;
// }

bool CircularQueue::full()
{
    pthread_mutex_lock(&mMutex);
    bool ret = queueFull();
    pthread_mutex_unlock(&mMutex);
    return ret;
}

bool CircularQueue::empty()
{
    pthread_mutex_lock(&mMutex);
    bool ret = queueEmpty();
    pthread_mutex_unlock(&mMutex);
    return ret;
}

void CircularQueue::wake()
{
    // printf("wake1\n");
    pthread_mutex_lock(&mMutex);
    // printf("wake2\n");
    if (mWait) {
        mWait = false;        
        pthread_cond_broadcast(&mCond);
    }    
    mNoBlock = true;
    pthread_mutex_unlock(&mMutex);    
}

bool CircularQueue::condWait(pthread_cond_t* cond, pthread_mutex_t* mutex, int ms)
{
    if (ms < 0) {
        return pthread_cond_wait(cond, mutex);
    }
    else {
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        // printf("ms3:%d\n", now.tv_sec * 1000 + now.tv_nsec / 1000000);
        uint64_t totalMs = now.tv_nsec / 1000000 + ms;
        now.tv_sec += (totalMs / 1000);
        now.tv_nsec = (totalMs % 1000) * 1000000;
        mWait = true;
        int r = pthread_cond_timedwait(cond, mutex, &now);
        mWait = false;
        //!!!when pthread_cond_timedwait returns, it certainly lock the mutex
        if (r == ETIMEDOUT) {
            // clock_gettime(CLOCK_MONOTONIC, &now);
            // printf("ms4:%d\n", now.tv_sec * 1000 + now.tv_nsec / 1000000);
            pthread_mutex_unlock(&mMutex);
        }
        else if (r) {
            // printf("pthread_cond_timedwait return error:%d\n", r);
            pthread_mutex_unlock(&mMutex);
        }
        return r;
    }
}

void CircularQueue::copyElement(QueueElement* dst, const QueueElement* src)
{
    //copy data field
    if (src->_element._data && src->_element._len > 0) {
        if (!dst->_element._data) {
            dst->_element._data = malloc(src->_element._len);
        }
        else if (dst->_element._len < src->_element._len) {
            free(dst->_element._data);
            dst->_element._data = malloc(src->_element._len);
        }

        memcpy(dst->_element._data, src->_element._data, src->_element._len);
    }
    dst->_element._len = src->_element._len;

    //copy other field
    dst->_index = src->_index;
    dst->_ts = src->_ts;
}