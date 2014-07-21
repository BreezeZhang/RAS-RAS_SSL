#include "package.h"
#include "comm.h"
#include "config.h"
#include "util.h"
#include "cmd_pro.h"
#include "sys_param.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/statfs.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include "openssl.h"

int check_user(char *user);


void getpwd(char *user, char *pwd)
{
	char username[ULEN] = "";
	int count = 0;

	DPRINTS("user: ", user);
	while((username[count++] = *user++) != '#' && count < ULEN)
		NULL;
	username[count-1] = '\0';
	strcat(pwd, "/userdir/");
	strcat(pwd, username);
	//strcat(pwd, "/");
	DPRINTS("GET PWD IS ", pwd);
}

static inline int send_data(void *ssl, struct package *buf, int len)
{
	int ret;
	if((ret = SSL_write((void *)ssl, buf, len)) > 0)
		return ret;
	ssl_err_print(ssl, ret);
	//pthread_exit(0);
	return 0;
}
static inline int recv_data(void *ssl, struct package *buf, int len)
{
	int ret;
	if((ret = SSL_read((void *)ssl, buf, len)) > 0)
		return ret;
		
	ssl_err_print(ssl, ret);
	return 0;
}

static void process_lgoin(void *fd, struct package tpack, char *pwd)
{
	struct package send_package;
	memset(&send_package, 0, sizeof(struct package));
	if(check_user(tpack.ufc.user) == 1)
	{
		DPRINT("cheack_user end");
		send_package = pack(CMD_LOGIN,STATE_LOGIN_CFM, 0, 0, NULL );
		getpwd(tpack.ufc.user, pwd);
		DPRINTS("Current path is ", pwd);
	}
	else
	{
		DPRINT("PACK LOGIN err");
		send_package = pack(CMD_LOGIN, STATE_LOGIN_ERR, 0, 0, NULL );
	}
	DPRINT("BEGIN ssl write");
	if(send_data(fd, &send_package, sizeof(struct package)) < 0)
	{
		DPRINT("WRITE ERROR");
		perror("ssl_write error");
		pthread_exit(NULL);
	}
}

void process_filelist(void *ssl, struct package tpack, char *pwd)
{
	DIR *pdir;
	struct dirent *pdt;
	char filetype[2] = "";
	char filename[MAXBUF] = "";
	struct package send_package;
	DPRINT("GET LIST FUN");
	if(pwd == NULL)	
		DPRINT("PWD IS nULL");

	if((pdir = opendir(pwd)) == NULL)
	{
		DPRINTS("PWD= :", pwd);

		perror("opendir: ");
		send_package = pack(CMD_LIST, STATE_FILELIST_ERR, NULL, 0, NULL);
		send_data(ssl, &send_package, sizeof(struct package));
		return;
	}
	while(1)
	{
		pdt = readdir(pdir);
		if(pdt == NULL)
			break;
		else
		{

			if(strcmp(pdt->d_name, ".")==0 || strcmp(pdt->d_name, "..")==0)
				continue;

			strcpy(filename, pdt->d_name);
	//		sprintf(filetype, "%c", pdt->d_type);
			filetype[0] = pdt->d_type;
			filetype[1] = '\0';
	
	//		DPRINTS("FILES:", ufc.filename);
			send_package = pack(CMD_LIST, STATE_FILELIST_PRO, (UFC *)filename, 2, filetype);
			send_data(ssl, &send_package, sizeof(struct package));
		}
	}
	closedir(pdir);
	memset(&send_package, 0, sizeof(struct package));
	send_package = pack(CMD_LIST, STATE_FILELIST_END, NULL, 0, NULL);
	send_data(ssl, &send_package, sizeof(struct package));
	DPRINT("transmit filelist end");
}

