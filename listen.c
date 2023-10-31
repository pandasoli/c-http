#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include "httpserver.h"

static HTTPServer *gl_server;

void handleSIGINT(int signal) {
	httpserver_free(gl_server);
	close(gl_server->fd);

	exit(0);
}


int httpserver_listen(HTTPServer *server) {
	gl_server = server;

	if (signal(SIGINT, handleSIGINT) == SIG_ERR) {
		perror("register signal handler SIGINT failed");
		return 1;
	}

	// bind socket to port
	if (bind(server->fd, (struct sockaddr *) &server->addr, sizeof(server->addr)) < 0) {
		perror("bind failed");
		return 1;
	}

	// listen for connections
	if (listen(server->fd, 10) < 0) {
		perror("listen failed");
		return 1;
	}

	printf("Server listening on port %d\n", server->port);

	while (1) {
		// client info
		struct sockaddr_in client_addr;
		socklen_t client_addr_len = sizeof(client_addr);
		int client_fd;

		// accept client connections
		if ((
				client_fd = accept(
					server->fd,
					(struct sockaddr *) &client_addr,
					&client_addr_len)
			) < 0) {
			perror("accept failed");
			continue;
		}

		// create a new thread to handle client request
		__ClientHandlerArgs *args = malloc(sizeof(*args));

		args->client_fd = client_fd;
		args->routes = server->routes;

		pthread_t thread_id;
		pthread_create(&thread_id, NULL, (void *(*)(void *)) handle_client, (void *) args);
		pthread_detach(thread_id);
	}

	// close server
	close(server->fd);

	return 0;
}
