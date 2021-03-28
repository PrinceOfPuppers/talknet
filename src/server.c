#include "server.h"

#include <pthread.h>
#include <stdio.h>
#include <string.h>



void *incoming_conn_listener_thread(void *conn_void){
    Conn *c = (Conn *)conn_void;
    puts("here");
    while(sock_to_in_buffer(c)){

        // TODO: deal with conjoined packets
        pthread_mutex_lock(&stdout_mutex);
        printf("inbound: %s\n",c->in_buffer);
        pthread_mutex_unlock(&stdout_mutex);

    }
    puts("disconnecting socket");
    disconnect(c);
    return NULL;
}

int await_connections(Conn *listen_conn, Conn_pool *conn_pool){
	
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

    while (1){
        client = get_free_conn(conn_pool);
        client->sock_fd = accept(listen_conn->sock_fd, (struct sockaddr *)&client->net_info, &sockaddr_in_len);
        puts("Connection accepted");
        
        // TODO: handshake

        client->out_buffer = "Hello Client , I have received your connection. And now I will assign a handler for you\n";
        out_buffer_to_sock(client,strlen(client->out_buffer));

        if( pthread_create(&client->thread_id , NULL , incoming_conn_listener_thread , (void *)client) < 0 ){
			perror("could not create thread");
			return 1;
		}
        puts("Handler assigned");
    }
}

