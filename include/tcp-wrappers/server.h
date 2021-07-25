#pragma once
#include "socket-wrapper.h"

// Server is for recieving incomming connections from peers

#define LISTEN_PORT 8234

int await_connections(Conn_pool *conn_pool, void (*server_client_main_pntr)(Conn *, void *), void *server_client_main_args);