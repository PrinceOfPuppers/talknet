#pragma once
#include "socket-wrapper.h"

// Server is for recieving incomming connections from peers

#define LISTEN_PORT 8234

#define AWAIT_CONN_QUEUE_LEN 3

int start_server(void (*server_client_main)(Conn *, void *), void *server_client_main_args);