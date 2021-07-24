#pragma once
#include "socket-wrapper.h"
#include <pthread.h>

// Client is for making outgoing connections to a peer
// returns listener thread id
pthread_t connect_to_peer(Conn_pool *conn_pool, char *ip, int port);