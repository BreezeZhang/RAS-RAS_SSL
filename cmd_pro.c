#include "cmd_pro.h"
#include <string.h>
#define	BUFLEN	256

static int rm_file(char *cmds, char *pwd)
{
	char dirname[BUFLEN];
	char filepath[BUFLEN];
	while(*(cmds +3) == ' ')
		cmds++;
	strcpy(dirname, cmds+3);
	strcpy(filepath, pwd);
	rmdir(filepath);
	
	return 1;
}
static int cd_file(char *cmds, char *pwd)
{
	char dirname[BUFLEN];
	char filepath[BUFLEN];
	while(*(cmds+3) == ' ')
		cmds++;
	strcpy(dirname, cmds+3);
	strcpy(filepath, pwd);
	strcat(filepath, "/");
	strcat(filepath, dirname);
	strcpy(pwd, filepath);
	return 1;
}
static int mv_file(char *cmds, char *pwd)
{
	return 1;
}
int process_cmd(char *cmds, char *pwd)
{
		if(strncmp(cmds, "rm ", 3) == 0)
		{
			return rm_file(cmds, pwd);
		}
		if(strncmp(cmds, "cd ", 3) == 0)
		{
			return rm_file(cmds, pwd);
		}
		if(strncmp(cmds, "mv ", 3) == 0)
		{
			return rm_file(cmds, pwd);
		}
		return 0;
}
