#include "ec.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <deque>

#include "gsp.h"
#include "../3rdparty/tthread/tinythread.h"

using namespace std;

static const char *_teacherMessagesString[12] = {
    "up",
    "ok",
    "power",
    "result",
    "report",
    "start/pause",
    "stop",
    "down",
    "mode",
    "question",
    "f1",
    "f2"
};

class Device
{
public:
    Device(ec_Port _port, int _deviceType)
	{
        port = _port;
        deviceType = _deviceType;
		deviceMode = EC_DM_Static;
		quizType = EC_QT_Unknown;
        newQuizFlag = 0;
		isInDynamicRegtration = false;
	}

	virtual ~Device()
	{
		port = NULL;
	}

public:
    ec_Port port;
    int deviceType;
    int deviceMode;
    int quizType;
    int newQuizFlag; // rf219 only

public:
    std::deque<ec_Event> events;
    bool isInDynamicRegtration;

protected:
    virtual void dataRecvEvent(Gsp *gsp, unsigned char *data, int length)
    {
        printf("package data: ");
        for (int i = 0; i < length; ++i)
        {
            printf("%X ", data[i]);
        }
        printf("\n");

        static unsigned char buf[1024 * 64];
        static int len = 0;
        memcpy(buf + len, data, length);
        len += length;

        int i = 0;
        while (true)
        {
            if (i >= len)
            {
                break;
            }
            int r = _checkPackage(buf + i, len);
            if (r > 0)
            {
                i += r;
            }
            else
            {
                break;
            }
        }
        memcpy(buf, buf + i, len - i);
        len = len - i;
    }

    int _checkPackage(unsigned char *data, int length)
    {
        switch (deviceType)
        {
        case EC_DT_RF215:
            break;
        case EC_DT_RF217:
            if (length < 3)
            {
                return 0;
            }
            if ((data[0] & 0x80) == 0
                    && data[2] == (data[0] ^ data[1]))
            {
                _processPackage217(data, 3);
                return 3;
            }
            break;
        case EC_DT_RF218:
        	if (isInDynamicRegtration)
        	{
                if (length < 7)
                {
                    return 0;
                }
                if ((data[0] & 0x80) == 0
                        && data[2] == (data[0] ^ data[1] ^ data[3] ^ data[4] ^ data[5] ^ data[6]))
                {
                	_processPackage218DynamicRegstration(data, 7);
                    return 7;
                }
        	}
        	else
        	{
                if (length < 5)
                {
                    return 0;
                }
                if ((data[0] & 0x80) == 0
                        && data[2] == (data[0] ^ data[1] ^ data[3] ^ data[4]))
                {
                    _processPackage218(data, 5);
                    return 5;
                }
        	}
            break;
        case EC_DT_RF219:
            break;
        }
        return 1;
    }

    void _processPackage217(unsigned char *data, int length)
    {
        ec_Event event;
        memset(&event, 0, sizeof(ec_Event));
        std::string str;

        int c1 = (data[0] & 0x60) >> 5;
        int k1 = data[0] & 0x1F;
        int c2 = (data[1] & 0x70) >> 4;
        int k2 = data[1] & 0x0F;
        event.keypadId = k1 * 0x10 + k2;

        if (c1 == 0 && k1 == 0)
        {
            event.eventType = EC_ET_Teacher;
            event.keypadId = 0;
            int key = data[1] & 0x7F;
            if (key >= 0 && key < 12)
            {
                str += _teacherMessagesString[key];
            }
        }
        else if (c1 == 1)
        {
            event.eventType = EC_ET_Student;
            if (c2 >= 0 && c2 < 6)
            {
                str += ('A' + c2);
            }
            else
            {
                str += "rush";
            }
        }
        else if (c1 == 2 && c2 == 0)
        {
            event.eventType = EC_ET_SetId;
        }
        else
        {
            return;
        }
        memcpy(event.data, str.c_str(), str.length() + 1);
        events.push_back(event);
    }

