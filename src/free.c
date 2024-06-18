#include <stdlib.h>
#include <regex.h>
#include "httpserver.h"

void httpserver_free(HTTPServer *server) {
	__Route *route = server->routes;

	while (route != NULL) {
		__Route *next = route->next;

		regfree(&route->path);

		{
			__Middleware *current = route->middlewares;

			while (current != NULL) {
				__Middleware *next = current->next;
				free(current);
				current = next;
			}
		}

		free(route);
		route = next;
	}
}

void httpserver_request_free(HTTPRequest *req) {
	for (int i = 0; i < req->headers.size; ++i) {
		entry *e = req->headers.elements[i];

		while (e != NULL) {
			free((void *) e->val);
			e = e->next;
		}
	}
	req->headers.free(req->headers);

	for (int i = 0; i < req->params_len; i++)
		if (req->params[i] != NULL)
			free(req->params[i]);
	free(req->params);
}

void httpserver_response_free(HTTPResponse *res) {
	for (int i = 0; i < RES_TO_FREE_SIZE; i++)
		if (res->to_free[i] != NULL)
			free(res->to_free[i]);
}
