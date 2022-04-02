#define MAXDATASIZE 4096
#define SERVERPORT 8089
#define MAXKEYVALUESIZE 255
#define MIN_CHUNKS 4
#define MAX_CHUNKS 1024
#define ONE_MBYTE 1048576
#define TOT_ENDPOINTS 7
#define IP_SIZE 16
#define SOCKET_ALIVE 10
#define SOCKET_DEAD 9

typedef struct {
    char *method;
    char *path;
} endpoint;

typedef struct
{ 
    void* sock_desc; 
    char* ip; 
} arg_struct;

endpoint endpoints[TOT_ENDPOINTS] = {
    [0] = { .method = "GET", .path = "/index" },
    [1] = { .method = "GET", .path = "/empty" },
    [2] = { .method = "GET", .path = "/garbage" },
    [3] = { .method = "GET", .path = "/getIP" },
    [4] = { .method = "POST", .path = "/empty" },
    [5] = { .method = "OPTIONS", .path = "/empty" },
    [6] = { .method = "GET", .path = "/" }
};

char *not_allowed_msg = "<h1>405</h1>method not allowed\r\n",
     *not_found_msg = "<h1>404</h1>endpoint does not exists\r\n";