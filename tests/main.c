#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "httpserver.h"

#define PORT 8080

void home(HTTPRequest req, HTTPResponse *res) {
	printf("Hello from <home>\n");

	puts("Headers: ");

	for (int i = 0; i < req.headers.size; ++i) {
		if (req.headers.elements[i] == NULL) continue;

		entry *e = req.headers.elements[i];
		while (e != NULL) {
			printf("  \"%s\": \"%s\"\n", e->key, (char *) e->val);
			e = e->next;
		}
	}
	/*printf("Content - %s: \"%s\"\n", req.content_type ? req.content_type : "(nil)", req.content);*/

	for (int i = 0; i < req.params_len; i++)
		printf("%d - \"%s\"\n", i, req.params[i]);

	res->body = "Hello, World!";
}

void number(HTTPRequest req, HTTPResponse *res) {
	char *format = "There's a number to you: %s\n";
	int size = snprintf(NULL, 0, format, req.params[0]) + 1;

	char *msg = malloc(size);
	snprintf(msg, size, format, req.params[0]);

	res->body = msg;
	res->to_free[0] = msg;
}

int main(void) {
	HTTPServer server;
	httpserver_init(&server, PORT);

	httpserver_route(
		&server,
		GET,
		"^/$",
		&home,
		NULL
	);

	httpserver_route(&server, GET, "^/number/([0-9]+)$", &number, NULL);

	httpserver_listen(&server);
	httpserver_free(&server);

	return 0;
}
