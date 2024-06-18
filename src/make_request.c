#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "httpserver.h"
#include "hashtable.h"


HTTPRequest make_request(char *buffer, char *url, regmatch_t *route_matches) {
	HTTPRequest req;

	// Process number of placeholders
	int params_len = -1;
	while (route_matches[++params_len].rm_so != -1);

	req.params_len = params_len;

	// process placeholders
	req.params = malloc(sizeof(char *) * params_len);

	for (int i = 0; i < params_len; i++) {
		int start = route_matches[i].rm_so;
		int end = route_matches[i].rm_eo;
		int len = end - start;

		// Extract the matched substring
		char *param = malloc(len + 1);
		strncpy(param, url + start, len);
		param[len] = '\0';

		req.params[i] = param;
	}

	// process headers
	req.headers = hashtable_create(10, &djb2);

	char *last_header = buffer;

	while (1) {
		char *key = strstr(last_header, "\r\n") + 2 /* skip \r\n */;

		if (strncmp(key, "\r\n", 2) == 0)
			break;

		int key_len = strcspn(key, ":");

		char *val_start = strstr(key, ": ") + 2 /* skip ": " */;
		int val_len = strcspn(val_start, "\r\n");
		last_header = val_start + val_len;

		char *nval = malloc(val_len);
		strncpy(nval, val_start, val_len);
		nval[val_len] = 0;

		req.headers.insert(&req.headers, key, key_len, nval);
	}

	// process content
	{
		char *content = strstr(buffer, "\r\n\r\n");
		if (content != NULL) content += 4;

		req.content = content;
	}

	return req;
}
