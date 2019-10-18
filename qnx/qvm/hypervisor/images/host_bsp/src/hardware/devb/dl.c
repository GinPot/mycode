/*
*
* Copyright (c) 2013, QNX Software Systems.
*
* Licensed under the Apache License, Version 2.0 (the "License"). You
* may not reproduce, modify or distribute this software except in
* compliance with the License. You may obtain a copy of the License
* at: http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" basis,
* WITHOUT WARRANTIES OF ANY KIND, either express or implied.
*
*/



#undef dlopen
#undef dlclose
#undef dlerror
#undef dlsym

#include <dlfcn.h>
#include <errno.h>
#include <malloc.h>
#include <string.h>
#include "dl.h"

static int
is_bound(const struct dll_list *test) {
	const struct dll_list	*l;

	for (l = dll_list; l->fname != NULL; ++l) {
		if (l == test) return(1);
	}
	return(0);
}

void *
bound_dlopen(const char *pathname, int mode) {
	const struct dll_list	*l;

	for (l = dll_list; l->fname != NULL; ++l) {
		if (pathname != NULL && !strcmp(l->fname, pathname)) return((void *)l);
	}
	return(dlopen(pathname, mode));
}

void *
bound_dlsym(void *handle, const char *name) {
	const struct dll_list	*l;
	const struct dll_syms	*s;

	if (handle == RTLD_DEFAULT) {
		for (l = dll_list; l->fname != NULL; ++l) {
			for (s = l->syms; s->symname != NULL; ++s) {
				if (!strcmp(name, s->symname)) return(s->addr);
			}
		}
	}

	if (!is_bound(handle)) return(dlsym(handle, name));

	for (s = ((struct dll_list *)handle)->syms; s->symname != NULL; ++s) {
		if (!strcmp(name, s->symname)) return(s->addr);
	}
	return((void *)NULL);
}

int
bound_dlclose(void *handle) {
	if (!is_bound(handle)) return(dlclose(handle));

	return(0);
}

char *
bound_dlerror(void) {
	return(dlerror());
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/devb/dl.c $ $Rev: 823127 $")
#endif
