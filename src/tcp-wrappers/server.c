#include "server.h"

#include <pthread.h>
#include <stdio.h>
#include <string.h>

struct _Server_thread_args{
    void (*server_client_main_pntr)(Conn *,void *);
    Conn *c;
    void *server_client_main_args;
};
typedef struct _Server_thread_args _Server_thread_args;


void *incoming_conn_thread_target(void *server_thread_args_void){
    _Server_thread_args *server_thread_args = (_Server_thread_args *)server_thread_args_void;

    Conn *c = server_thread_args->c;

    // calls server_client_main, passed by user of network library
    (*(server_thread_args->server_client_main_pntr))(c, server_thread_args->server_client_main_args);
    
    puts("disconnecting socket");
    disconnect_conn(c);
    free(server_thread_args);
    return NULL;
}

/////////////////////////////////////////
// server connection establishing code //
/////////////////////////////////////////
int await_connections(Conn_pool *conn_pool, void (*server_client_main_pntr)(Conn *, void *), void *server_client_main_args){
    Conn *lc = get_free_conn(conn_pool);

    if(!listen_conn(lc,LISTEN_PORT) ){
        puts("Failed to set up server listener");
        return 0;
    }

    Conn *client;
    _Server_thread_args *server_thread_args;

    while (1){
        client = accept_conn(lc, conn_pool);
        puts("Connection accepted");

        // arguments to pass to thread
        server_thread_args = malloc(sizeof(_Server_thread_args));
        server_thread_args->c = client;
        server_thread_args->server_client_main_pntr = server_client_main_pntr;
        server_thread_args->server_client_main_args = server_client_main_args;

        if( pthread_create(&client->thread_id , NULL , incoming_conn_thread_target , server_thread_args) < 0 ){
			perror("could not create thread");
			return 1;
		}
        puts("Handler assigned");

        puts("socket fds:");
        for(int i = 0; i <CONN_POOL_SIZE; i++){
            printf("%i ",conn_pool->pool[i].sock_fd);
        }
        puts("");
    }
    return 0;
}

