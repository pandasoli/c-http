#include <stdio.h>
#include <stdlib.h>
#include "httpserver.h"


int httpserver_route(HTTPServer *server, int method, char *path, __RouteFn fn, __MiddlewareFn *middlewares) {
	__Route *route = malloc(sizeof(__Route));

	if (route < 0) {
		perror("malloc failed");
		return 1;
	}

	if (regcomp(&route->path, path, REG_EXTENDED)) {
		perror("compile regex failed");
		return 1;
	}

	route->method = method;
	route->fn = fn;
	route->next = server->routes;

	if (middlewares != NULL) {
		int i = 0;

		__MiddlewareFn fn = middlewares[i];
		__Middleware *last = NULL;

		while (fn != NULL) {
			__Middleware *middleware = malloc(sizeof(__Middleware));
			middleware->fn = fn;
			middleware->next = last;

			last = middleware;
			fn = middlewares[++i];
		}

		route->middlewares = last;
	}

	server->routes = route;

	return 0;
}
