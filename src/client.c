#include "client.h"

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

void *outgoing_conn_listener_thread(void *conn_void){
    Conn *c = (Conn *)conn_void;
    c->out_buffer = "test";
    out_buffer_to_sock(c,strlen(c->out_buffer));

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

pthread_t connect_to_peer(Conn_pool *conn_pool, char *ip, int port){
	Conn *c = get_free_conn(conn_pool);

	c->sock_fd = socket(AF_INET , SOCK_STREAM , 0);

	if (c->sock_fd == -1){
		printf("Could not create socket");
        return -1;
	}

	c->net_info.sin_addr.s_addr = inet_addr(ip);
	c->net_info.sin_family = AF_INET;
	c->net_info.sin_port = htons( port );

	//Connect to remote server
	if (connect(c->sock_fd , (struct sockaddr *)&c->net_info , sizeof(c->net_info)) < 0){
		puts("connect error");
		return -1;
	}

	puts("Connected\n");

    // TODO: handshake

    if( pthread_create(&c->thread_id , NULL , outgoing_conn_listener_thread , (void *)c) < 0 ){
		perror("could not create thread");
		return -1;
	}
    puts("Handler assigned");
    return c->thread_id;
}