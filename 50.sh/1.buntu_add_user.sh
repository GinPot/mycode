#!/bin/bash
# Description:
#   This script will create a normal user with given name and
#   create a samba share for this user.
# Author:
#   Lai Bin
# Updated:
#   2019/01/17

USERNAME=$1
PASSWORD=123456
HOME_DIR=$2
SAMBA_CONF=/etc/samba/smb.conf

if [ -z "$USERNAME" ]; then
    echo "Please specify the user name!"
    echo "Usage: $0 <user_name>"
    exit 1
fi

if [ -z "$HOME_DIR" ]; then
    HOME_DIR=/home
    echo "Using default home directory $HOME_DIR!"
fi

echo "Adding user: $USERNAME with default password: $PASSWORD"
sudo useradd -m -d $HOME_DIR/$USERNAME -p $(openssl passwd -1 $PASSWORD) -s /bin/bash $USERNAME >/dev/null

echo "Adding samba user: $USERNAME with default password: $PASSWORD"
echo -ne "$PASSWORD\n$PASSWORD\n" | sudo smbpasswd -a -s $USERNAME >/dev/null

if grep -q "\[$USERNAME\]" "$SAMBA_CONF"; then

    echo "This user has setup in samba"
else

    echo "Configuring the samba for user $USERNAME"
    echo "
[$USERNAME]
path = $HOME_DIR/$USERNAME
create mode = 0644
directory mode = 0775
writeable = yes
browseable = yes
display charset = UTF-8
unix charset = UTF-8
dos charset = UTF-8
guest ok = no
valid users = $USERNAME
write list = $USERNAME" | sudo tee -a $SAMBA_CONF >/dev/null
    sudo service smbd restart
    sudo service nmbd restart
fi

#echo "Add extra disk volume..."
#su $USERNAME -l -c "mkdir -p /data1/$USERNAME && ln -s /data1/$USERNAME /home/$USERNAME/data"

echo "Done!"