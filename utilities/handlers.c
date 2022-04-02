void *connection_handler(void *arguments)
{
	arg_struct *args = arguments;
	int socket = args->sock_desc;

	for (;;)
	{
		char *buffer = malloc(MAXDATASIZE * sizeof(char)),
			 *req_received = malloc(MAXDATASIZE * sizeof(char)),
			 *req_received_body = malloc(MAXDATASIZE * sizeof(char)),
			 *bufptr = req_received;
		int read_size,
			recvBytes = 0,
			tot = 0,
			written_bytes = 0,
			endpoint_exists_and_meth;
		size_t numbytes;
		http_request request;

		memset(buffer, 0, MAXDATASIZE * sizeof(char));
		memset(req_received, 0, MAXDATASIZE * sizeof(char));

		for (;;)
		{
			if(!check_socket_still_open(socket))
			{
				tot = 0;

				break;
			}

			recvBytes = recv(socket, buffer, sizeof(char), 0);

			if(recvBytes <= 0)
            {
                close(socket);
				
                free(buffer);
				free(req_received);
				free(req_received_body);
				free(arguments);

				return 1;
            }
			
			memcpy(bufptr, buffer, recvBytes);
			tot += recvBytes;
			bufptr += recvBytes;
			bufptr[0] = 0;

			if (tot >= 4)
			{
				char *tempptr = bufptr - 4;

				if (tempptr[0] == '\r' && tempptr[1] == '\n' && tempptr[2] == '\r' && tempptr[3] == '\n')
				{
					break;
				}
			}
		}

		if(tot == 0)
		{
			break;
		}

		strcpy(req_received_body, req_received);
		parse_path_and_query(req_received, &request);

		int status = get_headers_and_body(req_received_body, &request, socket);
		
		if(status == 1)
		{
			free(req_received);
			free(req_received_body);
			free_param(request.params);
			free(request.ip);
			free(request.query);
			free(request.path);
			free(request.method);
			free(arguments);

			return 1;
		}

		if(request.ip == NULL || strlen(request.ip) == 0)
		{
			strcpy(request.ip, "127.0.0.1");
		}

		//printf("\n[Debug] Method %s, Path %s, Query %s, IP %s[%d], Socket #%d\n", request.method, request.path, request.query, request.ip, strlen(request.ip), socket);
		//fflush(stdin);

		endpoint_exists_and_meth = check_endpoint(&request);
		memset(buffer, 0, MAXDATASIZE * sizeof(char));
		written_bytes += write_reponse(buffer + written_bytes, endpoint_exists_and_meth);
		written_bytes += write_common_headers(buffer + written_bytes);
		
		if (endpoint_exists_and_meth == 1)
		{
			if (strcmp(request.path, endpoints[6].path) == 0 || strcmp(request.path, endpoints[0].path) == 0)
			{
				send(socket, buffer, written_bytes, 0);
				call_index(socket);
			}
			else if (strcmp(request.path, endpoints[1].path) == 0)
			{
				send(socket, buffer, written_bytes, 0);
				call_empty(&request, socket);
			}
			else if (strcmp(request.path, endpoints[2].path) == 0)
			{
				send(socket, buffer, written_bytes, 0);
				call_garbage(&request, socket);
			}
			else if (strcmp(request.path, endpoints[3].path) == 0)
			{
				send(socket, buffer, written_bytes, 0);
				get_ip(&request, socket);
			}
		}
		else if (endpoint_exists_and_meth == -1)
		{
			written_bytes += sprintf(buffer + written_bytes, "Content-Length: %d\r\n", strlen(not_allowed_msg));
			written_bytes += sprintf(buffer + written_bytes, "\r\n");
			written_bytes += sprintf(buffer + written_bytes, not_allowed_msg);
			
			send(socket, buffer, written_bytes, 0);
		}
		else
		{
			written_bytes += sprintf(buffer + written_bytes, "Content-Length: %d\r\n", strlen(not_found_msg));
			written_bytes += sprintf(buffer + written_bytes, "\r\n");
			written_bytes += sprintf(buffer + written_bytes, not_found_msg);
			
			send(socket, buffer, written_bytes, 0);
		}

		free(buffer);
		free(req_received);
		free(req_received_body);
		free_param(request.params);
		free(request.ip);
		free(request.query);
		free(request.path);
		free(request.method);

		if(!check_socket_still_open(socket))
		{
			break;
		}
	}

	free(arguments);

	return 0;
}
