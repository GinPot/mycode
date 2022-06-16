#!/usr/bin/python

import sys
import os
from ctypes import *

class PHEAD(LittleEndianStructure):
    _pack_ = 1
    _fields_ = [
        ('ts_nsec', c_uint64),
        ('len',     c_uint16),
        ('text_len',c_uint16),
        ('dict_len',c_uint16),
        ('facility',c_uint8),
        ('flags',   c_uint8,5),
        ('level',   c_uint8,3),
    ]
    def decode(self, data):
        memmove(addressof(self), data, sizeof(self))
        return len(data)

headsize = 16

if __name__ == "__main__":
	fd = open(sys.argv[1], "rb")
	fd.seek(0x7F5C3000, 1)
	line = fd.read(headsize)
	while line:
		ss = PHEAD()
		ss.decode(line)
		if (ss.level > 7 or ss.len > 1024 or ss.text_len > ss.len or ss.len == 0 or ss.text_len == 0):
			break
		stream = fd.read(ss.text_len)
		print ("[ %05d.%06d ] ")%(ss.ts_nsec/1000000000,(ss.ts_nsec%1000000000)/1000),
		print stream
		offset = 4 - ss.len%4
		if (offset >= 4):
			offset = 0
		offset += ss.len - ss.text_len - headsize
		if (offset < 0 ):
			offset = 0
		fd.seek(offset, 1)
		line = fd.read(headsize)
	fd.close()