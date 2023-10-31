#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../httpserver.h"

#define PORT 8080

void home(HTTPRequest req, HTTPResponse *res) {
	printf("Hello from <home>\n");
	printf("Content - %s: \"%s\"\n", req.content_type, req.content);

	for (int i = 0; i < req.params_len; i++)
		printf("%d - \"%s\"\n", i, req.params[i]);
}

int main(void) {
	HTTPServer server;
	httpserver_init(&server, PORT);

	httpserver_route(
		&server,
		POST,
		"^/$",
		&home,
		NULL
	);

	httpserver_listen(&server);
	httpserver_free(&server);

	return 0;
}
