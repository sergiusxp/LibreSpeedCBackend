typedef struct
{
	char* key;
	char* value;
	struct param *next;
} param;

typedef struct
{
    char *method;
    char *path;
	char *query;
    struct param *params;
    char *ip;
} http_request;

int is_numeric(char *str)
{
    int status = 0,
        i = 0;
    char c;

    if(str == NULL)
    {
        return 0;
    }

    while((c = str[i]) != '\0')
	{
		if(isdigit(c) == 0)
        {
            status = 1;
        }

        if (status == 1)
        {
            return 0;
        }

        i++;
	}
	
	return 1;
}

void get_random_bytes(uint8_t *pseudo_array, int bytes)
{
    srand(time(NULL));

    for(int j = 0; j < bytes; j ++)
    {
        pseudo_array[j] = (uint8_t)(rand() % 256);
    }
}

void get_client_ip(struct sockaddr *client, char *ip)
{
    struct sockaddr_in* pV4Addr = client;
    struct in_addr ipAddr = pV4Addr->sin_addr;
    
    inet_ntop(AF_INET, &ipAddr, ip, INET_ADDRSTRLEN);
    ip[INET_ADDRSTRLEN - 1] = 0;
}

param* create_parameter(char* key, char* value)
{
    param* newPar = (param*) malloc(sizeof(param));

    newPar->key = (char*) malloc(MAXKEYVALUESIZE * sizeof(char));
    newPar->value = (char*) malloc(MAXKEYVALUESIZE * sizeof(char));
    strcpy(newPar->key, key);
    strcpy(newPar->value, value);
    newPar->next = NULL;
    
    return newPar;
}

param* add_parameter(param* head, char* key, char* value)
{
    param* newPar = create_parameter(key, value);
    param* temp = head == NULL ? NULL : (head->next);
    
    if(head != NULL)
    {
        head->next = newPar;
    }
    
    newPar->next = temp;
    
    return newPar;
}

void free_param(param* head)
{
    while (head != NULL)
    {
        param* next = head->next;

        if (head->key != NULL) 
        {
            free(head->key);
        }

        if (head->value != NULL) 
        {
            free(head->value);
        }

        free(head);
        head = next;
    }
}

int write_common_headers(char *buf)
{
    return sprintf(buf, "Server: LibreSpeedBackend/1.0.0\r\n");
}

int complete_headers(char *buf)
{
    return sprintf(buf, "\r\n");
}

int write_reponse(char *buf, int endpoint_exists)
{
    if(endpoint_exists == 0)
    {
        return sprintf(buf, "HTTP/1.1 404 Not Found\r\n");
    }
    else if(endpoint_exists == -1)
    {
        return sprintf(buf, "HTTP/1.1 405 Method Not Allowed\r\n");
    }

    return sprintf(buf, "HTTP/1.1 200 OK\r\n");
}

param* parse_query_params(char *raw, param* head)
{
    int index = 0;
    char *end_str,
         *query = malloc(MAXDATASIZE * sizeof(char)),
         *token,
         c;

    strcpy(query, raw);
    token = strtok_r(query, "&", &end_str);
 
    while (token != NULL)
    {
        char *end_token, *token2 = strtok_r(token, "=", &end_token);

        head = add_parameter(head, token, "");

        while (token2 != NULL)
        {
            if(token2 != NULL && strcmp(head->key, token2) != 0)
            {
                strcpy(head->value, token2);
            }

            ++index;
            token2 = strtok_r(NULL, "=", &end_token);
        }

        token = strtok_r(NULL, "&", &end_str);
    }

    free(query);

    return head;
}

void parse_path_and_query(char *raw, http_request *request)
{
    request->query = calloc(MAXDATASIZE, sizeof(char));
    request->path = calloc(MAXDATASIZE, sizeof(char));
    request->method = calloc(MAXKEYVALUESIZE, sizeof(char));
    request->ip = calloc(MAXDATASIZE, sizeof(char));

    if(raw == NULL)
    {
        return;
    }

    char *first_occ = strchr(raw, '\n'),
         *first_line,
         *token,
         *end_space,
         *start_query,
         *end_query;
    param* head = create_parameter("", "");
    param *list = head;

    if(first_occ != NULL)
    {
        first_line = malloc(MAXDATASIZE * sizeof(char));
        strncpy(first_line, raw, first_occ - raw);
        first_line[first_occ - raw] = 0;

        token = strtok_r(first_line, " ", &end_space);
        
        if(token != NULL)
        {
            strcpy(request->method, token);
        }
        else
        {
            strcpy(request->method, "");
        }
        
        token = strtok_r(NULL, " ", &end_space);

        if(token != NULL)
        {
            strcpy(request->path, token);
            strcpy(request->query, "");

            start_query = strchr(token, '?');

            if(start_query != NULL)
            {
                request->path[start_query - token] = 0;

                if(strlen(start_query) > 1)
                {
                    strcpy(request->query, start_query + 1);
                }
                else
                {
                    strcpy(request->query, "");
                }

                head = parse_query_params(request->query, head);
                request->params = list->next;
            }
            else
            {
                request->params = NULL;
            }
        }
        else
        {
            strcpy(request->path, "");
            strcpy(request->query, "");
        }

        free(first_line);
    }
}