void process_downfile(void *ssl, struct package tpack, char *pwd)
{
	int fd;
	char filepath[MAXBUF] = "";
	int count = 0;
	int filesize = 0;
	char buf[MAXBUF];
	struct stat statbuf;
	struct package send_package;
	struct package recv_package;
	memset(&send_package, 0, sizeof(struct package));
	memset(&recv_package, 0, sizeof(struct package));
	memset(&statbuf, 0, sizeof(struct stat));

	DPRINT("BEGIN PROCESS DOWN");
	if(tpack.state == 82)
	{
		strcat(filepath, pwd);
		strcat(filepath, "/");
		strcat(filepath, tpack.ufc.filename);
		DPRINTS("FILEPATH", filepath);
		if(stat(filepath, &statbuf) == -1)
		{
			send_package = pack(CMD_DOWN, 8, 0,0, NULL); /* not exit file */
			send_data(ssl, &send_package, sizeof(struct package));
			return;
		}
		send_package = pack(CMD_DOWN, 0, &tpack.ufc, statbuf.st_size, NULL);
		send_data(ssl, &send_package, sizeof(struct package));
	}
	else
	{
		DPRINT("NOT EXIST THIS SITUTION");
		return;
	}
	
	if((fd = open(filepath, O_RDONLY)) == -1)
	{
		perror("open file error");
		send_package = pack(CMD_DOWN, 6, 0,0, NULL); /* not exit file */
		send_data(ssl, &send_package, sizeof(struct package));
		return;
	}
	/******/
	recv_data(ssl, &recv_package, sizeof(struct package));
	if(recv_package.state == 05) /* check the client disk is wheather  enough */
	{
		printf("client has not enough stroge space\n");
		return ;
	}
	else if(recv_package.state == 07)
	{
		printf("client create file error, filename: %s \n", send_package.ufc.filename);
	}
	else if(recv_package.state == 01)
	{
		printf("Strant transmit %s \n", send_package.ufc.filename);
	}

	while((count = read(fd, buf, MAXBUF)) > 0)
	{
		memset(&send_package, 0, sizeof(struct package));
		send_package = pack(CMD_DOWN, 02, &tpack.ufc, count, buf);
		if((send_data(ssl, &send_package, sizeof(struct package))) == -1)
		{
			perror("send login message:");
		}
	}
	close(fd);

	send_package = pack(CMD_DOWN, 04, &tpack.ufc, 0, 0);

	send_data(ssl, &send_package, sizeof(struct package));
	return ;
}

void process_upfile(void *ssl, struct package tpack, char *pwd)
{
	int fd;
	char filepath[MAXBUF] = "";
	int count = 0;
	int filesize = 0;
	char buf[MAXBUF];
	struct statfs statbuf;
	struct package send_package;
	struct package recv_package;
	memset(&send_package, 0, sizeof(struct package));
	memset(&recv_package, 0, sizeof(struct package));
	
	DPRINT("ENTER PROCESS UP");
	if(tpack.state == STATE_UPFILE_REQ)
	{
		strcat(filepath, pwd);
		strcat(filepath, "/");
		strcat(filepath, tpack.ufc.filename);

		if(statfs(filepath, &statbuf) != -1)/* judge the filename wheather is not exist */
		{
			send_package = pack(CMD_UP, STATE_UPFILE_EXT, &tpack.ufc, 0, NULL);
			send_data(ssl, &send_package, sizeof(struct package));	
			return;
		}
		
		memset(&statbuf, 0, sizeof(struct statfs));
		if(statfs(pwd, &statbuf) == -1)
			err_msg("statfs error");
		if((statbuf.f_bsize*statbuf.f_bfree) < tpack.count)
		{
			send_package = pack(CMD_UP, STATE_UPFILE_SPACE_FULL, &tpack.ufc, 0, NULL);
			send_data(ssl, &send_package, sizeof(struct package));
			return ;
		}
		
		
		/* touch file */
		if((fd = open(filepath, O_WRONLY|O_CREAT), S_IRUSR|S_IWUSR|S_IXUSR) == -1)
		{
			perror("create and open file error");
			send_package = pack(CMD_UP, STATE_UPFILE_CRTFILE_ERR, &tpack.ufc, 0, NULL);
			send_data(ssl, &send_package, sizeof(struct package));	
			return;
		}
		send_package = pack(CMD_UP, STATE_UPFILE_CFM, &tpack.ufc, 0, NULL);
		memset(&send_package, 0, sizeof(struct package));
		send_data(ssl, &send_package, sizeof(struct package));	
	}
	else
	{
		return;
	}
	/*start transmit */
	while(1)
	{
		memset(&recv_package, 0, sizeof(struct package));
		recv_data(ssl, &recv_package, sizeof(struct package));
		if(recv_package.state == STATE_UPFILE_END) /* check the client disk is end */
		{
			close(fd);
			DPRINT("RECEVE END");
			return;
		}
		if(recv_package.state == STATE_UPFILE_PRO)
		{    
			if(write(fd, &recv_package.buf, recv_package.count) == -1)
			{
				DPRINT("WRITE FAILED");
				close(fd);
				return;
			}
		}
	}
}

