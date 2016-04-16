#include <stdio.h>
#include <string.h>
#include <string>

#include "ec.h"


int main(int argc, char **argv)
{
    if (argc != 3)
    {
        printf("Name: rf21x-test \n");
        printf("Usage: rf21x-test DeviceType PortPath \n");
        printf("  Usage Example: rf21x-test rf217 hid:// \n");
        printf("  Usage Example: rf21x-test rf217 hid://0007800D \n");
        printf("  Usage Example: rf21x-test rf217 serial:///dev/ttyUSB0 \n");
        printf("  Usage Example: rf21x-test rf217 tcp://192.168.1.10:9000+hid:// \n");
        printf("Parameters:\n");
        printf("  DeviceType: rf217, rf218, rf219, rf215. \n");
        printf("  PortPath: PortType://PortAddress. \n");
        printf("    PortType: hid, serial, tcp \n");
        printf("    PortAddress: port address \n");
        printf("************************************************************\n");
        printf("Press any key to exit.\n");
        while (1)
        {
            if (getchar() > 0)
            {
                break;
            }
        }
        return 0;
    }

    // User can use serial number as address like "hid://0007800D" to select a hid device to be opened.
//    printf("Start listing all current HID device:\n");
//    char buf[100];
//    int i = 0;
//    while (rf21x_getHidSerialNumber(i, buf, sizeof(buf)))
//    {
//        printf("HID Device %d: %s\n", i, buf);
//        ++i;
//    }
//    printf("Finish listing all current HID device.\n\n");

    char *typeName = argv[1];
    int deviceType = EC_DT_Unknown;
    if (strcmp(typeName, "rf217") == 0)
    {
        deviceType = EC_DT_RF217;
    }
    else if (strcmp(typeName, "rf218") == 0)
    {
        deviceType = EC_DT_RF218;
    }
    else if (strcmp(typeName, "rf219") == 0)
    {
        deviceType = EC_DT_RF219;
    }
    else if (strcmp(typeName, "rf215") == 0)
	{
        deviceType = EC_DT_RF215;
	}
	else
    {
        printf("DeviceType error.\n");
    }

    ec_Port port = ec_openPort("hid://", 115200);
    if (port == NULL)
    {
        printf("Port Error. \n");
        return 0;
    }

//    char cmd[1024];
//
//    cmd[0] = 0x50;
//    cmd[1] = 0xBE;
//    ec_cmd(port, deviceType, cmd, 2, 100);
//    ec_sleep(1000);

//    cmd[0] = 0x50;
//    cmd[1] = 0xC1;
//    ec_cmd(port, deviceType, cmd, 2, 100);
//    ec_sleep(1000);
//
//    cmd[0] = 0x50;
//    cmd[1] = 0xB9;
//    ec_cmd(port, deviceType, cmd, 2, 100);
//    ec_sleep(1000);
//
//    cmd[0] = 0x01;
//    cmd[1] = 0x02;
//    cmd[2] = 0x03;
//    ec_writePort(port, cmd, 3, 100);
//    ec_sleep(1000);

//    return 0;

    ec_Device device = ec_createDevice(port, deviceType);

    ec_setDeviceMode(device, EC_DM_Dynamic);
    ec_sleep(500);

    ec_startDynamicRegistration(device, 11112);

//    ec_openDevice(device, 1, 10);
//    ec_sleep(500);
//
//    ec_startQuiz(device, EC_QT_Single, 1, 1, 1);

//    ec_setKeypadId(device, 1);
//    ec_sleep(1000);

    ec_Event event;
    for (int i = 0; i < 100; ++i)
    {
        if (ec_getEvent(device, &event) == 0)
        {
        	ec_sleep(100);
            continue;
        }
        printf("Event: %d (%d, %d, %d, %d): %s \n",
        		event.keypadId, event.eventType, event.quizType, event.quizNumber, event.timeStamp,
				event.data);
    }

    ec_stopDynamicRegistration(device);

    ec_stopQuiz(device);
    ec_closeDevice(device);
    ec_destroyDevice(&device);
    ec_closePort(&port);

    printf("Finish�� \n");

    return 0;
}