#!/usr/bin/python

import sys
import struct
import os
import zlib

def main():
    if sys.version_info > (3, 0):
        print("EEROR: please use python2!")
        return 1
    
    if len(sys.argv) == 1:
        print("usage: {} <gpt file path>".format(sys.argv[0]))
        return 1
    
    with open(sys.argv[1], "rb") as file:
        file_len = os.path.getsize(sys.argv[1])
        data = file.read(file_len)

        gpt_header_off = data.find("EFI PART")
        if gpt_header_off == -1:
            print("ERROR: it is not found gpt header!")
            return 1
			
        if 1 == gpt_partition_crc32_cal(file, gpt_header_off):
			return 1
        print("==== GPT header info. ====")
        number_partition_entry = parse_gpt_header(file, gpt_header_off, 0)
        if number_partition_entry == 0:
            return 1

        if 2 == parse_gpt_header(file, gpt_header_off, 1):
            lba_size = gpt_header_off
            part_off = 2 * lba_size
            print("==== MBR partition ====")
            if parse_protective_mbr(file, lba_size) == False:
                return 1
        else:
            lba_size = file_len - gpt_header_off
            part_off = 0
        
        print("==== base info. ====")
        print "file size is: %d" % file_len
        print "lba size is: %d" % lba_size
        print "gpt header offset size is: %d" % gpt_header_off
        print "partition offset is: %d\n\n" % part_off
		
        print("==== GPT partition ====")
        if parse_partition_entries(file, number_partition_entry, lba_size, part_off) == False:
            return 1

			
def parse_protective_mbr(file, lba_size):
    file.seek(0)
    mbr_data = file.read(512)
    if len(mbr_data) < 512:
        print("ERROR: error read MBR!")
        return False

    position = 446
    number_partition = 0

    while number_partition < 4:
        print('==== Partition {} ===='.format(number_partition+1))

        parse_partition_entry(mbr_data[position:position+16], lba_size)

        number_partition += 1
        position += 16

    return True


def parse_partition_entry(data, sector_size):
    fields = struct.unpack('<8B2L', data)

    sector = fields[2]
    cylinder = fields[3]
    
    print("Status of physical drive: {}".format(fields[0]))
    print("start CHS address:\n    head: {} sector: {} cylinder: {}"
        .format(fields[1], fields[2] & 0x3F, cylinder_format(fields[2], fields[3])))
    print("Partition type: {}".format(fields[4]))
    print("end CHS address:\n    head: {} sector: {} cylinder: {}"
        .format(fields[5], fields[6] & 0x3F, cylinder_format(fields[6], fields[7])))
    print("Number of sectors before the partition: {}".format(fields[8]))
    print("Number of sectors in partition: {}".format(fields[9]))
    if ((fields[2] & 0x3F) != 0) and ((fields[2] & 0x3F) != 0):
        start_addr = (fields[2] & 0x3F) * sector_size
        end_addr = (fields[3] & 0x3F) * sector_size
        part_size = end_addr - start_addr + 1
        if part_size > 0:
            print("Partition sumary:\n  start address: {} end address: {} size: {}"
                .format(hex(start_addr), hex(end_addr), unit_trans(part_size)))
    print('\n')    


def cylinder_format(part1, part2):
    return ((part1 & 0xC0) << 2) | part2


def parse_gpt_header(file, gpt_off, get_main_flag):
    file.seek(gpt_off)
    gpt_header = file.read(512)
    if len(gpt_header) < 512:
        print("ERROR: error read GPT header")
        return 0

    fields = struct.unpack('<8c4B3L4QI3H6BQ3L420c', gpt_header)

    if 1 == get_main_flag:
        return fields[29]
	
    print("Signature: {}".format(''.join(fields[0:8])))
    print("Revision: {} {} {} {}".format(fields[8], fields[9], fields[10], fields[11]))
    print("Header size: {}".format(fields[12]))
    print("CRC32: {:x}".format(fields[13]))
    print("Reserved: {}".format(fields[14]))
    print("Current LBA: {}".format(fields[15]))
    print("Backup LBA: {}".format(fields[16]))
    print("First usable LBA: {}".format(fields[17]))
    print("Last usable LBA: {}".format(fields[18]))
    print("Disk GUID: {}".format(guid_format(fields[19:29])))
    print("Starting LBA: {}".format(fields[29]))
    print("Number of partition entries: {}".format(fields[30]))
    print("Size of partition entry: {}".format(fields[31]))
    print("CRC32: {:x}".format(fields[32]))
    print("\n")
    	
    return fields[30]


