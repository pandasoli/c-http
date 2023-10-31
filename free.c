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
