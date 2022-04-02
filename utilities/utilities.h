#include "utilities.c"

int is_numeric(char *str);
void get_random_bytes(uint8_t *pseudo_array, int bytes);
void get_client_ip(struct sockaddr *client, char *str);
param* create_parameter();
param* add_parameter(param* head, char* key, char* val);
void free_param(param* head);
int write_common_headers(char *buf);
int complete_headers(char *buf);
int write_reponse(char *buf, int endpoint_exists);
param* parse_query_params(char *query, param *head);
void parse_path_and_query(char *header, http_request *request);
int get_param_exists(http_request *http_req, char *param);
char* get_param_value(http_request *http_req, char *param);
int get_headers_and_body(char *header, http_request *request, int socket);
int check_endpoint(http_request *request);
int check_socket_still_open(int socket);