#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/socket.h>
#include "httpserver.h"


static void call_route(regmatch_t *route_matches, char *url, __Route *route, int client_fd, char *buffer) {
	HTTPRequest req = make_request(buffer, url, route_matches);

	HTTPResponse res = {
		.status = 200,
		.status_msg = "OK",
		.mime_type = "text/plain",
		.body = "",
		.to_free = {0}
	};

	int allowed = 1;

	{
		__Middleware *current = route->middlewares;

		while (current != NULL) {
			int res_ = current->fn(req, &res);

			for (int i = 0; i < RES_TO_FREE_SIZE; i++)
				if (res.to_free[i] != NULL)
					free(res.to_free[i]);

			if (res_ != 0) {
				allowed = 0;
				break;
			}

			current = current->next;
		}
	}

	if (allowed)
		route->fn(req, &res);

	snprintf(buffer, BUFFER_SIZE,
		"HTTP/1.1 %d %s\r\n"
		"Content-Type: %s\r\n"
		"\r\n"
		"%s",
		res.status, res.status_msg,
		res.mime_type,
		res.body);
	buffer[BUFFER_SIZE] = 0;

	// send HTTP response to client
	send(client_fd, buffer, strlen(buffer), 0);

	// free used memory
	for (int i = 0; i < req.params_len; i++)
		if (req.params[i] != NULL)
			free(req.params[i]);
	for (int i = 0; i < RES_TO_FREE_SIZE; i++)
		if (res.to_free[i] != NULL)
			free(res.to_free[i]);
}

void *handle_client(__ClientHandlerArgs *args) {
	char *buffer = malloc(BUFFER_SIZE + 1);

	if (buffer == NULL) {
		perror("malloc failed");
		return NULL;
	}

	// receive request data from client and store into buffer
	{
		ssize_t bytes_received = recv(args->client_fd, buffer, BUFFER_SIZE, 0);

		if (bytes_received <= 0) {
			close(args->client_fd);
			free(args);
			free(buffer);

			return NULL;
		}

		buffer[bytes_received] = 0;
		buffer[BUFFER_SIZE] = 0;
	}

	int method;

	if (buffer[0] == 'G') method = GET;
	else if (buffer[0] == 'P' || buffer[1] == 'O') method = POST;
	else if (buffer[0] == 'P' || buffer[1] == 'U') method = PUT;
	else if (buffer[0] == 'P' || buffer[1] == 'A') method = PATCH;
	else {
		perror("method identification failed");
		return NULL;
	}

	char *path = decode_url(buffer);

	printf("---------\n");
	printf("Buffer:\n\"%s\"\n", buffer);
	printf("Path: \"%s\"\n", path);

	__Route *route = args->routes;
	int found_route = 0;

	while (route != NULL) {
		regmatch_t matches[REQ_MAX_PARAMS];

		if (
			route->method == method &&
			!regexec(&route->path, path, REQ_MAX_PARAMS, matches, 0)
		) {
			call_route(matches + 1, path, route, args->client_fd, buffer);
			found_route = 1;
			break;
		}

		route = route->next;
	}

	if (found_route == 0) {
		snprintf(buffer, BUFFER_SIZE,
			"HTTP/1.1 404 Not Found\r\n"
			"Content-Type: text/plain\r\n");
		buffer[BUFFER_SIZE] = 0;

		// send HTTP response to client
		send(args->client_fd, buffer, strlen(buffer), 0);
	}

	// clear memory
	free(path);
	free(buffer);
	close(args->client_fd);
	free(args);

	return NULL;
}
