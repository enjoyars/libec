#include "gsp-hid.h"

#include <stdio.h>
#include <stdlib.h>

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

/*!
  * \brief Thread for esp implementation.
  */
static void gsp_thread(void *arg)
{
    GspHID *gsp = (GspHID*)arg;

    unsigned char buf[1000];
    while (1)
    {
        tthread::this_thread::sleep_for(tthread::chrono::milliseconds(1));
        if (gsp->_mutexFinished.try_lock())
        {
            gsp->_mutexFinished.unlock();
            break;
        }

        //printf("Thread \n");
        if (gsp->_mutexEvent.try_lock())
        {
            //printf("Thread lock\n");
            //printf("1. Thread Read\n");
            while (1)
            {
                int r = gsp->readDataNoWaiting(buf, sizeof(buf));
    //            for (int i = 0; i < r; ++i)
    //            {
    //                printf("data %d: %X\n", i, buf[i]);
    //            }
                if (r > 0)
                {
                    gsp->_dataRecvEvent(buf, r);
                }
                else
                {
                    break;
                }
            }
            //printf("2. Thread Read\n");
            gsp->_mutexEvent.unlock();
        }
    }

    //printf("Thread Return\n");
    return;
}

GspHID::GspHID()
{
    _device = NULL;
    _thread = NULL;
    _timeout = 10;
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

//    struct hid_device_info *devs, *cur_dev;
//    devs = hid_enumerate(0x10c4, 0x1819);
//    cur_dev = devs;
//    while (cur_dev)
//    {
//        printf("Device Found\n  type: %04hx %04hx\n  path: %s\n  serial_number: %ls",
//            cur_dev->vendor_id, cur_dev->product_id, cur_dev->path, cur_dev->serial_number);
//        printf("\n");
//        printf("  Manufacturer: %ls\n", cur_dev->manufacturer_string);
//        printf("  Product:      %ls\n", cur_dev->product_string);
//        printf("\n");
//        cur_dev = cur_dev->next;
//    }
//    hid_free_enumeration(devs);

    std::string str(path.c_str());
    if (path.empty())
    {
        _device = hid_open(0x10c4, 0x1819, NULL);
    }
    else
    {
        _device = hid_open(0x10c4, 0x1819, (wchar_t*)((c2w(str.c_str())).c_str()));
    }
    if (_device == NULL)
    {
        goto failsafe;
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

    _mutexFinished.lock();
    _thread = new tthread::thread(gsp_thread, this);
    if (_thread == NULL)
    {
        goto failsafe;
    }

    return true;
failsafe:
    close();
    return false;
}

bool GspHID::close()
{
    if (_thread != NULL)
    {
        _mutexFinished.try_lock();
        _mutexFinished.unlock();
        _thread->join();
        delete _thread;
        _thread = NULL;
    }

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

int GspHID::readData(void *data, int length)
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
            if (t >= _timeout)
            {
                break;
            }
            tthread::this_thread::sleep_for(tthread::chrono::milliseconds(1));
        }
        //printf("readData: %d\n", r);
    }
    return s;
}

int GspHID::writeData(const void *data, int length)
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
            if (t >= _timeout)
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

void GspHID::setTimeout(int ms)
{
    _timeout = ms;
}

int GspHID::readDataNoWaiting(void *data, int length)
{
    int s = 0;
    while (1)
    {
        int r = _read((char*)data + s, length - s);

        if (r > 0)
        {
            s += r;
        }
        else
        {
            break;
        }

        if (s >= length)
        {
            break;
        }
        //printf("readData: %d\n", r);
    }
    return s;
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

    _mutexDevice.lock();
    // buffer size must be 64. buf[0] changed after reading.
    int r = hid_read(_device, buf, 64);
    _mutexDevice.unlock();

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

    _mutexDevice.lock();
    // buffer size must be 64. buf[0] changed after writing.
    int r = hid_write(_device, buf, 64);
    _mutexDevice.unlock();

    if (r >= length)
    {
        return buf[0];
    }
    return 0;
}
