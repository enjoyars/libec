#include "gsp-hid.h"

#include <string>
#include <stdio.h>
#include <stdlib.h>

#include "../3rdparty/tthread/tinythread.h"

std::wstring c2w(const char *pc)
{
    std::wstring val = L"";

    if(NULL == pc)
    {
        return val;
    }
    //size_t size_of_ch = strlen(pc)*sizeof(char);
    //size_t size_of_wc = get_wchar_size(pc);
    size_t size_of_wc;
    size_t destlen = mbstowcs(0,pc,0);
    if (destlen ==(size_t)(-1))
    {
        return val;
    }
    size_of_wc = destlen+1;
    wchar_t * pw  = new wchar_t[size_of_wc];
    mbstowcs(pw,pc,size_of_wc);
    val = pw;
    delete pw;
    return val;
}

GspHID::GspHID()
{
    _device = NULL;
    hid_init();
}

GspHID::~GspHID()
{
    close();
    //hid_exit();
}

bool GspHID::open(const std::string path, int baud)
{
    close();

    if (path.size() > 0)
    {
        _device = hid_open(0x10c4, 0x1819, (wchar_t*)((c2w(path.c_str())).c_str()));
    }
    else
    {
        _device = hid_open(0x10c4, 0x1819, NULL);
    }

    if (_device == NULL)
    {
        close();
        return false;
    }

    hid_set_nonblocking(_device, 1);

    unsigned char buf[9];
    buf[0] = 0x50;
    buf[1] = baud >> 24;
    buf[2] = baud >> 16;
    buf[3] = baud >> 8;
    buf[4] = baud >> 0;
    buf[5] = 0x00; // No Parity
    buf[6] = 0x00; // No Flow Control
    buf[7] = 0x03; // 8 data bits
    buf[8] = 0x00; // Short Stop Bit
    hid_send_feature_report(_device, buf, 9); // Config UART

    buf[0] = 0x43;
    buf[1] = 0x03;
    hid_send_feature_report(_device, buf, 2); // Purge FIFOs

    buf[0] = 0x41;
    buf[1] = 0x01;
    hid_send_feature_report(_device, buf, 2); // Enable UART
    return true;
}

bool GspHID::close()
{
    if (_device != NULL)
    {
        hid_close(_device);
        _device = NULL;
    }
    return true;
}

bool GspHID::isOpened()
{
    return (_device != NULL);
}

int GspHID::read(unsigned char *data, int length, int timeout)
{
    int s = 0;
    int t = -1;
    while (1)
    {
        int r = _read((char*)data + s, length - s);
        if (r > 0)
        {
            s += r;
            if (s >= length)
            {
                break;
            }
        }
        else
        {
            ++t;
            if (t >= timeout)
            {
                break;
            }
            tthread::this_thread::sleep_for(tthread::chrono::milliseconds(1));
        }
        //printf("readData: %d\n", r);
    }
    return s;
}

int GspHID::write(const unsigned char *data, int length, int timeout)
{
    int s = 0;
    int t = -1;
    while (1)
    {
        int r = _write((char *)data + s, length - s);
        if (r > 0)
        {
            s += r;
            if (s >= length)
            {
                break;
            }
        }
        else
        {
            ++t;
            if (t >= timeout)
            {
                break;
            }
            tthread::this_thread::sleep_for(tthread::chrono::milliseconds(1));
        }
    }
    return s;
}

void GspHID::flush()
{
    unsigned char buf[100];
    buf[0] = 0x43;
    buf[1] = 0x03;
    hid_send_feature_report(_device, buf, 2); // Purge FIFOs
    return;
}

int GspHID::_read(void *data, int length)
{
    if (length > 63)
    {
        length = 63;
    }

    // buffer size must be 64.
    unsigned char buf[64];
    memset(buf, 0, 64);
    buf[0] = length;

    // buffer size must be 64. buf[0] changed after reading.
    int r = hid_read(_device, buf, 64);

    if(r <= 1)
    {
        return 0;
    }
    if (buf[0] < length)
    {
        r = buf[0];
    }
    else
    {
        r = length;
    }
    //r = std::min(buf[0], length);

    memcpy(data, buf + 1, r);
    return r;
}

int GspHID::_write(void *data, int length)
{
    if (length > 63)
    {
        length = 63;
    }

    // buffer size must be 64.
    unsigned char buf[64];
    memset(buf, 0, 64);
    buf[0] = length;
    memcpy(buf + 1, data, length);

    // buffer size must be 64. buf[0] changed after writing.
    int r = hid_write(_device, buf, 64);

    if (r >= length)
    {
        return buf[0];
    }
    return 0;
}