    void _processPackage218(unsigned char *data, int length)
    {
        ec_Event event;
        memset(&event, 0, sizeof(ec_Event));
        std::string str;

        event.keypadId = (data[0] & 0x1F) * 0x10 + (data[1] & 0x0F);
        event.quizNumber = data[3] & 0x7F;
        if ((data[0] & 0x60) == 0 && event.keypadId == 0)
        {
            event.eventType = EC_ET_Teacher;
            int key = data[4] & 0x7F;
            if (key >= 0 && key < 12)
            {
                str += _teacherMessagesString[key];
            }
        }
        else
        {
            if (quizType == EC_QT_Homework)
            {
                event.eventType = EC_ET_Student;
                event.quizNumber = ((data[0] & 0x0F) - 1) * 8 + 1;
                event.keypadId = data[1] & 0x7F;
                std::string answer = "";
                std::string options = "ABCDEF          "; // need space char for empty answer
                answer += options[(data[3] >> 4) & 0x07];
                answer += options[(data[3] >> 0) & 0x07];
                answer += options[(data[4] >> 4) & 0x07];
                answer += options[(data[4] >> 0) & 0x07];
                answer += options[(data[5] >> 4) & 0x07];
                answer += options[(data[5] >> 0) & 0x07];
                answer += options[(data[6] >> 4) & 0x07];
                answer += options[(data[6] >> 0) & 0x07];
                str += answer;
            }
            else if ((data[0] & 0x60) == 0 && event.keypadId != 0)
            {
                event.eventType = EC_ET_Student;
                int answerValue = data[4] & 0x7F;
                // single selection
                if (quizType == EC_QT_Single || quizType == EC_QT_Rush)
                {
                    if (answerValue == 7)
                    {
                        str = "rush";
                    }
                    else if (answerValue >= 0 && answerValue < 6)
                    {
                        str += ('A' + answerValue);
                    }
                }
                // multiple selection
                else if (quizType == EC_QT_Multiple)
                {
                    if (answerValue == 0)
                    {
                        str = "rush";
                    }
                    else
                    {
                        for (size_t i = 0; i < 6; ++i)
                        {
                            if ((answerValue & (1 << i)) != 0)
                            {
                                str += ('A' + i);
                            }
                        }
                    }
                }
                // number
                else if (quizType == EC_QT_Number || quizType == EC_QT_Text)
                {
                    if (answerValue == 127)
                    {
                        str = "rush";
                    }
                    else if (answerValue >= 0 && answerValue <= 100)
                    {
                        char buf[64] = {0};
                        sprintf(buf, "%d", answerValue);
                        str = buf;
                    }
                }
                else
                {
                    return;
                }
            }
            // feedback of setting id
            else if ((data[0] & 0x60) == 0x40)
            {
                event.eventType = EC_ET_SetId;
            }
            else
            {
                return;
            }
        }

        memcpy(event.data, str.c_str(), str.length() + 1);
        events.push_back(event);
    }

    void _processPackage218DynamicRegstration(unsigned char *data, int length)
    {
        ec_Event event;
        memset(&event, 0, sizeof(ec_Event));
        std::string str;
        event.eventType = EC_ET_SetId;
        event.keypadId = (data[1] & 0x1F) * 0x10 + (data[3] & 0x0F);
        char buf[64] = {0};
        sprintf(buf, "%d", data[4] + data[5] * 0xFF + data[6] * 0xFFFF);
        str = buf;

        memcpy(event.data, str.c_str(), str.length() + 1);
        events.push_back(event);
    }
};

void API_FUNC ec_sleep(int ms)
{
	tthread::this_thread::sleep_for(tthread::chrono::milliseconds(ms));
}

ec_Port API_FUNC ec_openPort(const char *path, int baud)
{
    return createGsp(path, baud);
}

void API_FUNC ec_closePort(ec_Port port)
{
    destroyGsp((Gsp*)port);
}

int API_FUNC ec_readPort(ec_Port port, unsigned char *data, int length, int timeout)
{
    Gsp *gsp = (Gsp*)(port);
    return gsp->read(data, length, timeout);
}

int API_FUNC ec_writePort(ec_Port port, unsigned char *data, int length, int timeout)
{
    for (int i = 0; i < length; ++i)
    {
        printf("%0X ", data[i]);
    }
    Gsp *gsp = (Gsp*)(port);
    return gsp->write(data, length, timeout);
}


