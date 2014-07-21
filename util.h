#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#define  err_msg(MSG)	{	\
		perror(MSG);	\
		DPRINT("");	\
		exit(errno);	\
}

#define DEBUG_ENABLE 
#ifdef DEBUG_ENABLE
#define DPRINT(info)	do{	\
	printf("Info : File = %s, Line = %d, Info = %s\n",\
			__FILE__, __LINE__, info);	\
}while(0)

#define	DPRINTD(INFO, num)	do{	\
	printf(INFO"%d\n", num);	\
}while(0)
#define DPRINTS(INFO, info)	do{	\
	printf(INFO"%s\n", info);	\
}while(0)
#else
#define	DPRINT(info)
#endif

#define	STATE_RUN	1
#define	STATE_STOP	0
#define	STATE_EXIT	-1

struct	shared_info{
	int	state;
};



#endif