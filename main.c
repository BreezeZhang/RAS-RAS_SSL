#include "server.h"
#include "menu.h"
#include "util.h"
#include <pthread.h>
#include <string.h>
#include "main.h"

int main()
{
	pthread_t server_id;
	pthread_t menu_id;
	struct shared_info	info;

	memset(&info, 0, sizeof(struct shared_info));
	info.state = STATE_STOP;

	if((pthread_create(&server_id, NULL, (void *)server_thread, &info)) != 0)
	{
		err_msg("\033[33m Create control server thread error!\033[0m");
	}
	if((pthread_create(&menu_id, NULL, (void *)menu_thread, &info)) != 0)
	{
		err_msg("pthread_create menu process error");
	}
	
	pthread_join(server_id, NULL);
	pthread_join(menu_id, NULL);

	return 0;
}
