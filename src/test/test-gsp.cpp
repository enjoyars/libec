#include <stdio.h>

#include "gsp.h"

int main(int argc, char **argv)
{
    Gsp *gsp = createGsp("hid://", 25600);
    destroyGsp(gsp);
    gsp = NULL;
    return 0;
}
