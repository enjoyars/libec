#include "gsp.h"

#include <string>
#include <stdio.h>
#include <stdlib.h>

#include "gsp-hid.h"

static std::string w2c(const wchar_t * pw)
{
    std::string val = "";
    if(!pw)
    {
        return val;
    }
    size_t size= wcslen(pw)*sizeof(wchar_t);
    char *pc = NULL;
    if(!(pc = (char*)malloc(size)))
    {
        return val;
    }
    size_t destlen = wcstombs(pc,pw,size);

    if (destlen ==(size_t)(0))
    {
        return val;
    }
    val = pc;
    delete pc;
    return val;
}

Gsp::Gsp()
{
    _sp = NULL;
}

Gsp::~Gsp()
{
    close();
}

void Gsp::setRecvEvent(GspRecvEvent *recvEvent)
{
    if (_sp == NULL)
    {
        return;
    }
    _sp->setRecvEvent(recvEvent);
}

void Gsp::enableRecvEvent()
{
    if (_sp == NULL)
    {
        return;
    }
    _sp->enableRecvEvent();
}

void Gsp::disableRecvEvent()
{
    if (_sp == NULL)
    {
        return;
    }
    _sp->disableRecvEvent();
}

bool Gsp::open(const std::string port, const int baud)
{
    close();

    std::string path;

    if (port.size() >= 6 && port.compare(0, 6, "hid://") == 0)
    {
        path = port.substr(6, port.length() - 6);
        _sp = new GspHID();
    }

    if (_sp == NULL)
    {
        goto failsafe;
    }

    if (_sp->open(path.c_str(), baud) == true)
    {
        _sp->disableRecvEvent();
        _sp->setTimeout(10);
        _sp->flush();
        return true;
    }

failsafe:
    close();
    return false;
}

bool Gsp::close()
{
    if (_sp != NULL)
    {
        _sp->close();
        delete _sp;
        _sp = NULL;
    }
    return true;
}

bool Gsp::isOpened()
{
    if (_sp == NULL)
    {
        return false;
    }
    return _sp->isOpened();
}

int Gsp::readData(void *data, int length)
{
    if (_sp == NULL)
    {
        return -1;
    }
    return _sp->readData(data, length);
}

int Gsp::writeData(const void *data, int length)
{
    if (_sp == NULL)
    {
        return -1;
    }
    return _sp->writeData(data, length);
}

void Gsp::flush()
{
    if (_sp == NULL)
    {
        return;
    }
    _sp->flush();
}

void Gsp::setTimeout(int ms)
{
    if (_sp == NULL)
    {
        return;
    }
    if (ms < 0)
    {
        ms = 0;
    }
    _sp->setTimeout(ms);
}

bool Gsp::getHidSerialNumber(int index, std::string &str)
{
    struct hid_device_info *devs, *cur_dev;
    devs = hid_enumerate(0x10c4, 0x1819);
    cur_dev = devs;
    int i = 0;
    while (cur_dev)
    {
        if (index == i)
        {
            str = w2c(cur_dev->serial_number);
            hid_free_enumeration(devs);
            return true;
        }
        cur_dev = cur_dev->next;
        ++i;
    }

    hid_free_enumeration(devs);
    return false;
}
