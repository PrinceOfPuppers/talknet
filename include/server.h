#pragma once
#include "socket-wrapper.h"

// Server is for recieving incomming connections from peers

#define LISTEN_PORT 8234

#define AWAIT_CONN_QUEUE_LEN 3

int await_connections(Conn *listen_conn, Conn_pool *conn_pool);