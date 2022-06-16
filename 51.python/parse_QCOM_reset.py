#!/usr/bin/python

import sys
import os
from ctypes import *

class GCC_RESET_STATUS(LittleEndianStructure):
    _pack_ = 1
    _fields_ = [
        ('SECURE_WDOG_EXPIRE_RESET',c_uint16,1),
		('PMIC_RESIN_RESET',		c_uint16,1),
		('MSM_TSENSE0_RESET',		c_uint16,1),
		('SRST_RESET',				c_uint16,1),
		('PROC_HALT_CTI',     		c_uint16,1),
		('MSM_TSENSE1_RESET', 		c_uint16,1),
    ]
    def decode(self, data):
        memmove(addressof(self), data, sizeof(self))
        return len(data)
		
class PON_RESON1(LittleEndianStructure):
    _pack_ = 1
    _fields_ = [
		('HARD_RESET', 	c_uint8,1),
		('SMPL', 		c_uint8,1),
		('RTC', 		c_uint8,1),
		('DC_CHG',     	c_uint8,1),
		('USB_CHG',		c_uint8,1),
		('PON1',		c_uint8,1),
		('CBLPWR_N',	c_uint8,1),
		('KPDPWR_N',	c_uint8,1),
    ]
    def decode(self, data):
        memmove(addressof(self), data, sizeof(self))
        return len(data)
		
class WARM_RESET_POFF_SOFT_RESET_REASON1(LittleEndianStructure):
    _pack_ = 1
    _fields_ = [
		('SOFT', 				c_uint8,1),
		('PS_HOLD', 			c_uint8,1),
		('PMIC_WD', 			c_uint8,1),
		('GP1',     			c_uint8,1),
		('GP2',					c_uint8,1),
		('KPDPWR_AND_RESIN',	c_uint8,1),
		('RESIN_N',				c_uint8,1),
		('KPDPWR_N',			c_uint8,1),
    ]
    def decode(self, data):
        memmove(addressof(self), data, sizeof(self))
        return len(data)

class POFF_REASON2(LittleEndianStructure):
    _pack_ = 1
    _fields_ = [
		('RESERVED',c_uint8,5),
		('UVLO',	c_uint8,1),
		('OTST3',	c_uint8,1),
        ('STAGE3',	c_uint8,1),
    ]
    def decode(self, data):
        memmove(addressof(self), data, sizeof(self))
        return len(data)
		
class SOFT_RESET_REASON1(LittleEndianStructure):
    _pack_ = 1
    _fields_ = [
        ('KPDPWR_N',			c_uint8,1),
		('RESIN_N',				c_uint8,1),
		('KPDPWR_AND_RESIN',	c_uint8,1),
		('GP2',					c_uint8,1),
		('GP1',     			c_uint8,1),
		('PMIC_WD', 			c_uint8,1),
		('PS_HOLD', 			c_uint8,1),
		('SOFT', 				c_uint8,1),
    ]
    def decode(self, data):
        memmove(addressof(self), data, sizeof(self))
        return len(data)
		
if __name__ == "__main__":
	fd = open(sys.argv[1], "rb")
	line = fd.read(4)
	ss = GCC_RESET_STATUS()
	ss.decode(line)
	print ("===============================")
	print ("RESET STATUS:")
	if (1 == ss.SECURE_WDOG_EXPIRE_RESET):
		print ("	watchdog reset triggered")
	if (1 == ss.PMIC_RESIN_RESET):
		print ("	PMIC resetin occurs")
	if (1 == ss.MSM_TSENSE0_RESET):
		print ("	temperature sensor0 activates")
	if (1 == ss.SRST_RESET):
		print ("	JTAG SRST triggered")
	if (1 == ss.PROC_HALT_CTI):
		print ("	PROC_HALT_CTI")
	if (1 == ss.MSM_TSENSE1_RESET):
		print ("	temperature sensor1 activates")
	fd.close()
	fd = open(sys.argv[2], "rb")
	print ("===============================")
	print ("PMIC STATUS:")
	print ("PON REASON1:")
	line = fd.read(1)
	ss = PON_RESON1()
	ss.decode(line)
	if (1 == ss.HARD_RESET):
		print ("	hard trigger reset")
	if (1 == ss.SMPL):
		print ("	power loss")
	if (1 == ss.RTC):
		print ("	RTC event")
	if (1 == ss.DC_CHG):
		print ("	wall charger insertion")
	if (1 == ss.USB_CHG):
		print ("	USB charger insertion")
	if (1 == ss.PON1):
		print ("	power on 1")
	if (1 == ss.CBLPWR_N):
		print ("	USB cable insertion")
	if (1 == ss.KPDPWR_N):
		print ("	keypad triggered")
	print ("WARM RESET REASON1:")
	line = fd.read(1)
	line = fd.read(1)
	ss = WARM_RESET_POFF_SOFT_RESET_REASON1()
	ss.decode(line)
	if (1 == ss.SOFT):
		print ("	soft trigger reset")
	if (1 == ss.PS_HOLD):
		print ("	PS_HOLD pulled low")
	if (1 == ss.PMIC_WD):
		print ("	PMIC watchdog triggered")
	if (1 == ss.GP1):
		print ("	general purpose use")
	if (1 == ss.GP2):
		print ("	genera2 purpose use")
	if (1 == ss.KPDPWR_AND_RESIN):
		print ("	keypad and resin triggered")
	if (1 == ss.RESIN_N):
		print ("	resin triggered")
	if (1 == ss.KPDPWR_N):
		print ("	keypad triggered")
	print ("POFF REASON1:")
	line = fd.read(1)
	line = fd.read(1)
	ss = WARM_RESET_POFF_SOFT_RESET_REASON1()
	ss.decode(line)
	if (1 == ss.SOFT):
		print ("	soft trigger reset")
	if (1 == ss.PS_HOLD):
		print ("	PS_HOLD pulled low")
	if (1 == ss.PMIC_WD):
		print ("	PMIC watchdog triggered")
	if (1 == ss.GP1):
		print ("	triggered by keypad reset1")
	if (1 == ss.GP2):
		print ("	triggered by keypad reset2")
	if (1 == ss.KPDPWR_AND_RESIN):
		print ("	keypad and resin triggered")
	if (1 == ss.RESIN_N):
		print ("	resin triggered")
	if (1 == ss.KPDPWR_N):
		print ("	keypad triggered")
	print ("POFF REASON2:")
	line = fd.read(1)
	ss = POFF_REASON2()
	ss.decode(line)
	if (1 == ss.UVLO):
		print ("	triggered by UVLO")
	if (1 == ss.OTST3):
		print ("	triggered by overtemp")
	if (1 == ss.STAGE3):
		print ("	triggered by stage3 reset")
	print ("SOFT RESET REASON1:")
	line = fd.read(1)
	ss = WARM_RESET_POFF_SOFT_RESET_REASON1()
	ss.decode(line)
	if (1 == ss.PS_HOLD):
		print ("	PS_HOLD pulled low")
	if (1 == ss.PMIC_WD):
		print ("	PMIC watchdog triggered")
	if (1 == ss.GP1):
		print ("	triggered by keypad reset1")
	if (1 == ss.GP2):
		print ("	triggered by keypad reset2")
	if (1 == ss.KPDPWR_AND_RESIN):
		print ("	keypad and resin triggered")
	if (1 == ss.RESIN_N):
		print ("	resin triggered")
	if (1 == ss.KPDPWR_N):
		print ("	keypad triggered")
	print ("==============================="),
	fd.close()