#include "gsp.h"

#include "stdio.h"
#include <string>

#include "gsp-hid.h"


static std::string w2c(const wchar_t *pw)
{
    std::string val = "";
    if (!pw)
    {
        return val;
    }
    size_t size = wcslen(pw) * sizeof(wchar_t);
    char *pc = NULL;
    if (!(pc = (char*)malloc(size)))
    {
        return val;
    }
    size_t destlen = wcstombs(pc, pw, size);

    if (destlen ==(size_t)(0))
    {
        return val;
    }
    val = pc;
    delete pc;
    return val;
}

std::string enumerateAllHidPorts()
{
    std::string ports;
    struct hid_device_info *devs, *cur_dev;
    devs = hid_enumerate(0x10c4, 0x1819);
    cur_dev = devs;
    int n = 0;
    while (cur_dev)
    {
        ports += "//";
        ports += w2c(cur_dev->serial_number);
        ++n;
        cur_dev = cur_dev->next;
    }
    hid_free_enumeration(devs);
    return ports;
}

Gsp* createGsp(const std::string path, int baud)
{
    Gsp *gsp = NULL;
    std::string p = path;
    if (p.size() >= 6 && p.compare(0, 6, "hid://") == 0)
    {
        gsp = new GspHID();
        p = p.substr(6, p.size() - 6);
    }

    if (gsp->open(p, baud) == false)
    {
        delete gsp;
        gsp = NULL;
    }
    return gsp;
}

void destroyGsp(Gsp *gsp)
{
    delete(gsp);
}

