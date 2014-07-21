#include "server.h"
#include "util.h"
#include "thread_pool.h"
#include "config.h"
#include "log.h"
#include <pthread.h>
#include "comm.h"
#include <sys/types.h>
#include <sys/socket.h>
#include "openssl.h"
#include <netinet/in.h>
#include <string.h>


/*struct thread_info*/
threadpool_t*	thiz = NULL;

static current_client_num = 0;
static max_client_num = MAX_CLIENT_NUM;
static void server_control(int sockfd, void *arg);
static void process(void *arg);

void * server_thread(void *arg)
{
	int sockfd;

	ssl_init();
	sockfd = server_init();
	thiz = threadpool_create(THREADNUM);
	server_control(sockfd, arg);
	close(sockfd);
	ssl_free();
	threadpool_destroy(thiz);
}

int server_init()
{
	int fd;
	int err;
	int reuse = 1;
	struct sockaddr_in	server_addr;

	if((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		err_msg("socket error");
	if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) < 0)
		err_msg("setsockopt error");

	memset(&server_addr, 0, sizeof(struct sockaddr_in));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(PORT_NUM);

	if(bind(fd, (struct sockaddr*)(&server_addr), sizeof(struct sockaddr)) < 0)
		err_msg("bind error");	
	/*****need alter************/

	if(listen(fd, max_client_num) == -1)
	{
		err_msg("listen error");
	}
	return fd;
}

void server_control(int sockfd, void *arg)
{
	int newfd;
	
	struct shared_info *info;
	info = (struct shared_info*)arg;

	while(1)
	{
		switch(info->state)
		{
			case STATE_RUN:
				if((newfd = accept(sockfd, NULL, NULL)) == -1)
				{
					logV("accept error");
					break;
				}
				threadpool_add_task(thiz, process, &newfd);
				break;
			case STATE_STOP:
				break;
			case STATE_EXIT:
				break;
			default:
				break;
		}
	}		
	DPRINT(" NOT BECOME");
}


void process(void *arg)
{
	logV("new process thread created!\n");
	int *fd = (int *)arg;
	void *new_fd;
	char *pwd = NULL;
	pwd = (char *)malloc(MAXBUF);
	memset(pwd, 0, MAXBUF);

	new_fd = get_ssl_fd(fd);
	if(new_fd == NULL)
	{
		err_msg("get_ssl_fd error");
	}
	
	DPRINT("ssl_accept scuess\n");

	if(++current_client_num > MAX_CLIENT_NUM)
	{
		//send_nofree_connect(new_fd);
		--current_client_num;
		close_ssl_fd(new_fd);
		close(fd);
		free(pwd);
		return;
	}
	else
	{
	//	send_connected(new_fd);
	}
	start_commu(new_fd, pwd);
	
	--current_client_num;
	free(pwd);
	pwd = NULL;
	DPRINT("PROCCESS IN over");
}
