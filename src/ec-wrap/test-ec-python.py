#!/usr/bin/env python
# coding=utf-8

#####################################################################
import sys
reload(sys)
sys.setdefaultencoding('utf-8')

import os
os.sys.path.append(os.path.dirname(__file__))
#####################################################################

import ec

print("Begin!")

port = ec.ec_openPort('hid://', 115200)
device = ec.ec_createDevice(port, ec.EC_DT_RF218) 
#ec.ec_openDevice(device, 1, 40)
ec.ec_setDeviceMode(device, ec.EC_DM_STATIC)
ec.ec_sleep(1000)

ec.ec_disableEvent(device)
data = ec.intArray(1024);
data[0] = 0x50;
data[1] = 0xC2;
data[2] = 0x92;
print(ec.ec_writePort(port, data, 3, 100))
ec.ec_sleep(300)
data[0] = 0
data[1] = 0
data[2] = 0
print(ec.ec_readPort(port, data, 1, 100))
print(data[0])
ec.ec_enableEvent(device)

ec.ec_closeDevice(device)
ec.ec_destroyDevice(device)
ec.ec_closePort(port)

print("Finish!")