void process_cmds(void *ssl, struct package tpack, char *pwd)
{
	int ret = 0;
	char filepath[BUFLEN] = "";
	int count = 0;
	int filesize = 0;
	char cmds[BUFLEN] = "";

	struct package send_package;
	struct package recv_package;
	
	DPRINT("ENTER PROCESS cmd");
	if(tpack.state == 82)
	{
		strcpy(cmds, tpack.ufc.cmds);
		ret = process_cmd(cmds, pwd);
	}
	memset(&send_package, 0, sizeof(struct package));
	if(ret) //right
		send_package = pack(CMD_CMD, 00, NULL, 0, NULL);
	else
		send_package = pack(CMD_CMD, 02, NULL, 0, NULL); //error
	send_data(ssl, &send_package, sizeof(struct package));	
}

void send_no_free_connect(void *fd)
{
	struct package send_package;
	send_package = pack(CMD_LOGIN, 04, NULL, 0, NULL);
	send_data(fd, &send_package, sizeof(struct package)); 
}



void unpack(void *fd, struct package tpack, char *pwd)
{
	struct package send_package;
	memset(&send_package, 0, sizeof(struct package));

/*	pthread_t down_id, up_id;*/
	
	switch(tpack.cmd)
	{
		case CMD_DOWN:
/*			pthread_create(&down_id, NULL, thread_down, &thread_info);*/
			process_downfile(fd, tpack, pwd);
			break;
		case CMD_UP:
/*			pthread_create(&up_id, NULL, thread_up, &thread_info);*/
			process_upfile(fd, tpack, pwd);
			break;
		case CMD_LIST:
			process_filelist(fd, tpack, pwd);
			break;
		case CMD_CMD:
			process_cmds(fd, tpack, pwd);
			break;
		case CMD_LOGIN:
			process_lgoin(fd, tpack, pwd);
			break;
		default:
			break;
	}

}
void start_commu(void *fd, char *pwd)
{
	int *newfd = fd;
	struct package recv_package;
	while(1)
	{
		DPRINT("begin ssl_read\n");
		memset(&recv_package, 0, sizeof(struct package));
		if(recv_data(newfd, &recv_package, sizeof(struct package)) == 0)
		{
			perror("ssl read error \n");
			break;
		}
		DPRINT("ssl read end\n");
		if(recv_package.cmd == CMD_QUIT)
		{
			close(*newfd);
			break;
		}
		else
		{
			DPRINT("begin unpack");
			unpack(newfd, recv_package, pwd);
		}
	}
}

int check_user(char *user)
{
	int fd;
	char buf[ULEN+1] = "";
	int count = 0;
	DPRINT("begin check user");

	if((fd = open(USER_FILE, O_RDONLY)) < 0)
	{
		printf("can not open user file!\n");
		exit(-1);
	}
	
	DPRINTS("USERNAME: ", user);
	do{

		if((count = read(fd, buf, ULEN)) < 0)
		{
			printf("can't not read from user file!\n");
			exit(-1);
		}
		buf[ULEN+1] = '\0';
		DPRINTS("BUF: ", buf);
		if(strcmp(user, buf) == 0) 
			return 1; 
	}while(count);
	return -1;
}

