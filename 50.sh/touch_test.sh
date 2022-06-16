
num=1

while true
do

#home
sendevent /dev/input/event0 3 57 49
sendevent /dev/input/event0 3 54 1865
sendevent /dev/input/event0 3 53 646
sendevent /dev/input/event0 3 58 105
sendevent /dev/input/event0 0 0 0
sendevent /dev/input/event0 3 57 4294967295
sendevent /dev/input/event0 0 0 0
sleep 3
#setting
sendevent /dev/input/event0 3 57 39
sendevent /dev/input/event0 3 54 1279
sendevent /dev/input/event0 3 53 597
sendevent /dev/input/event0 3 58 101
sendevent /dev/input/event0 0 0 0
sendevent /dev/input/event0 3 57 4294967295
sendevent /dev/input/event0 0 0 0
sleep 3
#home
sendevent /dev/input/event0 3 57 49
sendevent /dev/input/event0 3 54 1865
sendevent /dev/input/event0 3 53 646
sendevent /dev/input/event0 3 58 105
sendevent /dev/input/event0 0 0 0
sendevent /dev/input/event0 3 57 4294967295
sendevent /dev/input/event0 0 0 0
sleep 3
#导航
sendevent /dev/input/event0 3 57 59
sendevent /dev/input/event0 3 54 776
sendevent /dev/input/event0 3 53 155
sendevent /dev/input/event0 3 58 105
sendevent /dev/input/event0 0 0 0
sendevent /dev/input/event0 3 57 4294967295
sendevent /dev/input/event0 0 0 0
sleep 3
#home
sendevent /dev/input/event0 3 57 49
sendevent /dev/input/event0 3 54 1865
sendevent /dev/input/event0 3 53 646
sendevent /dev/input/event0 3 58 105
sendevent /dev/input/event0 0 0 0
sendevent /dev/input/event0 3 57 4294967295
sendevent /dev/input/event0 0 0 0
sleep 3
#setting
sendevent /dev/input/event0 3 57 39
sendevent /dev/input/event0 3 54 1279
sendevent /dev/input/event0 3 53 597
sendevent /dev/input/event0 3 58 101
sendevent /dev/input/event0 0 0 0
sendevent /dev/input/event0 3 57 4294967295
sendevent /dev/input/event0 0 0 0
sleep 3
#home
sendevent /dev/input/event0 3 57 49
sendevent /dev/input/event0 3 54 1865
sendevent /dev/input/event0 3 53 646
sendevent /dev/input/event0 3 58 105
sendevent /dev/input/event0 0 0 0
sendevent /dev/input/event0 3 57 4294967295
sendevent /dev/input/event0 0 0 0
sleep 3
#music
sendevent /dev/input/event0 3 57 49
sendevent /dev/input/event0 3 54 763
sendevent /dev/input/event0 3 53 450
sendevent /dev/input/event0 3 58 105
sendevent /dev/input/event0 0 0 0
sendevent /dev/input/event0 3 57 4294967295
sendevent /dev/input/event0 0 0 0
sleep 3

num=$(($num+1))
echo $num 
done