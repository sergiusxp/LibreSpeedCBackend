uint8_t *garbage_data = NULL;

void init_garbage()
{
    garbage_data = malloc(ONE_MBYTE * sizeof(uint8_t));
    
    get_random_bytes(garbage_data, ONE_MBYTE);
}

void call_index(int socket)
{
    char *buffer = calloc(MAXDATASIZE, sizeof(char)),
         *html_content = "Welcome to <b>LibreSpeed</b> <b>C Backend</b>.";
    int written_bytes = 0;

    written_bytes += sprintf(buffer + written_bytes, "Content-Type: text/html\r\n");
    written_bytes += sprintf(buffer + written_bytes, "Connection: keep-alive\r\n");
    written_bytes += sprintf(buffer + written_bytes, "Content-Length: %d\r\n", strlen(html_content));
    written_bytes += sprintf(buffer + written_bytes, "\r\n");

    written_bytes += sprintf(buffer + written_bytes, "%s", html_content);

    send(socket, buffer, written_bytes, 0);

    free(buffer);
}

void call_empty(http_request *http_req, int socket)
{
    char *buffer = calloc(MAXDATASIZE, sizeof(char));
    int p_exists = get_param_exists(http_req, "cors"),
        written_bytes = 0;
    
    if(p_exists == 1)
    {
        written_bytes += sprintf(buffer + written_bytes, "Access-Control-Allow-Origin: *\r\n");
        written_bytes += sprintf(buffer + written_bytes, "Access-Control-Allow-Methods: GET, POST\r\n");
        written_bytes += sprintf(buffer + written_bytes, "Access-Control-Allow-Headers: Content-Encoding, Content-Type\r\n");
    }

    written_bytes += sprintf(buffer + written_bytes, "Content-Type: text/html\r\n");
    written_bytes += sprintf(buffer + written_bytes, "Cache-Control: no-store, no-cache, must-revalidate, max-age=0, s-maxage=0\r\n");
    written_bytes += sprintf(buffer + written_bytes, "Cache-Control: post-check=0, pre-check=0\r\n");
    written_bytes += sprintf(buffer + written_bytes, "Pragma: no-cache\r\n");
    written_bytes += sprintf(buffer + written_bytes, "Connection: keep-alive\r\n");
    written_bytes += sprintf(buffer + written_bytes, "Content-Length: 0\r\n");
    written_bytes += sprintf(buffer + written_bytes, "\r\n");

    send(socket, buffer, written_bytes, 0);

    free(buffer);
}

int determine_chunks(http_request *http_req)
{
    int p_exists = get_param_exists(http_req, "ckSize"),
        chunks = 0;
    char *p_val = get_param_value(http_req, "ckSize");

    if (p_exists == 0 || is_numeric(p_val) == 0 || (chunks = atoi(p_val)) < 1)
    {
        return MIN_CHUNKS;
    }
    else
    {
        if(chunks > 1024)
        {
            return MAX_CHUNKS;
        }
    }

    return chunks;
}

void call_garbage(http_request *http_req, int socket)
{
    int chunks = determine_chunks(http_req),
        p_exists = get_param_exists(http_req, "cors"),
        i,
        written_bytes = 0,
        check;
    char *buffer = calloc(MAXDATASIZE, sizeof(char));
    
    if(p_exists == 1)
    {
        written_bytes += sprintf(buffer + written_bytes, "Access-Control-Allow-Origin: *\r\n");
        written_bytes += sprintf(buffer + written_bytes, "Access-Control-Allow-Methods: GET, POST\r\n");
    }

    // Indicates a file download
    written_bytes += sprintf(buffer + written_bytes, "Content-Description: File Transfer\r\n");
    written_bytes += sprintf(buffer + written_bytes, "Content-Type: application/octet-stream\r\n");
    written_bytes += sprintf(buffer + written_bytes, "Content-Disposition: attachment; filename=random.dat\r\n");
    written_bytes += sprintf(buffer + written_bytes, "Content-Transfer-Encoding: binary\r\n");

    // Cache settings: never cache this request
    written_bytes += sprintf(buffer + written_bytes, "Cache-Control: no-store, no-cache, must-revalidate, max-age=0, s-maxage=0\r\n");
    written_bytes += sprintf(buffer + written_bytes, "Cache-Control: post-check=0, pre-check=0\r\n");
    written_bytes += sprintf(buffer + written_bytes, "Pragma: no-cache\r\n");
    written_bytes += sprintf(buffer + written_bytes, "Content-Length: %d\r\n", chunks * ONE_MBYTE * sizeof(uint8_t));
    written_bytes += sprintf(buffer + written_bytes, "\r\n");

    send(socket, buffer, written_bytes, 0);

    for(i = 0; i < chunks; i ++)
    {
        if(!check_socket_still_open(socket))
		{
			break;
		}

        check = send(socket, garbage_data, ONE_MBYTE * sizeof(uint8_t), 0);
        
        if(check <= 0)
        {
            break;
        }
    }

    free(buffer);
}

void get_ip(http_request *http_req, int socket)
{
    char *buffer = calloc(MAXDATASIZE, sizeof(char));
    int p_exists = get_param_exists(http_req, "cors"),
        written_bytes = 0;
    
    if(p_exists == 1)
    {
        written_bytes += sprintf(buffer + written_bytes, "Access-Control-Allow-Origin: *\r\n");
        written_bytes += sprintf(buffer + written_bytes, "Access-Control-Allow-Methods: GET, POST\r\n");
        written_bytes += sprintf(buffer + written_bytes, "Access-Control-Allow-Headers: Content-Encoding, Content-Type\r\n");
    }

    written_bytes += sprintf(buffer + written_bytes, "Content-Type: text/html\r\n");
    written_bytes += sprintf(buffer + written_bytes, "Content-Length: %d\r\n", strlen(http_req->ip));
    written_bytes += sprintf(buffer + written_bytes, "\r\n");
    written_bytes += sprintf(buffer + written_bytes, "%s", http_req->ip);
    written_bytes += sprintf(buffer + written_bytes, "\r\n");

    send(socket, buffer, written_bytes, 0);

    free(buffer);
}