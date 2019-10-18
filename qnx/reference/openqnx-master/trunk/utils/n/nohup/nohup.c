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


#ifndef lint
static const char copyright[] = "@(#) Copyright (c) 1989, 1993\nThe Regents of the University of California. All rights reserved.\n";
#endif /* not lint */

#ifndef lint
#if 0
static char sccsid[] = "@(#)nohup.c	8.1 (Berkeley) 6/6/93";
#endif
static const char rcsid[] =
  "$FreeBSD: src/usr.bin/nohup/nohup.c,v 1.6 2001/07/19 21:25:10 mike Exp $";
#endif /* not lint */

#include <sys/param.h>
#include <sys/stat.h>

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void dofile __P((void));
static void usage __P((void));

#define	FILENAME	"nohup.out"
/*
 * POSIX mandates that we exit with:
 * 126 - If the utility was found, but failed to execute.
 * 127 - If any other error occurred. 
 */
#define	EXIT_NOEXEC	126
#define	EXIT_NOTFOUND	127
#define	EXIT_MISC	127

int
main(argc, argv)
	int argc;
	char *argv[];
{
	int exit_status;

	while (getopt(argc, argv, "") != -1)
		usage();
	argc -= optind;
	argv += optind;
	if (argc < 1)
		usage();

	if (isatty(STDOUT_FILENO))
		dofile();
	if (isatty(STDERR_FILENO) && dup2(STDOUT_FILENO, STDERR_FILENO) == -1)
		/* may have just closed stderr */
		err(EXIT_MISC, "%s", argv[0]);

	(void)signal(SIGHUP, SIG_IGN);

	execvp(*argv, argv);
	exit_status = (errno == ENOENT) ? EXIT_NOTFOUND : EXIT_NOEXEC;
	err(exit_status, "%s", argv[0]);
}

static void
dofile()
{
	int fd;
	char path[MAXPATHLEN];
	const char *p;

	/*
	 * POSIX mandates if the standard output is a terminal, the standard
	 * output is appended to nohup.out in the working directory.  Failing
	 * that, it will be appended to nohup.out in the directory obtained
	 * from the HOME environment variable.  If file creation is required,
	 * the mode_t is set to S_IRUSR | S_IWUSR.
	 */
	p = FILENAME;
	fd = open(p, O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
	if (fd != -1)
		goto dupit;
	if ((p = getenv("HOME")) != NULL && *p != '\0' &&
	    (size_t)snprintf(path, sizeof(path), "%s/%s", p, FILENAME) <
	    sizeof(path)) {
		fd = open(p = path, O_RDWR | O_CREAT | O_APPEND,
		    S_IRUSR | S_IWUSR);
		if (fd != -1)
			goto dupit;
	}
	errx(EXIT_MISC, "can't open a nohup.out file");

dupit:
	if (dup2(fd, STDOUT_FILENO) == -1)
		err(EXIT_MISC, "dup2");
	(void)fprintf(stderr, "appending output to %s\n", p);
}

static void
usage()
{
	(void)fprintf(stderr, "usage: nohup [--] command [arguments]\n");
	exit(EXIT_MISC);
}


