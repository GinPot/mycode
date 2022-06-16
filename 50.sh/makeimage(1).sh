#!/bin/bash
rm -f shared.img.sparse 
mkfs.vfat -n "shared" -F 32 -S 4096 -C shared.tmp 8388608 2>/dev/null
rm -rf hiphishowdata
mkdir hiphishowdata
sudo mount -o uid=1000,gid=1000 shared.tmp hiphishowdata
cp -r HiPhiShow/ hiphishowdata/
sudo umount hiphishowdata
rm -rf hiphishowdata
img2simg shared.tmp shared.img.sparse
sudo chown 1000:1000 shared.img.sparse
rm -f shared.tmp
