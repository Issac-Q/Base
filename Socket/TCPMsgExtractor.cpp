#include "TCPMsgExtractor.h"
#include "IOProtocol.h"
#include "TCPReceiver.h"
#include <stdio.h>
#include "CaptureLog.h"

uint64_t TCPMsgExtractor::sLenEndByte = 0;

TCPMsgExtractor::TCPMsgExtractor(uint64_t maxCacheSize, TCPReceiver* l)
: mMaxCacheSize(maxCacheSize)
, mCacheBuf(nullptr)
, mTCPReceiver(l)
{
    if (sLenEndByte == 0) {
        uint64_t start;
        uint8_t bytes;
        IOProtocol::packetLenProtocol(&start, &bytes);
        sLenEndByte = start + bytes;
    }
    reset();
}

TCPMsgExtractor::~TCPMsgExtractor()
{
    if (mCacheBuf) {
        free(mCacheBuf);
        mCacheBuf = nullptr;
    }
}


void TCPMsgExtractor::reset()
{
    mCacheSize = 0;
    mCurrentPacketLen = 0;
    if (!mCacheBuf) {
        mCacheBuf = malloc(mMaxCacheSize);
    }
}

void TCPMsgExtractor::startExtract(const void* fragment, uint64_t len)
{
    //A fragment may contains a part of a message 
    //or may contains many messages's section part
    if (!fragment && len) {
        LOG_SOCKET_E("startExtract invald data\n");
        return;
    }
    
    LOG_SOCKET_D("intput len:%d\n", len);

    uint64_t offset = 0;   //packet start offset in fragment
    uint64_t fragmentLen = len; //rest len of fragment
    uint64_t totalLen = mCacheSize + fragmentLen; //rest len of cacheBuf + fragment

    while(fragmentLen) {
        if (totalLen < sLenEndByte) {
            //case 1
            mCurrentPacketLen = 0;
            uint64_t sectionRestLenBeforeLen = fragmentLen;
            increaseCacheBufIfNeed(&mCacheBuf, mMaxCacheSize, mCacheSize + sectionRestLenBeforeLen);
            memcpy(mCacheBuf + mCacheSize, fragment + offset, sectionRestLenBeforeLen);
            mCacheSize += sectionRestLenBeforeLen;
            LOG_SOCKET_D("aaa cacheSzie:%d\n", mCacheSize);
            break;
        }
        else {
            if (mCurrentPacketLen == 0) {
                if (mCacheSize > 0) {
                    //copy the rest "length" occupies bytes to get packLength"
                    uint64_t restLenOccupyLen = sLenEndByte - mCacheSize;
                    increaseCacheBufIfNeed(&mCacheBuf, mMaxCacheSize, mCacheSize + restLenOccupyLen);
                    memcpy(mCacheBuf + mCacheSize, fragment, restLenOccupyLen);
                    mCacheSize += restLenOccupyLen;
                    offset += restLenOccupyLen;
                    fragmentLen -= restLenOccupyLen;
                    mCurrentPacketLen = IOProtocol::packLength(mCacheBuf);
                    LOG_SOCKET_D("bbb mCurrentPacketLen:%d\n", mCurrentPacketLen);
                }
                else {
                    mCurrentPacketLen = IOProtocol::packLength(fragment + offset);
                    LOG_SOCKET_D("ccc mCurrentPacketLen:%d\n", mCurrentPacketLen);
                }
            }
            
            if (totalLen < mCurrentPacketLen) {
                //case 2
                uint64_t sectionRestPacket = fragmentLen;
                increaseCacheBufIfNeed(&mCacheBuf, mMaxCacheSize, mCacheSize + sectionRestPacket);
                memcpy(mCacheBuf + mCacheSize, fragment + offset, sectionRestPacket);
                mCacheSize += sectionRestPacket;
                LOG_SOCKET_D("ddd mCacheSize:%d\n", mCacheSize);
                break;
            }
            else {
                //case 3              
                if (mCacheSize > 0) {
                    //case 3.1
                    uint64_t restPacketLen = mCurrentPacketLen - mCacheSize;
                    increaseCacheBufIfNeed(&mCacheBuf, mMaxCacheSize, mCacheSize + restPacketLen);
                    memcpy(mCacheBuf + mCacheSize, fragment + offset, restPacketLen);
                    mCacheSize += restPacketLen;
                    offset += restPacketLen;
                    fragmentLen -= restPacketLen;
                    // onMessage(mCacheBuf, mCacheSize);
                    LOG_SOCKET_D("1111---parketLeng:%d, targetID:%d, msgType:%d-------------\n", IOProtocol::packLength(mCacheBuf), IOProtocol::messageFromId(mCacheBuf), IOProtocol::messageType(mCacheBuf));
                    if (mTCPReceiver) {
                        mTCPReceiver->onTCPMsgReceive(mCacheBuf, mCacheSize);
                    }
                    mCacheSize = 0;
                }
                else {
                    //case 3.2
                    // onMessage(fragment + offset, mCurrentPacketLen);
                    LOG_SOCKET_D("2222---parketLeng:%d, targetID:%d, msgType:%d-------------\n", IOProtocol::packLength(fragment + offset), IOProtocol::messageFromId(fragment + offset), IOProtocol::messageType(fragment + offset));
                    if (mTCPReceiver) {
                        mTCPReceiver->onTCPMsgReceive(fragment + offset, mCurrentPacketLen);
                    }                    
                    offset += mCurrentPacketLen;
                    fragmentLen -= mCurrentPacketLen;
                }

                totalLen -= mCurrentPacketLen;
                mCurrentPacketLen = 0;
                LOG_SOCKET_D("eee totalLen:%d, offset:%d, fragmentLen:%d\n", totalLen, offset, fragmentLen);
            }
        }
    } 
}

void TCPMsgExtractor::increaseCacheBufIfNeed(void** cacheBuf, uint64_t curBytes, uint64_t needBytes)
{
    if (cacheBuf && *cacheBuf && curBytes < needBytes) {      
        LOG_SOCKET_D("TCPMsgExtractor::increaseCacheBufIfNeed: curBytes:%d, needBytes:%\n", curBytes, needBytes);
        void* tmp = malloc(needBytes);
        memcpy(tmp, *cacheBuf, curBytes);
        free(*cacheBuf);
        *cacheBuf = tmp;
    }
}