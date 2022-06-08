#include "SocketReceiver.h"
#include "CaptureLog.h"

SocketReceiver::SocketReceiver()
{

}

SocketReceiver::~SocketReceiver()
{

}

void SocketReceiver::onDataReceived(int boardId, void* data, int len)
{
    LOG_SOCKET_D("SocketReceiver::onDataReceived: boardId:%d, len:%d\n", boardId, len);
}