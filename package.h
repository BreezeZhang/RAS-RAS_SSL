#ifndef	__PACKAGE_H__
#define	__PACKAGE_H__

#include "config.h"

typedef union {
	char user[BUFLEN];
	char filename[BUFLEN];
	char cmds[BUFLEN];
}UFC;
struct package
{
	unsigned char cmd;
    unsigned char state;
  	UFC	ufc;
	int count;
	char buf[MAXBUF];  
};

struct package pack(unsigned char cmd, unsigned char state, UFC *pufc,  int count, char *buf);

#endif