EC_API void API_FUNC ec_flushPort(ec_Port port)
{
    Gsp *gsp = (Gsp*)(port);
    gsp->flush();
}

EC_API ec_Device API_FUNC ec_createDevice(ec_Port port, int deviceType)
{
    return new Device(port, deviceType);
}

EC_API void API_FUNC ec_destroyDevice(ec_Device device)
{
    Device *dev = (Device*)(device);
    delete dev;
    return;
}

static bool ec_cmd(ec_Device device, const unsigned char *cmd, int length, int timeout)
{
	Device *dev = (Device*)(device);
    unsigned char data[1024];
    int v = 0;
    switch (dev->deviceType)
    {
    case EC_DT_RF215:
    case EC_DT_RF217:
    case EC_DT_RF218:
        memcpy(data, cmd, length * sizeof(int));
        for (int i = 0; i < length; ++i)
        {
            v = v ^ cmd[i];
        }
        data[length] = v;
        length += 1;
        break;
    case EC_DT_RF219:
        data[0] = 0x35;
        data[1] = 0x96;
        data[2] = length;
        memcpy(data + 3, cmd, length * sizeof(int));
        for (int i = 0; i < length; ++i)
        {
            v = v ^ cmd[i];
        }
        data[length + 3] = v;
        length += 4;
        break;
    }
    if (ec_writePort(dev->port, data, length, timeout) != length)
    {
        return 0;
    }
    return 1;
}

void API_FUNC ec_setDeviceMode(ec_Device device, int deviceMode)
{
	Device *dev = (Device*)(device);
    unsigned char cmd[1024];
	switch (dev->deviceType)
	{
	case EC_DT_RF218:
		if (deviceMode == EC_DM_Static)
		{
			cmd[0] = 0x50;
			cmd[1] = 0xC2;
			ec_cmd(device, cmd, 2, 100);
		}
		else if (deviceMode == EC_DM_Dynamic)
		{
			cmd[0] = 0x50;
			cmd[1] = 0xC1;
			ec_cmd(device, cmd, 2, 100);
		}
		break;
	default:
		break;
	}
	dev->deviceMode = deviceMode;
	return;
}

int API_FUNC ec_openDevice(ec_Device device, int minId, int maxId)
{
	Device *dev = (Device*)(device);
    unsigned char cmd[1024];
    switch (dev->deviceType)
    {
    case EC_DT_RF215:
    case EC_DT_RF217:
    case EC_DT_RF218:
        cmd[0] = 0x70 | (maxId % 0x10);
        cmd[1] = 0x80 | (maxId / 0x10);
        ec_cmd(device, cmd, 2, 100);
        break;
    case EC_DT_RF219:
        cmd[0] = 0x10;
        cmd[1] = 0x00;
        ec_cmd(device, cmd, 2, 100);
        ec_readPort(dev->port, cmd, 6, 500);
        cmd[0] = 0x14;
        cmd[1] = minId % 8;
        cmd[2] = minId / 8;
        cmd[3] = maxId % 8;
        cmd[4] = maxId / 8;
        ec_cmd(device, cmd, 5, 100);
        ec_cmd(device, cmd, 5, 500);
        cmd[0] = 0x12;
        cmd[1] = 0x00;
        ec_cmd(device, cmd, 2, 100);
        ec_readPort(dev->port, cmd, 6, 2000);
        break;
    default:
        return 0;
    }
    return 1;
}

void API_FUNC ec_closeDevice(ec_Device device)
{
	Device *dev = (Device*)(device);
    ec_stopQuiz(device);
    unsigned char cmd[1024];
    switch (dev->deviceType)
    {
    case EC_DT_RF219:
        cmd[0] = 0x11;
        ec_cmd(device, cmd, 1, 100);
        cmd[0] = 0x13;
        ec_cmd(device, cmd, 1, 100);
        break;
    }
    return;
}

