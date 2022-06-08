#ifndef TCP_MSG_EXTRACTOR_H
#define TCP_MSG_EXTRACTOR_H

#include <inttypes.h>

class TCPReceiver;

class TCPMsgExtractor
{
public:
    TCPMsgExtractor(uint64_t maxCacheSize, TCPReceiver* receiver);
    ~TCPMsgExtractor();
    
    void reset();
    void startExtract(const void* fragment, uint64_t len);

private:
    void increaseCacheBufIfNeed(void** cacheBuf, uint64_t curBytes, uint64_t needBytes);

    void* mCacheBuf;
    uint64_t mCacheSize;
    uint64_t mCurrentPacketLen;
    uint64_t mMaxCacheSize;

    TCPReceiver* mTCPReceiver;

    //static
    static uint64_t sLenEndByte;
};

#endif