#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <ctype.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/sockios.h>

#include "includes.h"

int main(int argc, char *argv[])
{
	int socket_desc,
		client_sock,
		socket_dim,
		option = 1,
		log_level;
	struct sockaddr_in server,
		   client;

	socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

	if (socket_desc == -1)
	{
		puts("\n[Fatal error] Could not create socket.");
	}

	puts("\n[Debug] Socket created.");

	// workaround needed for some linux distro, sometimes they send random pipes, 
	// causing the program to finish without any error 
	sigset_t set;
	sigaddset(&set, SIGPIPE);
	int retcode = sigprocmask(SIG_BLOCK, &set, NULL);
	
	if (retcode == -1) 
	{
		error("sigprocmask");
	}
	// end workaround

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(SERVERPORT);

	if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		perror("\n[Fatal error] Bind failed.");
		return 1;
	}

	puts("\n[Debug] Bind done.");
	listen(socket_desc, 3);
	puts("\n[Info] Waiting for incoming connections...");
	socket_dim = sizeof(struct sockaddr_in);

	init_garbage();

	for(;;)
	{
		client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&socket_dim);
		
		int z;
		int s;
		struct linger so_linger;
		
		so_linger.l_onoff = 1;
		so_linger.l_linger = 0;
		
		z = setsockopt(socket_desc,
					SOL_SOCKET,
					SO_LINGER,
					&so_linger,
					sizeof so_linger);
		
		if (z) 
		{
			perror("setsockopt(2)");
			
			return 1;
		}

		if(client_sock < 0)
		{
			perror("\n[Fatal error] Accept failed.");

			return 1;
		}

		puts("\n[Info] Connection accepted.");

		pthread_t sniffer_thread;
		arg_struct *args = malloc(sizeof(arg_struct));
		args->sock_desc = client_sock;
		
		if (pthread_create(&sniffer_thread, NULL, connection_handler, (void *)args) < 0)
		{
			perror("\n[Fatal error] Could not create thread.");
			
			free(args);
			
			return 1;
		}
		
		puts("\n[Info] Handler assigned.");
	}

	return 0;
}