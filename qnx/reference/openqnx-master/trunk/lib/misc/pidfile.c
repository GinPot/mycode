/*	$NetBSD: pidfile.c,v 1.7 2002/05/22 07:31:45 itojun Exp $	*/

/*-
 * Copyright (c) 1999 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Jason R. Thorpe and Matthias Scheler.
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
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * $QNXLicenseC:
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



#include <sys/cdefs.h>
#if defined(LIBC_SCCS) && !defined(lint)
__RCSID("$NetBSD: pidfile.c,v 1.7 2002/05/22 07:31:45 itojun Exp $");
#endif

#include <sys/param.h>
#include <paths.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <util.h>

static int   pidfile_atexit_done;
static pid_t pidfile_pid;
static char *pidfile_basename;
static char *pidfile_path;

static void pidfile_cleanup(void);

int
pidfile(const char *basename)
{
	FILE *f;

	/*
	 * Register handler which will remove the pidfile later.
	 */
	if (!pidfile_atexit_done) {
		if (atexit(pidfile_cleanup) < 0)
			return -1;
		pidfile_atexit_done = 1;
	}

	if (basename == NULL)
		basename = getprogname();

	/*
	 * If pidfile has already been created for the supplied basename
	 * we don't need to create a pidfile again.
	 */
	if (pidfile_path != NULL) {
		if (strcmp(pidfile_basename, basename) == 0)
			return 0;
		/*
		 * Remove existing pidfile if it was created by this process.
		 */
		pidfile_cleanup();

		free(pidfile_path);
		pidfile_path = NULL;
		free(pidfile_basename);
		pidfile_basename = NULL;
	}

	pidfile_pid = getpid();

	pidfile_basename = strdup(basename);
	if (pidfile_basename == NULL)
		return -1;

	/* _PATH_VARRUN includes trailing / */
	(void) asprintf(&pidfile_path, "%s%s.pid", _PATH_VARRUN, basename);
	if (pidfile_path == NULL) {
		free(pidfile_basename);
		pidfile_basename = NULL;
		return -1;
	}

	if ((f = fopen(pidfile_path, "w")) == NULL) {
		free(pidfile_path);
		pidfile_path = NULL;
		free(pidfile_basename);
		pidfile_basename = NULL;
		return -1;
	}

	(void) fprintf(f, "%d\n", pidfile_pid);
	(void) fclose(f);
	return 0;
}

static void
pidfile_cleanup(void)
{
	/* Only remove the pidfile if it was created by this process. */
	if ((pidfile_path != NULL) && (pidfile_pid == getpid()))
		(void) unlink(pidfile_path);
}
