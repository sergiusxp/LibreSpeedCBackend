#include "endpoints.c"

void call_index(int socket);
void call_empty(http_request *http_req, int socket);
void call_garbage(http_request *http_req, int socket);
void get_ip(http_request *http_req, int socket);