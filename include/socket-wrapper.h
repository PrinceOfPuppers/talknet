#pragma once

#include<arpa/inet.h>
#include <pthread.h>

#define IN_BUFF_SIZE 2000
#define OUT_BUFF_SIZE 500


struct Conn{
    int sock_fd;
    pthread_t thread_id;

    struct sockaddr_in net_info;

    char *in_buffer;
    char *out_buffer;

    pthread_mutex_t disconnect_conn_mutex;
};
typedef struct Conn Conn;

void init_conn(Conn *c);
void disconnect(Conn *c);
void free_conn_buffers(Conn *c);

int out_buffer_to_sock(Conn *c,int len);
int sock_to_in_buffer(Conn *c);

// TODO: temp function, stdout_mutex must be initalized
extern pthread_mutex_t stdout_mutex;

//void out_buffer_to_stdout(Conn *c, pthread_mutex_t stdout_mutex);

/////////////////////////
//    conn pooling     //
/////////////////////////
#define CONN_POOL_SIZE 10

struct Conn_pool{
    Conn pool[CONN_POOL_SIZE];
    pthread_mutex_t mutex;
};

typedef struct Conn_pool Conn_pool;

void init_conn_pool(Conn_pool *conn_pool);
void free_conn_pool(Conn_pool *conn_pool);

Conn *get_free_conn(Conn_pool *conn_pool);

