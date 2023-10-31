#pragma once

#include <netinet/in.h>
#include <regex.h>

#define GET 0
#define POST 1
#define PATCH 2
#define PUT 3

#define BUFFER_SIZE 1024
#define RES_TO_FREE_SIZE 20
#define REQ_MAX_PARAMS 10


typedef struct {
	int status;
	char *status_msg;
	char *body;
	char *mime_type;
	void *to_free[RES_TO_FREE_SIZE];
} HTTPResponse;

typedef struct {
	char **params;
	size_t params_len;

	char *content;
	char *content_type;

	/**
	 * This field is used by the application to transmit data from middlewares to routes.
	 */
	void *data;
} HTTPRequest;

typedef void (*__RouteFn)(HTTPRequest req, HTTPResponse *res);
typedef int (*__MiddlewareFn)(HTTPRequest req, HTTPResponse *res);

typedef struct __Middleware {
	__MiddlewareFn fn;
	struct __Middleware *next;
} __Middleware;

typedef struct __Route {
	int method;
	regex_t path;
	__RouteFn fn;
	__Middleware *middlewares;

	struct __Route *next;
} __Route;

typedef struct {
	int client_fd;
	__Route *routes;
} __ClientHandlerArgs;

typedef struct {
	int fd;
	struct sockaddr_in addr;

	int port;
	__Route *routes;
} HTTPServer;

/**
 * Initialize server properties
 * 
 * `server` - An instance of the struct HTTPServer
 * `port` - The port to listen on
 */
int httpserver_init(HTTPServer *server, int port);

/**
 * Start listening for incoming calls
 *
 * `server` - An instance of the struct HTTPServer
 */
int httpserver_listen(HTTPServer *server);

/**
 * Free server properties
 */
void httpserver_free(HTTPServer *server);
void httpserver_request_free(HTTPRequest *req);
void httpserver_response_free(HTTPResponse *req);

/**
 * Create route
 *
 * `server` - An instance of the struct HTTPServer
 * `method` - The HTTP method
 * `path` - e.g. "/home"
 * `fn` - The function to be requested
 * `middlewares` - A null-terminated array of middlewares
 */
int httpserver_route(HTTPServer *server, int method, char *path, __RouteFn fn, __MiddlewareFn *middlewares);

void *handle_client(__ClientHandlerArgs *args);
char *decode_url(char *buffer);
HTTPRequest make_request(char *buffer, char *url, regmatch_t *route_matches);
