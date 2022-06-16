#!/usr/bin/python

import sys
import os
import struct

if __name__ == "__main__":
	fd=open(sys.argv[1], "rb")
	print ("split log start\n"),
	fd.seek(0x5400000, 1)
	data=fd.read(0xa00000)
	last_pos=0
	pos=0
	file_size=0
	while pos<len(data):
		last_pos=pos
		pos=data.find("SLOG2", pos+len("SLOG2"))
		if pos==-1:
			break
		file_size=pos-last_pos
		cnt=0
		flag_name=''
		while cnt<8:
			flag_name+=data[pos+0x56+cnt]
			cnt=cnt+1
		if flag_name!='slogger2':
			continue
		cnt=0
		file_name=''
		while struct.unpack('B'*1, data[pos+0x5f+cnt])[0]!=0:
			file_name+=data[pos+0x5f+cnt]
			cnt=cnt+1
		print ("start dump at address 0x%08x - flag %s - file %s - size %d\n")%(pos, flag_name, file_name, file_size),
		fd_w = open(file_name, "wb")
		cnt=0
		while cnt < file_size:
			fd_w.write(data[pos+cnt])
			cnt=cnt+1
		fd_w.close()
	print ("split log end\n"),
	fd.close()
