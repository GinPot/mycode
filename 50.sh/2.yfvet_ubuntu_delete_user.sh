#!/bin/bash
# Description:
#   This script will delete the user with given name and
#   delete his samba share point as well.
# Author:
#   Lai Bin (bin.lai@yfve.com.cn)
# Updated:
#   2019/01/17

USERNAME=$1
SAMBA_CONF=/etc/samba/smb.conf

if [ -z "$USERNAME" ]; then
    echo "Please specify the user name!"
    echo "Usage: $0 <user_name>"
    exit 1
fi

echo "Deleting user: $USERNAME, but his home data will remains."
sudo userdel -r $USERNAME >/dev/null

echo "Deleting samba user: $USERNAME"
sudo smbpasswd -x $USERNAME >