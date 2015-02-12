#ifndef __GENERAL_SERIAL_PORT_HID_H__
#define __GENERAL_SERIAL_PORT_HID_H__

#include "gsp.h"

#include <stdio.h>
#include <string.h>

#include "../3rdparty/hidapi/hidapi.h"


static void gsp_thread(void *arg);

class GspHID : public AbstractGsp
{
    friend void gsp_thread(void *arg);

public:
    explicit GspHID();
    virtual ~GspHID();

    virtual bool open(const std::string path, int baud);
    virtual bool close();
    virtual bool isOpened();

    virtual int readData(void *data, int length);
    virtual int writeData(const void *data, int length);

    virtual void flush();
    virtual void setTimeout(int ms);

private:
    int readDataNoWaiting(void *data, int length);
    int _read(void *data, int length);
    int _write(void *data, int length);

private:
    hid_device *_device;
    int _timeout;

    tthread::thread *_thread;
    tthread::mutex _mutexDevice;
    tthread::mutex _mutexFinished;
};

#endif
