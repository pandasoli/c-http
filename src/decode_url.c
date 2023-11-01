#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "httpserver.h"


char *decode_url(char *buffer) {
	const char *path_start = strchr(buffer, ' ');
	if (path_start == NULL) return NULL;

	path_start++;

	const char *path_end = strpbrk(path_start, " ");
	if (path_end == NULL) return NULL;

	size_t path_len = path_end - path_start;
	size_t hex_amount = 0;

	for (int i = 0; i < path_len; i++)
		if (path_start[i] == '%' && i + 2 < path_len)
			hex_amount++;

	path_len += hex_amount;
	char *path = malloc(path_len + hex_amount + 1);
	size_t path_i = 0;

	for (int i = 0; i < path_len; i++)
		if (path_start[i] == '%') {
			if (i + 2 >= path_len) {
				// Invalid percent encoding
			}

			char hex[] = {
				path_start[i + 1],
				path_start[i + 2],
				0
			};

			path[path_i++] = strtol(hex, NULL, 16);
			i += 2;
		}
		else {
			path[path_i++] = path_start[i];
		}

	path[path_len] = 0;

	return path;
}
