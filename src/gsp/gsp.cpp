#include "gsp.h"

#include "stdio.h"
#include <string>

#include "gsp-hid.h"

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
