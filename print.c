#include "print.h"
#include <stdlib.h>
#include <stdio.h>

void print_login_menu()
{
	system("clear");
	printf("\033[36m***********\033[0m\033[34mWelcome to Secure File Storage System\033[0m\033[36m**************\n\033[0m");
	printf("\033[36m*\033[0m \033[31m        ******     ******     ******     ******   \033[0m  \033[36m       *\n\033[0m\033[0m");
	printf("\033[36m*\033[0m \033[31m       **          *         **         **        \033[0m  \033[36m       *\n\033[0m\033[0m");
	printf("\033[36m*\033[0m \033[31m        *****      ******     *****      *****    \033[0m  \033[36m       *\n\033[0m\033[0m");
	printf("\033[36m*\033[0m \033[31m            **     *              **         **   \033[0m  \033[36m       *\n\033[0m\033[0m");
	printf("\033[36m*\033[0m \033[31m       ******      *         ******     ******  \033[0m \033[34mLuke\033[0m  \033[36m*\n\033[0m\033[0m");
	printf("\033[36m**************************************************************\n\033[0m");
}

void print_main_menu()
{
	system("clear");
	printf("  \033[34m***********Server console***********\033[0m\n");
	printf("  \033[34m*\033[0m          1.Configure             \033[34m*\033[0m\n");
	printf("  \033[34m*\033[0m          2.Run server            \033[34m*\033[0m\n");
	printf("  \033[34m*\033[0m          3.Stop server           \033[34m*\033[0m\n");
	printf("  \033[34m*\033[0m          4.Show status           \033[34m*\033[0m\n");
	printf("  \033[34m*\033[0m          5.Exit                  \033[34m*\033[0m\n");
	printf("  \033[34m************************************\033[0m\n");
	printf("  Please input the server command number:");
}

void print_cfg_menu()
{
	system("clear");
	printf("  \033[34m***************Configure**************\033[0m\n");
	printf("  \033[34m*\033[0m        1.Set maximum client        \033[34m*\033[0m\n");
	printf("  \033[34m*\033[0m        2.Add admin account         \033[34m*\033[0m\n");
	printf("  \033[34m*\033[0m        3.Add client account        \033[34m*\033[0m\n");
	printf("  \033[34m*\033[0m        4.Go back                   \033[34m*\033[0m\n");
	printf("  \033[34m**************************************\033[0m\n");
	printf("  Please input the configuration command number:");
}


