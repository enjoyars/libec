#include "gsp-hid.h"

#include <string>
#include <stdio.h>
#include <stdlib.h>

#include "../3rdparty/tthread/tinythread.h"


#ifdef WIN32
    #include <Windows.h>
    double tictoc()
    {
        LARGE_INTEGER count, frequency;
        QueryPerformanceCounter(&count);
        QueryPerformanceFrequency(&frequency);
//        printf("@%f \n", double(count.QuadPart) / double(frequency.QuadPart / 1000));
        return double(count.QuadPart) / double(frequency.QuadPart / 1000);
    }
#else
    #include <sys/time.h>
    double tictoc()
    {
        timeval t;
        gettimeofday(&t, NULL);
        return (double(t.tv_sec) * 1000 + double(t.tv_usec) / 1000);
    }
#endif

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

//    buf[0] = 0x43;
//    buf[1] = 0x03;
//    hid_send_feature_report(_device, buf, 2); // Purge FIFOs

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
    double t = tictoc();
    for (s = s + max(0, _read(data + s, length - s));
         (s < length) && (tictoc() - t > timeout);
         s = s + max(0, _read(data + s, length - s)))
    {
        tthread::this_thread::sleep_for(tthread::chrono::microseconds(1000));
    }
    return s;
}

int GspHID::write(const unsigned char *data, int length, int timeout)
{
    int s = 0;
    double t = tictoc();
    for (s = s + max(0, _write(data + s, length - s));
         (s < length) && (tictoc() - t > timeout);
         s = s + max(0, _write(data + s, length - s)))
    {
        tthread::this_thread::sleep_for(tthread::chrono::microseconds(1000));
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

int GspHID::_read(unsigned char *data, int length)
{
    memset(data, 0, length);
    length = min(length, 63);

    // buffer size must be 64.
    unsigned char buf[64];
    memset(buf, 0, 64);
    buf[0] = length;

    // buffer size must be 64. buf[0] changed after reading.
    if(hid_read(_device, buf, length + 1) > 0)
    {
        memcpy(data, buf + 1, buf[0]);
        return buf[0];
    }
    return 0;
}

int GspHID::_write(const unsigned char *data, int length)
{
    length = min(length, 63);

    // buffer size must be 64.
    unsigned char buf[64];
    memset(buf, 0, 64);
    buf[0] = length;
    memcpy(buf + 1, data, length);

    // buffer size must be 64. buf[0] changed after writing.
    if (hid_write(_device, buf, length + 1) > 0)
    {
        return buf[0];
    }
    return 0;
}