char* get_param_value(http_request *http_req, char *param_to_check)
{
    if(http_req == NULL)
    {
        return;
    }

    param *tmp = http_req->params;

    if(tmp == NULL)
    {
        return NULL;
    }

    while(tmp != NULL)
    {
        if(tmp->key[0] == '\0')
        {
            break;
        }

        if(strcmp(tmp->key, param_to_check) == 0)
        {
            return tmp->value;
        }

        tmp = tmp->next;
    }

    return NULL;
}

int get_param_exists(http_request *http_req, char *param_to_check)
{
    if(http_req == NULL)
    {
        return 0;
    }

    param *tmp = http_req->params;

    if(tmp == NULL)
    {
        return 0;
    }

    while(tmp != NULL)
    {
        if(tmp->key == NULL)
        {
            break;
        }

        if(tmp->key[0] == '\0')
        {
            break;
        }

        if(strcmp(tmp->key, param_to_check) == 0)
        {
            return 1;
        }

        tmp = tmp->next;
    }

    return 0;
}

int get_headers_and_body(char *raw, http_request *request, int socket)
{
    uint8_t *buffer = malloc(ONE_MBYTE * sizeof(uint8_t));
    int content_length = 0,
        ctr = 0,
        found = 0,
        tmp;
    char *end_str, 
         *header = malloc(ONE_MBYTE * sizeof(char)),
         *token,
         *cl;

    strcpy(header, raw);
    token = strtok_r(header, "\r\n", &end_str);

    while (token != NULL)
    {
        if(strcmp(token, "") == 0 || strcmp(token, "\r\n") == 0)
        {
            break;
        }

        if(strncmp("Content-Length:", token, 15) == 0)
        {
            found = 1;
            cl = malloc(ONE_MBYTE * sizeof(char));
            strcpy(cl, token + 15);
            content_length = atoi(cl);

            free(cl);
        }
        else if(strncmp("X-Forwarded-For:", token, 16) == 0)
        {
            strcpy(request->ip, token + 16);
        }

        token = strtok_r(NULL, "\r\n", &end_str);
    }
    
    if(found == 1)
    {
        if(content_length == 0)
        {
            free(buffer);
            free(header);
            
            return;
        }

        for(;;)
        {
            if(ctr >= content_length)
            {
                break;
            }

            if(!check_socket_still_open(socket))
            {
                break;
            }
            
            if(ctr + ONE_MBYTE > content_length)
            {
                tmp = recv(socket, buffer, (content_length - ctr) * sizeof(uint8_t), 0);
            }
            else
            {
                tmp = recv(socket, buffer, ONE_MBYTE * sizeof(uint8_t), 0);
            }

            if(tmp <= 0)
            {
                free(header);
                free(buffer);

                close(socket);

                return 1;
            }

            ctr += tmp;
        }
    }
    else if(strcmp(request->method, "POST") == 0)
    {
        for(;;)
        {
            if(!check_socket_still_open(socket))
            {
                free(header);
                free(buffer);

                return 1;
            }

            int tmp = recv(socket, buffer, ONE_MBYTE * sizeof(uint8_t), 0);

            if(tmp <= 0)
            {
                free(header);
                free(buffer);

                close(socket);

                return 1;
            }
        }
    }

    free(header);
    free(buffer);

    return 0;
}

int check_endpoint(http_request *request)
{
    int i,
        total = TOT_ENDPOINTS,
        found = 0;

    for(i = 0; i < total; i++)
    {
        if(strcmp(endpoints[i].path, "") == 0)
        {
            return 0;
        }

        if(strcmp(request->path, endpoints[i].path) == 0)
        {
            found = 1;

            if(strcmp(request->method, endpoints[i].method) == 0)
            {
                return 1;
            }
        }
    }

    return found == 1 ? -1 : 0;
}

int check_socket_still_open(int socket)
{
    return fcntl(socket, F_GETFD) != -1 || errno != EBADF;
}