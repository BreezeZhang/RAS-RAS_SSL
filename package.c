#include "package.h"
#include "sys_param.h"
#include <string.h>

struct package pack(unsigned char cmd, unsigned char state, UFC *pufc,  int count, char *buf)
{
	struct package tpackage;
	memset(&tpackage, 0, sizeof(struct package));
	tpackage.cmd = cmd;
	tpackage.state = state;
	tpackage.count = count;
	if(buf != NULL)
	{
		memcpy(tpackage.buf, buf, count);
	}
	if(pufc == NULL)
		return tpackage;
	switch(tpackage.cmd)
	{
	case CMD_LOGIN:	
		if(pufc->user)
			strcpy(tpackage.ufc.user, pufc->user);
		break;
	case CMD_DOWN:
		strcpy(tpackage.ufc.filename, pufc->filename);
		break;
	case CMD_UP:
		strcpy(tpackage.ufc.filename, pufc->filename);
		break;
	case CMD_LIST:
		strcpy(tpackage.ufc.filename, pufc->filename);
		break;
	case CMD_QUIT:
		break;
	case CMD_CMD:
		strcpy(tpackage.ufc.cmds, pufc->cmds);
		break;
	default:
		break;
	}
	return tpackage;
}

