#!/usr/bin/python

import sys
import os
from ctypes import *

if __name__ == "__main__":
	os.chdir(sys.argv[1])
	
	print("QCOM reset BIN parse start\n")
	fd = open("reset_reason.txt", "w")
	res = os.popen("python parse_QCOM_reset.py RST_STAT.BIN PMIC_PON.BIN").read()
	fd.write(res)
	fd.close()
	print("QCOM reset BIN parse end\n")
	
	print("QCOM bmetrics BIN parse start\n")
	fd = open("bmetrics.txt", "w")
	res = os.popen("python parse_bmetrics_log_for_DDR_BIN.py DDRCS0_0.BIN").read()
	fd.write(res)
	fd.close()
	print("QCOM bmetrics BIN parse end\n")
	
	print("QCOM slog2 BIN split start\n")
	fd = open("slog2_split_log.txt", "w")
	res = os.popen("python split_slog2_log_for_DDR_BIN.py DDRCS0_1.BIN").read()
	fd.write(res)
	fd.close()
	print("QCOM slog2 BIN split end\n")

	print("QCOM dmesg BIN parse start\n")
	fd = open("dmesg_log.txt", "w")
	res = os.popen("python parse_dmesg_log_for_DDR_BIN.py DDRCS1_1.BIN").read()
	fd.write(res)
	print("QCOM dmesg BIN parse end\n")
	fd.close()