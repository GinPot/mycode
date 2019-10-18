/*
 * $QNXtpLicenseC:
 * Copyright 2007, QNX Software Systems. All Rights Reserved.
 * 
 * You must obtain a written license from and pay applicable license fees to QNX 
 * Software Systems before you may reproduce, modify or distribute this software, 
 * or any work that includes all or part of this software.   Free development 
 * licenses are available for evaluation and non-commercial purposes.  For more 
 * information visit http://licensing.qnx.com or email licensing@qnx.com.
 *  
 * This file may contain contributions from others.  Please review this entire 
 * file for other proprietary rights or license notices, as well as the QNX 
 * Development Suite License Guide at http://licensing.qnx.com/license-guide/ 
 * for other information.
 * $
 */



/*
 * Copyright (c) 1989, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Michael Fischbein.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/cdefs.h>
#include <sys/types.h>
#include <sys/file.h>

#include <err.h>
#include <locale.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>
#include <utmp.h>

static void usage (void);
static void output (struct utmp *);
static FILE *file (const char *);

int
main(argc, argv)
	int argc;
	char **argv;
{
	char *p;
	struct utmp usr;
	struct passwd *pw;
	FILE *ufp;
	char *t;

	(void) setlocale(LC_TIME, "");

	switch (argc) {
	case 1:					/* who */
		ufp = file(_PATH_UTMP);
		/* only entries with both name and line fields */
		while (fread((char *)&usr, sizeof(usr), 1, ufp) == 1)
			if (*usr.ut_name != NULL && *usr.ut_line != NULL)
				output(&usr);
		break;
	case 2:					/* who utmp_file */
		ufp = file(argv[1]);
		/* all entries */
		while (fread((char *)&usr, sizeof(usr), 1, ufp) == 1)
			output(&usr);
		break;
	case 3:					/* who am i */
	        if (strcmp(argv[1], "am")
		    || (strcmp(argv[2], "I") && strcmp(argv[2], "i")))
		        usage();
		
		ufp = file(_PATH_UTMP);

		/* search through the utmp and find an entry for this tty */
		if ((p = ttyname(0))) {
			/* strip any directory component */
			if ((t = rindex(p, '/')))
				p = t + 1;
			while (fread((char *)&usr, sizeof(usr), 1, ufp) == 1)
				if (*usr.ut_name && !strcmp(usr.ut_line, p)) {
					output(&usr);
					exit(0);
				}
			/* well, at least we know what the tty is */
			(void)strncpy(usr.ut_line, p, UT_LINESIZE);
		} else
			(void)strcpy(usr.ut_line, "tty??");
		pw = getpwuid(getuid());
		(void)strncpy(usr.ut_name, pw ? pw->pw_name : "?", UT_NAMESIZE);
		(void)time(&usr.ut_time);
		output(&usr);
		break;
	default:
		usage();
	}
	exit(0);
}

static void
usage()
{
	(void)fprintf(stderr, "%s\n%s\n",
		"usage: who [file]",
		"       who am i");
	exit(EXIT_FAILURE);
}

void
output(up)
	struct utmp *up;
{
	char buf[80];
	static int d_first = -1;

	(void)printf("%-*.*s %-*.*s", UT_NAMESIZE, UT_NAMESIZE, up->ut_name,
	    UT_LINESIZE, UT_LINESIZE, up->ut_line);
	(void)strftime(buf, sizeof(buf),
		       d_first ? "%e %b %R" : "%b %e %R",
		       localtime(&up->ut_time));
	(void)printf("%s\n", buf);
}

static FILE *
file(name)
	const char *name;
{
	FILE *ufp;

	if (!(ufp = fopen(name, "r")))
		err(1, "%s", name);
	return(ufp);
}


