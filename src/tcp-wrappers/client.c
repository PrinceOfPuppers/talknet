#include "client.h"

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>



struct _Client_thread_args{
    void (*client_server_main_pntr)(Conn *,void *);
    Conn *c;
    void *client_server_main_args;
};
typedef struct _Client_thread_args _Client_thread_args;

void *_outgoing_conn_thread_target(void *client_thread_args_void){
    _Client_thread_args *client_thread_args = (_Client_thread_args *)client_thread_args_void;

    Conn *c = (Conn *)(client_thread_args->c);

    // calls server_client_main, passed by user of network library
    (*(client_thread_args->client_server_main_pntr))(c, client_thread_args->client_server_main_args);

    puts("disconnecting socket");
    disconnect_conn(c);
    free(client_thread_args);
    c->thread_id = 0;
    return NULL;
}

/////////////////////////////////////////
// client connection establishing code //
/////////////////////////////////////////

pthread_t connect_to_peer(Conn_pool *conn_pool, char *ip, int port, void (*client_server_main_pntr)(Conn *, void *), void *client_server_main_args){
	Conn *c = get_free_conn(conn_pool);

	//Connect to remote server
    if(!connect_conn(c, ip, port)){
        puts("Connect error");
        return -1;
    }

	puts("Connected\n");

    // arguments to pass to thread
    _Client_thread_args *client_thread_args = malloc(sizeof(_Client_thread_args));
    client_thread_args->c = c;
    client_thread_args->client_server_main_pntr = client_server_main_pntr;
    client_thread_args->client_server_main_args = client_server_main_args;

    if( pthread_create(&c->thread_id , NULL , _outgoing_conn_thread_target , client_thread_args) < 0 ){
		perror("could not create thread");
		return -1;
	}
    puts("Handler assigned");
    return c->thread_id;
}