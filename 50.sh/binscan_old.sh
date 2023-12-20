#!/proc/boot/ksh

#set -x

# BINSCAN - amallory@qnx.com 2012
# Scan system for binaries and extract their 'use -i' information
# into a CSV format which can be imported and reported on.

if [ "${KSH_VERSION}" == "" ]; then
	echo "You need to be running KSH as /bin/sh or run this with ksh"
	exit 1
fi

SCR_NAME=${0##*/}
SCR_REV="0.0.4"

if [ $# -lt 1 ]; then
	echo "${SCR_NAME} - Scan QNX Neutrino System (${SCR_REV})\n"
	echo "\n ${SCR_NAME} [path] [path ...]"
	echo "\n eg:"
	echo "  ${SCR_NAME} /usr/sbin /usr/bin"
	echo "  FIND=/proc/boot/find USER=/proc/boot/use ${SCR_NAME} /usr/bin" 
	exit 1
fi
echo "${SCR_NAME} - amallory@qnx.com build ${SCR_REV}"

# Try to be swift and locate some gear auto-magically
# User can override FIND and USE with the paths to both
# on the command line if they are in strange spots
WHICH=`which which 2> /dev/null`
if [ "${WHICH}" == "" ]; then
	if [ -e /usr/bin/find ]; then
		FIND=${FIND="/usr/bin/find"}
	elif [ -e /proc/boot/find ]; then
		FIND=${FIND="/proc/boot/find"}
	fi
	if [ -e /usr/bin/use ]; then
		USE=${USE="/usr/bin/use"}
	elif [ -e /proc/boot/use ]; then
		USE=${USE="/proc/boot/use"}
	fi
else
FIND=${FIND=`${WHICH} find 2> /dev/null`}
USE=${USE=`${WHICH} use 2> /dev/null`}
fi

if [ "${FIND}" == "" -o "${USE}" == "" ]; then
	echo "Error - find/use not set! FIND=${FIND} USE=${USE}"
	exit 1
fi

# Check (we may have had overrides from the user environment) for sanity
if [ ! -e ${FIND} ]; then
	echo "Cannot find ${FIND}!"
	exit 1
fi

if [ ! -e ${USE} ]; then
	echo "Cannot find ${USE}!"
	exit 1
fi

###########################

PATH_LIST=${*-/}

# We need to prune out some base dirs that we know shouldn't be touched
if [ "$PATH_LIST" == "/" ]; then
	PATH_LIST=`${FIND} ${PATH_LIST} -path '/dev' -o -path '/proc' -o -path '/.boot' -o -path '/.diskroot/' -prune -o -type d -maxdepth 1 -print`
	#Ugly kludge to ensure we look into /proc/boot for binaries but not /proc itself.
	PATH_LIST="/proc/boot ${PATH_LIST}"
fi	
ORIG_IFS=${IFS}
echo "Name, State, Ver, TagID, Date, Host, User, Fullpath"

COUNT=0
LAST_KEY=""

# Drive our loop with a find instead of saving all the files in a variable
${FIND} ${PATH_LIST} -follow -type f \( -perm -100 -o -perm -010 -o -perm -001 \) -exec echo FULLPATH={} \; -exec ${USE} -ie '{}' \; 2>&1 | {
	IFS='='
	while read KEY VAL; do
		
		# sanity check
		# if COUNT is greater than 30 (triple the number of data's we are looking for), then
		# assume something has gone horribly wrong and exit
		if [ ${COUNT} -gt 30 ]; then
			echo "ERROR: could not find appropriate data, looped too many times! ${COUNT}"
			exit
		fi
		
		# if you find a blank key, AND you have data (just look at UNAME)
		# OR if you find the RESET_STATE
		# then print out all the data and clear variables
		
		if [ "${KEY}" == "" ]; then
			if [ "${UNAME}" != "" ]; then
				echo "${UNAME}, ${USTATE}, ${UVER}, ${UTAGID}, ${USHORTDATE}, ${UHOST}, ${UUSER}, ${UFULLPATH}"
				UNAME=""
				UDESC=""
				UDATE=""
				USHORTDATE=""
				USTATE=""
				UHOST=""
				UUSER=""
				UVER=""
				UTAGID=""
				RESET_STATE=""
				COUNT=0
			fi
			continue
		fi
		
		if [ "${RESET_STATE}" == "end" ]; then
			echo "*** ${UNAME}, ${USTATE}, ${UVER}, ${UTAGID}, ${USHORTDATE}, ${UHOST}, ${UUSER}, ${UFULLPATH}"
			UNAME=""
			UDESC=""
			UDATE=""
			USHORTDATE=""
			USTATE=""
			UHOST=""
			UUSER=""
			UVER=""
			UTAGID=""
			RESET_STATE=""
			COUNT=0
		fi
		
		case ${KEY} in
		
			FULLPATH)
				# if you find FULLPATH again, reset the count
				if [ "${LAST_KEY}" == "FULLPATH" ]; then
					COUNT=0
				fi
				UFULLPATH=${VAL}
				#RESET_STATE=""
				;;
			NAME)
				UNAME=${VAL}
				;;
			DESCRIPTION)
				UDESC=${VAL}
				;;
			DATE)
				UDATE=${VAL}
				USHORTDATE=${UDATE%%-*}
				;;
			STATE)
				USTATE=${VAL}
				;;
			HOST)
				UHOST=${VAL}
				;;
			USER)
				UUSER=${VAL}
				if [ "${USTATE}" == "Experimental" ]; then
					UVER="N/A"
					UTAGID="N/A"
					#RESET_STATE="end"
				fi
				;;
			VERSION)
				UVER=${VAL}
				;;
			TAGID)
				UTAGID=${VAL}
				#if [ "${USTATE}" == "stable" ]; then
					#RESET_STATE="end"
				#fi
				;;
			"No info"*)
				# In case we trip over a binary that isn't ours we shouldn't
				# bork the output with the error from use -i
				UNAME="N/A"
				UDESC="N/A"
				UDATE="N/A"
				USHORTDATE="N/A"
				USTATE="N/A"
				UHOST="N/A"
				UUSER="N/A"
				UVER="N/A"
				UTAGID="N/A"
				RESET_STATE="end"
				;;
		esac
		#if [ "${RESET_STATE}" != "" ]; then
		#	echo "${UNAME}, ${USTATE}, ${UVER}, ${UTAGID}, ${USHORTDATE}, ${UHOST}, ${UUSER}, ${UFULLPATH}"
			#printf "%s, %s, %s, %s, %s, %s, %s, %s\n" ${UNAME} ${USTATE} ${UVER} ${UTAGID} ${USHORTDATE} ${UHOST} ${UUSER} ${UFULLPATH}
		#	RESET_STATE=""	
		#fi
		((COUNT = COUNT + 1))
		LAST_KEY=${KEY}
	done
	IFS="${ORIG_IFS}"
}

