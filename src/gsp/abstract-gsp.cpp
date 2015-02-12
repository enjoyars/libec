#include "gsp.h"

AbstractGsp::AbstractGsp()
{
    _recvEvent = NULL;
    _isEventEnable = true;
}

AbstractGsp::~AbstractGsp()
{
}

void AbstractGsp::setRecvEvent(GspRecvEvent *recvEvent)
{
    _recvEvent = recvEvent;
}

void AbstractGsp::enableRecvEvent()
{
    if (_isEventEnable)
    {
        return;
    }
    _mutexEvent.unlock();
    //printf("enableRecvEvent !!!!!\n");
    _isEventEnable = true;
}

void AbstractGsp::disableRecvEvent()
{
    if (_isEventEnable == false)
    {
        return;
    }
    _mutexEvent.lock();
//    while (_mutexEvent.try_lock() == false)
//    {
//        tthread::this_thread::sleep_for(tthread::chrono::milliseconds(5));
//    }
    //printf("disableRecvEvent !!!!!\n");
    _isEventEnable = false;
}

void AbstractGsp::_dataRecvEvent(unsigned char *data, int length)
{
    if (_recvEvent != NULL)
    {
        _recvEvent->dataRecvEvent(this, data, length);
    }
}