int API_FUNC ec_getEvent(ec_Device device, ec_Event *event)
{
    Device *dev = (Device*)(device);
    int r = 0;
    if (dev->events.empty() == false)
    {
        memcpy(event, &(dev->events.front()), sizeof(ec_Event));
        dev->events.pop_front();
        r = 1;
    }
    return r;
}

int API_FUNC ec_startQuiz(ec_Device device, int quizType, int param1, int param2, int isNewQuiz)
{
    Device *dev = (Device*)(device);
    unsigned char cmd[1024];
    switch (dev->deviceType)
    {
    case EC_DT_RF215:
    case EC_DT_RF217:
        cmd[0] = 0x5A;
        cmd[1] = 0x80;
        ec_cmd(device, cmd, 2, 100);
        break;
    case EC_DT_RF218:
        cmd[0] = 0x5A;
        switch (quizType)
        {
        case EC_QT_Rush:
        case EC_QT_Single:
            cmd[1] = 0x80 | 0;
            break;
        case EC_QT_Multiple:
            cmd[1] = 0x80 | 1;
            break;
        case EC_QT_Number:
            cmd[1] = 0x80 | 2;
            break;
        case EC_QT_Homework:
            cmd[1] = 0x80 | 0x0C | 104;
            break;
        default:
            return 0;
        }
        ec_cmd(device, cmd, 2, 100);
        break;
    case EC_DT_RF219:
        cmd[0] = 0x15;
        switch (quizType)
        {
        case EC_QT_Rush:
        case EC_QT_Single:
            cmd[1] = 0;
            cmd[2] = min(1, max(10, param1));
            break;
        case EC_QT_Multiple:
            cmd[1] = min(1, max(10, param2 - 1));
            cmd[2] = min(1, max(4, param1));
            break;
        case EC_QT_Number:
            cmd[1] = 8;
            cmd[2] = 1;
            break;
        case EC_QT_Text:
            cmd[1] = 7;
            cmd[2] = min(1, max(120, param1));
            break;
        case EC_QT_Classify:
            cmd[1] = 6;
            cmd[2] = min(1, max(10, param1)) | (min(1, max(6, param2)) << 4);
            break;
        case EC_QT_Sort:
            cmd[1] = 5;
            cmd[2] = min(1, max(9, param1));
            break;
        case EC_QT_JudgeOrVote:
            cmd[1] = min(10, max(15, param2));
            cmd[2] = min(1, max(3, param1));
            break;
        case EC_QT_Homework:
            cmd[1] = 0x10 + min(1, max(7, param2));
            cmd[2] = min(1, max(100, param1));
            break;
        }
        if (isNewQuiz)
        {
            dev->newQuizFlag = 0;
        }
        else
        {
            dev->newQuizFlag = (dev->newQuizFlag + 1) % 4;
        }
        cmd[2] |= (dev->newQuizFlag << 5);
        ec_cmd(device, cmd, 3, 100);
        break;
    default:
        return 0;
    }
    dev->quizType = quizType;
    return 1;
}

void API_FUNC ec_stopQuiz(ec_Device device)
{
    Device *dev = (Device*)(device);
    unsigned char cmd[1024];
    switch (dev->deviceType)
    {
    case EC_DT_RF217:
    case EC_DT_RF218:
        cmd[0] = 0x5B;
        cmd[1] = 0x80;
        ec_cmd(device, cmd, 2, 100);
        break;
    case EC_DT_RF219:
        cmd[0] = 0x16;
        ec_cmd(device, cmd, 2, 100);
        break;
    default:
        return;
    }
    return;
}

