#pragma once

#include<arpa/inet.h>
#include <pthread.h>

#include "rsa.h"


#define OUT_BUFF_SIZE 2002


#define IN_BUFF_SIZE 2000 // must be able to contain: OUT_BUFF_SIZE - HEADER_SIZE

// number of connections waiting in listener queue before additional ones get refuesd
#define AWAIT_CONN_QUEUE_LEN 3


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

// connects conn to remote server
int connect_conn(Conn *c, char *ip, int port);

void disconnect_conn(Conn *c);
void free_conn_buffers(Conn *c);

int send_message(Conn *c, char *message);
int send_messages(Conn *c, int argc, ...);
int sock_to_in_buffer(Conn *c, int blocking);

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


///////////////
// listeners //
///////////////

// setup conn as a listener
int listen_conn(Conn *lc,int port);

// awaits a new connection on listener, blocking, returns established connection
Conn *accept_conn(Conn *lc, Conn_pool *conn_pool);