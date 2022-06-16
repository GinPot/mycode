#!/usr/bin/python

import sys
import os
import struct

if __name__ == "__main__":
	fd = open(sys.argv[1], "rb")
	print ("parse log start\n"),
	fd.seek(0x8000, 1)
	while 1:
		name_buf = fd.read(1*100)
		cnt=0
		name=''
		while struct.unpack('B'*1, name_buf[cnt])[0]!=0:
			name+=name_buf[cnt]
			cnt=cnt+1
		type = struct.unpack('I'*1,fd.read(4*1))
		time = struct.unpack('f'*2,fd.read(4*2))
		if type[0] == 1:
			print ("[ %s ]")%(name),
			print (" type %d")%(type),
			print ("start at %f, end at %f\n")%(time[0], time[1]),
		elif type[0] == 4:
			print ("[ %s ]")%(name),
			print (" type %d")%(type),
			if 0 != time[0]:
				print ("start at %f\n")%(time[0]),
			elif 0 != time[1]:
				print ("end at %f\n")%(time[1]),
		else:
			print ("parse log end\n"),
			break
	fd.close()