#! /bin/sh

let "filenum = 0 "
let "dirnum = 0 "
ls $1
echo ""
for dir in $( ls )
do
if [ -d $dir ]
then
let "dirnum+=1"
else
let "filenum+=1"
fi
done
echo "The number of files is $filenum "