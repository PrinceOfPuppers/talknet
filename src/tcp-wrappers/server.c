#include "server.h"

#include <pthread.h>
#include <stdio.h>
#include <string.h>

struct Thread_args{
    void (*server_client_main_pntr)(Conn *,void *);
    Conn *c;
    void *server_client_main_args;
};
typedef struct Thread_args Thread_args;


void *incoming_conn_thread_target(void *thread_args_void){
    printf("test321\n");
    Thread_args *thread_args = (Thread_args *)thread_args_void;
    printf("test123\n");
    Conn *c = thread_args->c;

    // calls server_client_main
    (*(thread_args->server_client_main_pntr))(c, thread_args->server_client_main_args);
    
    puts("disconnecting socket");
    disconnect(c);
    free(thread_args);
    return NULL;
}

/////////////////////////////////////////
// server connection establishing code //
/////////////////////////////////////////
int _await_connections(Conn *listen_conn, Conn_pool *conn_pool, void (*server_client_main_pntr)(Conn *, void *), void *server_client_main_args){
    listen_conn->sock_fd = socket(AF_INET , SOCK_STREAM , 0);

	if (listen_conn->sock_fd == -1){
		printf("Could not create socket");
        return 1;
	}
    //Prepare the sockaddr_in structure
    listen_conn->net_info.sin_family = AF_INET;
    listen_conn->net_info.sin_addr.s_addr = INADDR_ANY;
    listen_conn->net_info.sin_port = htons( LISTEN_PORT );

    //Bind
	if( bind(listen_conn->sock_fd, (struct sockaddr *)&listen_conn->net_info , sizeof(listen_conn->net_info)) < 0){
		puts("bind failed");
		return 1;
	}
	puts("bind done");

    if (listen(listen_conn->sock_fd,AWAIT_CONN_QUEUE_LEN)){
        puts("listen failed");
    }

    socklen_t sockaddr_in_len = sizeof(struct sockaddr_in);
    Conn *client;
    Thread_args *thread_args;

    while (1){
        client = get_free_conn(conn_pool);
        client->sock_fd = accept(listen_conn->sock_fd, (struct sockaddr *)&client->net_info, &sockaddr_in_len);
        puts("Connection accepted");

        // arguments to pass to thread
        thread_args = malloc(sizeof(Thread_args));
        thread_args->c = client;
        thread_args->server_client_main_pntr = server_client_main_pntr;
        thread_args->server_client_main_args = server_client_main_args;

        if( pthread_create(&client->thread_id , NULL , incoming_conn_thread_target , thread_args) < 0 ){
			perror("could not create thread");
			return 1;
		}
        puts("Handler assigned");
    }
}



int start_server(void (*server_client_main_pntr)(Conn *, void *), void *server_client_main_args){
    Conn_pool conn_pool;
    init_conn_pool(&conn_pool);
    
    Conn listen_conn;
    init_conn(&listen_conn);

    int status = _await_connections(&listen_conn, &conn_pool, server_client_main_pntr, server_client_main_args);

    free_conn_buffers(&listen_conn);
    free_conn_pool(&conn_pool);

    return status;
}