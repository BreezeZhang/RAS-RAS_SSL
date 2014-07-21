#include "menu.h"
#include "util.h"
#include "config.h"
#include "print.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>


static void cfg_process();
static void set_max_client();
static void add_client_account();
static void add_admin_account();
static void user_login();

void* menu_thread(void *arg)
{
	int choice;
	struct	shared_info *info = (struct shared_info*)arg;
	
	user_login();
	while(1)
	{
		print_main_menu();
		scanf("%d", &choice);

		switch(choice)
		{
			case 1:
				cfg_process();
				break;
			case 2:
			/*	pthread_kill(*(pthread_t *)arg, SIGCONT);*/
				info->state	= STATE_RUN;
			   	break;
			case 3:
			/*	pthread_kill(*(pthread_t *)arg, SIGSTOP);*/
				info->state = STATE_STOP; 
				break;
			case 4:
				break;
			case 5:
			/*	pthread_kill(*(pthread_t *)arg, SIGKILL);*/
				info->state = STATE_EXIT;
				exit(0);
				break;
			default:
				break;
		}
	}

}

void cfg_process()
{
	int choice;
	int quit = 0;

	while(!quit)
	{
		print_cfg_menu();
		scanf("%d", &choice);

		switch(choice)
		{
			case 1:
				set_max_client();
				break;
			case 2:
				add_admin_account();
				break;
			case 3:
				add_client_account();
				break;
			case 4:
				quit = 1;
				break;
			default:
				break;
		}
	}
}

void set_max_client()
{
	int fd;
	int max_client;
	char buf[MAXBUF] = "\0";

	if((fd = open(CFG_FILE, O_RDONLY)) == -1)
	{
		printf("can't find config file\n");
		getchar();
		return;
	}
	if(read(fd, buf, MAXBUF) <= 0) 
	{
		err_msg("read config file error");
		getchar();
		return;
	}
	max_client = atoi(buf);
	printf("current max clinet is %d\n", max_client);
	printf("please input what you configure max clien: ");
	scanf("%d", &max_client);


}

void add_admin_account()
{
	int fd;
	char adm_name[ALEN+1] = "\0";
	char adm_pwd[ALEN+1] = "\0";
	char admin[2*ALEN+1] = "\0";

	if((fd = open(ADMINFILE, O_WRONLY|O_APPEND)) == -1)
	{
		err_msg("open admin error");
	}

	printf("\033[34mInput the new admin name:");
	scanf("%s", adm_name);
	printf("\033[32mInput the new admin passwd:");
	scanf("%s", adm_pwd);

	int i = 0;			
	for(i = strlen(adm_name); i < ALEN; i++)
			adm_name[i] = '#';
	for(i = strlen(adm_pwd); i < ALEN; i++)
			adm_pwd[i] = '*';
	adm_pwd[ALEN] = '\0';
	adm_name[ALEN] = '\0';
	strcat(admin, adm_name);
	strcat(admin, adm_pwd);

	if(write(fd, admin, strlen(admin)) < 0)
	{
		close(fd);
		err_msg("\033[31mwrite admin.txt error\033[0m\n");
	}
	else
		printf("\033[31madd admin success!\n");
	close(fd);

}

void add_client_account()
{
	int fd;
	char user_name[ALEN+1] = "\0";
	char user_pwd[ALEN+1] = "\0";
	char user[2*ALEN+1] = "\0";

	if((fd = open(USER_FILE, O_WRONLY|O_APPEND)) == -1)
	{
		err_msg("open user.dat error");
	}

	printf("\033[34mInput the new user name:");
	scanf("%s", user_name);
	printf("\033[32mInput the new user passwd:");
	scanf("%s", user_pwd);

	if(mkdir(user_name, 0777)==-1)
	{
		close(fd);
		perror("mkdir user dir error:\n");	
		getchar();
		return;
	}

	int i = 0;			
	for(i = strlen(user_name); i < ALEN; i++)
			user_name[i] = '#';
	for(i = strlen(user_pwd); i < ALEN; i++)
			user_pwd[i] = '*';
	user_pwd[ALEN] = '\0';
	user_name[ALEN] = '\0';
	strcat(user, user_name);
	strcat(user, user_pwd);

	if(write(fd, user, strlen(user)) < 0)
	{
		close(fd);
		err_msg("\033[31mwrite user.dat error\033[0m\n");
	}
	else
		printf("\033[31madd use success!\n");
	close(fd);

}

void user_login()
{
	int nread;
	int fd;
	int flag = 0;
	int num = MAXBUF;
	char adm[MAXBUF] = "\0";
	char adm_name[ALEN+1] = "\0";
	char adm_pwd[ALEN+1] = "\0";
	char *padm = adm;

	if((fd=open(ADMINFILE, O_RDONLY))<0)
	{
		printf("can not open admin.txt!");
		exit(-1);
	}	
	print_login_menu();
	while(num > 0)						
	{
		if((nread=read(fd, padm, ALEN))<0)
		{
			printf("can not read from admin.txt!");	
			exit(-1);
		}
		else if(nread==0)
		{
			break;	
		}
		padm += nread;	
		num -= nread;
	
	}
	close(fd);
	
	while(!flag)
	{
		/**********/
		printf("Admin Id:");
		scanf("%s", adm_name);
		printf("Admin PassWord:");
		scanf("%s", adm_pwd);
		int i = 0;
		for(i = strlen(adm_name); i < ALEN; i++)
			adm_name[i] = '#';
		for(i = strlen(adm_pwd); i < ALEN; i++)
			adm_pwd[i] = '*';
		adm_pwd[ALEN] = '\0';
		adm_name[ALEN] = '\0';

		padm = adm;
		num = MAXBUF - num;
		DPRINTS("adm_name: ", adm_name);
		DPRINTS("adm_pwd: ", adm_pwd);
		DPRINTS("padm: ", padm);
		while((padm != '\0') && (num > 0))
		{
			if(strncmp(padm, adm_name, ALEN) == 0)
			{				
				padm += ALEN;
				if(strncmp(padm, adm_pwd, ALEN) == 0)
				{
					flag = 1;
					break;
				}
			}
			else
			{
				padm += ALEN;
			}
			padm += ALEN;
			num = num - 2*ALEN;
		}
		if(!flag)
		{
			printf("\n\033[33mAdmin name or passwd is error!\033[0m\n\n");
			memset(adm_name, '\0', ALEN);
			memset(adm_pwd, '\0', ALEN);
		}
	}
	printf("\n\033[33mlogin success!\033[0m\n\n");	
}


