#! /bin/sh


echo "sleep 20s to check ais_be_server..."

sleep 20


check_num=0

while true
do
	processNum=`pidin a | grep ais_be_server | grep -v grep	| wc -l`
	if [ $processNum -eq 0 ];then
		echo "ais_be_server not running, try to start $check_num..."
		on -ae ais_be_server &
		let check_num++
		if [ $check_num -gt 3 ];then
			echo "try to start ais_be_server fail..."
			exit
		fi
	else
		echo "ais_be_server is running..."
		exit
	fi
	sleep 5
done