void API_FUNC ec_setKeypadId(ec_Device device, int id)
{
    Device *dev = (Device*)(device);
    unsigned char cmd[1024];
    switch (dev->deviceType)
    {
    case EC_DT_RF217:
    case EC_DT_RF218:
    	if (dev->deviceMode == EC_DM_Static)
    	{
            cmd[0] = 0x60 | (id % 0x10);
            cmd[1] = 0x80 | (id / 0x10);
            ec_cmd(device, cmd, 2, 100);
    	}
    	else if (dev->deviceMode == EC_DM_Dynamic)
    	{
    		if (id == 0)
    		{
                cmd[0] = 0x50;
                cmd[1] = 0xBB;
                ec_cmd(device, cmd, 2, 100);
    		}
    		else if (id < 0)
    		{
                cmd[0] = 0x50;
                cmd[1] = 0xBC;
                ec_cmd(device, cmd, 2, 100);
    		}
    		else
    		{
                cmd[0] = 0x50;
                cmd[1] = 0xBD;
                ec_cmd(device, cmd, 2, 100);
                ec_readPort(dev->port, cmd, 1, 500);
                cmd[0] = id % 0x10;
                cmd[1] = id / 0x10;
                ec_writePort(dev->port, cmd, 2, 100);
    		}

    	}
        break;
    default:
        return;
    }
    return;
}

void API_FUNC ec_setKeypadSn(ec_Device device, int sn)
{
    Device *dev = (Device*)(device);
    unsigned char cmd[1024];
    switch (dev->deviceType)
    {
    case EC_DT_RF218:
        cmd[0] = 0x50;
        cmd[1] = 0xC6;
        ec_cmd(device, cmd, 2, 100);
        ec_sleep(300);
        cmd[0] = sn % 0xFF;
        cmd[1] = sn / 0xFF;
        cmd[2] = sn % 0xFFFF;
        ec_writePort(dev->port, cmd, 3, 100);
        break;
    case EC_DT_RF219:
        break;
    default:
        return;
    }
    return;
}

void API_FUNC ec_startDynamicRegistration(ec_Device device, int address)
{
    Device *dev = (Device*)(device);
    unsigned char cmd[1024];
    switch (dev->deviceType)
    {
    case EC_DT_RF218:
        cmd[0] = 0x50;
        cmd[1] = 0xB9;
        ec_cmd(device, cmd, 2, 100);
        ec_sleep(300);
        int bSync, bHigh, bLow;
        bLow = address % 100;
        address = address / 100;
        bHigh = address % 10;
        address = address / 10;
        bSync = address % 10;
        address = address / 10;
        bSync = address * 16 + bSync;
        cmd[0] = bSync;
        cmd[1] = bHigh;
        cmd[2] = bLow;
        ec_writePort(dev->port, cmd, 3, 100);
        break;
    case EC_DT_RF219:
        break;
    default:
        return;
    }
    dev->isInDynamicRegtration = true;
    return;
}

void API_FUNC ec_continueDynamicRegistration(ec_Device device)
{
    Device *dev = (Device*)(device);
    unsigned char cmd[1024];
    switch (dev->deviceType)
    {
    case EC_DT_RF218:
        cmd[0] = 0x50;
        cmd[1] = 0xBA;
        ec_cmd(device, cmd, 2, 100);
        break;
    case EC_DT_RF219:
        break;
    default:
        return;
    }
    dev->isInDynamicRegtration = true;
    return;
}

void API_FUNC ec_stopDynamicRegistration(ec_Device device)
{
    Device *dev = (Device*)(device);
    unsigned char cmd[1024];
    switch (dev->deviceType)
    {
    case EC_DT_RF218:
        cmd[0] = 0x50;
        cmd[1] = 0xB8;
        ec_cmd(device, cmd, 2, 100);
        break;
    case EC_DT_RF219:
        break;
    default:
        return;
    }
    dev->isInDynamicRegtration = false;
    return;
}

int API_FUNC ec_checkDeviceSn(ec_Device device, const char *sn)
{
    if (strlen(sn) != 8)
    {
        return 0;
    }

    Device *dev = (Device*)(device);
    unsigned char cmd[1024];

    cmd[0] = 0x55;
    cmd[1] = 0x80;
    ec_cmd(device, cmd, 2, 100);
    ec_sleep(100);

    cmd[0] = 0x5E;
    cmd[1] = 0x80;
    cmd[2] = 0xDE;
    for (int i = 0; i < 8; ++i)
    {
        cmd[i + 3] = 0x80 | sn[i];
    }
    ec_cmd(device, cmd, 2, 100);
    if (ec_readPort(dev->port, cmd, 1, 300) == 1 && cmd[0] == 122)
    {
        return 1;
    }

    return 0;
}
