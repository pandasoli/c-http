#include <stdio.h>
#include "httpserver.h"


int httpserver_init(HTTPServer *server, int port) {
	// create server socket
	if ((server->fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket failed");
		return 1;
	}

	// config socket
	server->addr.sin_family = AF_INET;
	server->addr.sin_addr.s_addr = INADDR_ANY;
	server->addr.sin_port = htons(port);

	server->port = port;
	server->routes = NULL;

	return 0;
}
