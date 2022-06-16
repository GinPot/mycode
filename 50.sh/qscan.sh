#!/bin/ksh

#THIS IS EXPERIMENTAL SOFTWARE

# Copyright (c) 2017 QNX Software Systems. All Rights Reserved.
#
# You must obtain a written license from and pay applicable license fees to QNX
# Software Systems before you may reproduce, modify or distribute this software,
# or any work that includes all or part of this software.   Free development
# licenses are available for evaluation and non-commercial purposes.  For more
# information visit [http://licensing.qnx.com] or email licensing@qnx.com.
#
# This file may contain contributions from others.  Please review this entire
# file for other proprietary rights or license notices, as well as the QNX
# Development Suite License Guide at [http://licensing.qnx.com/license-guide/]
# for other information.


# qscan
# Scan target filesystem, collect file information, and save in CSV format.
# Information collected: permissions, checksums (sha512), path, and size for all files, plus 'use -i' information for elf binaries.

#set -x

if [ "${KSH_VERSION}" = "" -a "${BASH_VERSION}" = "" ]; then
	echo "You have to run this script with ksh or bash"
	exit 1
fi

SCR_NAME=${0##*/}
SCR_REV="1.1.1"

if [ $# -lt 1 ]; then
	echo "${SCR_NAME} - Scan QNX Neutrino System (${SCR_REV})"
	echo "usage: ${SCR_NAME} <path> [<path> ... ]"
	echo "example:"
	echo "  ${SCR_NAME} /usr/sbin /usr/bin"
	echo "  FIND=/proc/boot/find USE=/proc/boot/use ${SCR_NAME} /usr/bin"
	exit 1
fi
echo "#${SCR_NAME} - build ${SCR_REV}"

# User can override FIND and USE with the paths to both
# on the command line if they are in strange spots
FIND=${FIND:-find}
USE=${USE:-use}
OPENSSL=${OPENSSL:-openssl}

type ${FIND} >&2 || exit 2
type ${USE} >&2 || exit 2
type ${OPENSSL} >&2 || echo "Warning: openssl is required for checksums" 1>&2

###########################

PATH_LIST=${*-/}

# We need to prune out some base dirs that we know shouldn't be touched
if [ "$PATH_LIST" = "/" ]; then
	PATH_LIST=`${FIND} ${PATH_LIST} -path '/dev' -o -path '/proc' -o -path '/.boot' -o -path '/.diskroot/' -prune -o -type d -maxdepth 1 -mindepth 1 -print`
	#Ugly kludge to ensure we look into /proc/boot for binaries but not /proc itself.
	PATH_LIST="/proc/boot ${PATH_LIST}"
fi
ORIG_IFS=${IFS}
echo "Name,State,Ver,TagID,Date,Fullpath,BuildID,Package,Mode,Owner,Size,DigestSHA512Hex"


# Drive our loop with a find instead of saving all the files in a variable
echo ${FIND} ${PATH_LIST} >&2
# -perm -111
${FIND} ${PATH_LIST} -type f -printf "%f:%p:%u:%g:%m:%s\n" | {
	 IFS=':'
   while read BASENAME FULLPATH OUSER OGROUP MODE SIZE; do
     echo Reading ${FULLPATH} >&2
		 export MODE
		 export OUSER
		 export OGROUP
		 export BASENAME
		 export FULLPATH
		 export SIZE
		 (${OPENSSL} dgst -sha512 ${FULLPATH}; ${USE} -ie ${FULLPATH} 2>&1) | (
	   IFS='='
				UNAME=""
				UDESC=""
				UDATE=""
				USHORTDATE=""
				USTATE=""
				UPACKAGE=""
				UVER=""
				UTAGID=""
				UBUILDID=""
				while read KEY VAL; do
					case ${KEY} in
						NAME)
							UNAME=${VAL}
							;;
						QNX_BUILDID)
							UBUILDID=${VAL}
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
						DESCRIPTION)
							UDESC=${VAL}
							;;
						PACKAGE)
							UPACKAGE=${VAL}
							;;
						TAGID)
							UTAGID=${VAL}
							;;
						"No info"*)
							# In case we trip over a binary that isn't ours we shouldn't
							# bork the output with the error from use -i
							USTATE="N/A"
							UVER="N/A"
							UTAGID="N/A"
							UBUILDID="NOBUILDID"
							;;
						SHA512*)
							DIGESTSHA512=${VAL}
							;;
						"") break;;
					esac
				 done #done reading useinfo
			  UNAME=${UNAME:-${BASENAME}}
        case "$FULLPATH" in
          *,*)
				echo "\"${UNAME}\",${USTATE},${UVER},${UTAGID},${USHORTDATE},\"${FULLPATH}\",${UBUILDID},${UPACKAGE},${MODE},${OUSER}:${OGROUP},${SIZE},${DIGESTSHA512}"
            ;;
          *)
				echo "${UNAME},${USTATE},${UVER},${UTAGID},${USHORTDATE},${FULLPATH},${UBUILDID},${UPACKAGE},${MODE},${OUSER}:${OGROUP},${SIZE},${DIGESTSHA512}"
            ;;
        esac
				 )
			 done
}