def parse_partition_entries(file, number_partition_entry, lba_size, part_off):
    file.seek(part_off)
    partition_entry = 0
    while partition_entry < number_partition_entry:
        field_data = file.read(128)
        if len(field_data) < 128:
            print("ERROR: error read {} partition entry".format(partition_entry))
            return False

        if all(ord(x) == 0 for x in field_data):
            return True

        print("==== Partition entry {} ====".format(partition_entry))
        parse_gpt_partition_entry(field_data, lba_size)
        partition_entry += 1

    return True


def parse_gpt_partition_entry(data, lba_size):
    fields = struct.unpack('<I3H6BI3H6B3Q72c', data)
    print("Partition type GUID: {}".format(guid_format(fields[0:10])))
    print("Unique partition GUID: {}".format(guid_format(fields[10:20])))
    print("First LBA: {}".format(fields[20]))
    print("Last LBA: {}".format(fields[21]))
    print("Attribute flags: 0x{:x}".format(fields[22]))
    print("Partition name: {}".format(''.join(fields[23:])))
    if ((fields[20]) != 0) and (fields[21] != 0):
        start_addr = fields[20] * lba_size
        end_addr = fields[21] * lba_size
        part_size = end_addr - start_addr + 1
        if part_size > 0:
            print("Partition sumary:\n  name: {} \nstart address: {} end address: {} size: {}"
                .format(''.join(fields[23:]), hex(start_addr), hex(end_addr), unit_trans(part_size)))
    print("\n")


def guid_format(guid):
    return '{:08x}-{:04x}-{:04x}-{:04x}-{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}'.format(
        guid[0], guid[1], guid[2], guid[3], 
        guid[4], guid[5], guid[6], guid[7], guid[8], guid[9])


def unit_trans(size):
    def trans(integer, remainder, level):
        if integer >= 1024:
            remainder = integer % 1024
            integer //= 1024
            level += 1
            return trans(integer, remainder, level)
        else:
            return integer, remainder, level

    units = ['B', 'KB', 'MB', 'GB', 'TB', 'PB']
    integer, remainder, level = trans(size, 0, 0)
    if (level + 1) > len(units):
        return -1;
    return ('{} {}'.format(integer, units[level]))
	
def gpt_partition_crc32_cal(file, gpt_header_off):
    print "============================================"
    main_flag = 1
    file.seek(gpt_header_off)
    gpt_header1 = file.read(16)
    gpt_header2 = file.read(4)
    gpt_header3 = file.read(4)
    gpt_header4 = file.read(48)
    gpt_header5 = file.read(8)
    gpt_header6 = file.read(4)
    gpt_header7 = file.read(4)
    gpt_header8 = file.read(4)
    gpt_header9 = gpt_header1 + gpt_header3 + gpt_header3 + gpt_header4 + gpt_header5 + gpt_header6 + gpt_header7 + gpt_header8

    header_crc32 = struct.unpack('<I', gpt_header2)
    first_lba = struct.unpack('<Q', gpt_header5)
    num_part = struct.unpack('<I', gpt_header6)
    size_part = struct.unpack('<I', gpt_header7)
    part_crc32 = struct.unpack('<I', gpt_header8)
    num_byte_part = num_part[0] * size_part[0]
	
    if first_lba[0] != 2:
        main_flag = 0

    print "gpt header CRC32: 0x%x" % header_crc32[0]
    print "first LBA: %d" % first_lba[0] 
    print "number of partition table: %d" % num_part[0] 
    print "size of partition table: %d" % size_part[0] 
    print "partition table CRC32: 0x%x" % part_crc32[0] 
	
    #for i in gpt_header9:
        #print "%02x" % ord(i),
    #print " "
    cal_header_crc32 = (zlib.crc32(gpt_header9) & 0xffffffff)
    print "calcuted gpt header CRC32: 0x%x" % cal_header_crc32
	
    if 1 == main_flag:
        gpt_header = file.read(420)
        patition_data = file.read(num_byte_part)
        print "main gpt check"
    else:
        file.seek(0)
        patition_data = file.read(num_byte_part)
        print "backup gpt check"
    #for i in patition_data:
        #print "%02x" % ord(i),
    #print " "
    cal_part_crc32 = (zlib.crc32(patition_data) & 0xffffffff)
    print "calcuted partition table CRC32: 0x%x" % cal_part_crc32
	
    print "============================================"
    if (header_crc32[0] == cal_header_crc32) & (part_crc32[0] == cal_part_crc32):
        print "gpt CRC32 check successed."
        return 0
    else:
        print "gpt CRC32 check failed."
        return 1


if __name__ == '__main__':
	main()


