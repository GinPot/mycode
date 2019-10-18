/*	$NetBSD: getttyent.c,v 1.13 1998/02/03 18:23:46 perry Exp $	*/

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
/*
 * $QNXLicenseC:
 * Copyright 2007, QNX Software Systems. All Rights Reserved.
 *
 * You must obtain a written license from and pay applicable 
 * license fees to QNX Software Systems before you may reproduce, 
 * modify or distribute this software, or any work that includes 
 * all or part of this software.   Free development licenses are 
 * available for evaluation and non-commercial purposes.  For more 
 * information visit http://licensing.qnx.com or email 
 * licensing@qnx.com.
 * 
 * This file may contain contributions from others.  Please review 
 * this entire file for other proprietary rights or license notices, 
 * as well as the QNX Development Suite License Guide at 
 * http://licensing.qnx.com/license-guide/ for other information.
 * $
 */

#include <sys/cdefs.h>
#if defined(LIBC_SCCS) && !defined(lint)
#if 0
static char sccsid[] = "@(#)getttyent.c	8.1 (Berkeley) 6/4/93";
#else
__RCSID("$NetBSD: getttyent.c,v 1.13 1998/02/03 18:23:46 perry Exp $");
#endif
#endif /* LIBC_SCCS and not lint */

#ifndef __QNXNTO__
#include "namespace.h"
#endif
#include <ttyent.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#ifdef __weak_alias
__weak_alias(endttyent,_endttyent);
__weak_alias(getttyent,_getttyent);
__weak_alias(getttynam,_getttynam);
__weak_alias(setttyent,_setttyent);
#endif

extern char *nto_conf_path( char *, char *, int );

static char zapchar;
static FILE *tf;
static char *skip __P((char *));
static char *value __P((char *));

struct ttyent *
getttynam(tty)
	const char *tty;
{
	struct ttyent *t;

	setttyent();
	while ((t = getttyent()) != NULL)
		if (!strcmp(tty, t->ty_name))
			break;
	endttyent();
	return (t);
}

struct ttyent *
getttyent()
{
	static struct ttyent tty;
	int c;
	char *p;
#define	MAXLINELENGTH	200
	static char line[MAXLINELENGTH];

	if (!tf && !setttyent())
		return (NULL);
	for (;;) {
		if (!fgets(p = line, sizeof(line), tf))
			return (NULL);
		/* skip lines that are too big */
		if (!strchr(p, '\n')) {
			while ((c = getc(tf)) != '\n' && c != EOF)
				;
			continue;
		}
		while (isspace(*p))
			++p;
		if (*p && *p != '#')
			break;
	}

	zapchar = 0;
	tty.ty_name = p;
	p = skip(p);
	if (!*(tty.ty_getty = p))
		tty.ty_getty = tty.ty_type = NULL;
	else {
		p = skip(p);
		if (!*(tty.ty_type = p))
			tty.ty_type = NULL;
		else
			p = skip(p);
	}
	tty.ty_status = 0;
	tty.ty_window = NULL;

#define	scmp(e)	!strncmp(p, e, sizeof(e) - 1) && isspace(p[sizeof(e) - 1])
#define	vcmp(e)	!strncmp(p, e, sizeof(e) - 1) && p[sizeof(e) - 1] == '='
	for (; *p; p = skip(p)) {
		if (scmp(_TTYS_OFF))
			tty.ty_status &= ~TTY_ON;
		else if (scmp(_TTYS_ON))
			tty.ty_status |= TTY_ON;
		else if (scmp(_TTYS_SECURE))
			tty.ty_status |= TTY_SECURE;
		else if (scmp(_TTYS_LOCAL))
			tty.ty_status |= TTY_LOCAL;
		else if (scmp(_TTYS_RTSCTS))
			tty.ty_status |= TTY_RTSCTS;
		else if (scmp(_TTYS_DTRCTS))
			tty.ty_status |= TTY_DTRCTS;
		else if (scmp(_TTYS_SOFTCAR))
			tty.ty_status |= TTY_SOFTCAR;
		else if (scmp(_TTYS_MDMBUF))
			tty.ty_status |= TTY_MDMBUF;
		else if (scmp(_TTYS_NOARG))
			tty.ty_status |= TTY_NOARG;
		else if (vcmp(_TTYS_WINDOW))
			tty.ty_window = value(p);
		else
			break;
	}

	if (zapchar == '#' || *p == '#')
		while ((c = *++p) == ' ' || c == '\t')
			;
	tty.ty_comment = p;
	if (*p == 0)
		tty.ty_comment = 0;
	if ((p = strchr(p, '\n')) != NULL)
		*p = '\0';
	return (&tty);
}

#define	QUOTED	1

/*
 * Skip over the current field, removing quotes, and return a pointer to
 * the next field.
 */
static char *
skip(p)
	char *p;
{
	char *t;
	int c, q;

	for (q = 0, t = p; (c = *p) != '\0'; p++) {
		if (c == '"') {
			q ^= QUOTED;	/* obscure, but nice */
			continue;
		}
		if (q == QUOTED && *p == '\\' && *(p+1) == '"')
			p++;
		*t++ = *p;
		if (q == QUOTED)
			continue;
		if (c == '#') {
			zapchar = c;
			*p = 0;
			break;
		}
		if (c == '\t' || c == ' ' || c == '\n') {
			zapchar = c;
			*p++ = 0;
			while ((c = *p) == '\t' || c == ' ' || c == '\n')
				p++;
			break;
		}
	}
	*--t = '\0';
	return (p);
}

static char *
value(p)
	char *p;
{

	return ((p = strchr(p, '=')) ? ++p : NULL);
}

int
setttyent()
{
	if (tf) {
		rewind(tf);
		return (1);
	} else {
		if ((tf = fopen( nto_conf_path( _PATH_TTYS, 0, 0), "r")) != NULL)
			return (1);
	}
	return (0);
}

int
endttyent()
{
	int rval;

	if (tf) {
		rval = !(fclose(tf) == EOF);
		tf = NULL;
		return (rval);
	}
	return (1);
}
