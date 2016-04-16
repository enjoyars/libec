#ifndef __GENERAL_SERIAL_PORT_HID_H__
#define __GENERAL_SERIAL_PORT_HID_H__

#include "gsp.h"

#include <string.h>

#include "../3rdparty/hidapi/hidapi.h"

class GspHID : public Gsp
{
public:
    explicit GspHID();
    virtual ~GspHID();

    virtual bool open(const std::string path, int baud);
    virtual bool close();
    virtual bool isOpened();

    virtual int read(unsigned char *data, int length, int timeout);
    virtual int write(const unsigned char *data, int length, int timeout);
    virtual void flush();

private:
    int _read(void *data, int length);
    int _write(void *data, int length);

private:
    hid_device *_device;
};

#endif
