#include <stdlib.h>
#include <string.h>
#include "httpserver.h"


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

	// process content type
	{
		char *key = "Content-Type: ";
		char *key_start = strstr(buffer, key);

		if (key_start != NULL) {
			char *val_start = key_start + strlen(key);
			size_t val_len = strcspn(val_start, "\r\n");

			req.content_type = malloc(val_len + 1);
			strncpy(req.content_type, val_start, val_len);
			req.content_type[val_len] = 0;
		}
		else
			req.content_type = NULL;
	}

	// process content
	{
		char *content = strstr(buffer, "\r\n\r\n");
		if (content != NULL) content += 4;

		req.content = content;
	}

	return req;
}
