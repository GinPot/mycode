#ifndef _SENDMONIT_H_
#define _SENDMONIT_H_

#include "performancedisplay.h"

struct Todataapp tomonit;
int getmonitflag;

extern void* pthreadgetmonit(void* arg);


#endif