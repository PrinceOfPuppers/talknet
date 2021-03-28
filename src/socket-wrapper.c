#include "socket-wrapper.h"

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>	//inet_addr
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t stdout_mutex = PTHREAD_MUTEX_INITIALIZER;

// simply initalizes conn with buffers
void init_conn(Conn *c){
    // sock_fd is also used as an indicator if the connection is live and a thread is running
    c->sock_fd = -1;
    c->thread_id = 0;
    
    c->in_buffer = malloc(sizeof(char)*IN_BUFF_SIZE);
    c->out_buffer = malloc(sizeof(char)*OUT_BUFF_SIZE);

    pthread_mutex_init(&c->disconnect_conn_mutex,NULL);

}

void disconnect(Conn *c){
    pthread_mutex_lock(&c->disconnect_conn_mutex);
    if (c->sock_fd != -1){
        // TODO: send EOF
        pthread_cancel(c->thread_id); // close listener thread

        close(c->sock_fd);
        c->sock_fd = -1;
        c->thread_id = 0;
    }
    pthread_mutex_unlock(&c->disconnect_conn_mutex);
}

void free_conn_buffers(Conn *c){
    disconnect(c);
    
    free(c->in_buffer);
    c->in_buffer = NULL;
    
    free(c->out_buffer);
    c->out_buffer = NULL;

    // conn pool stack allocates hence free should not be called on conn
    // free(c);
}

int out_buffer_to_sock(Conn *c,int len){
	//Send some data
	if( send(c->sock_fd, c->out_buffer , len , 0) < 0)
	{
		puts("Send failed");
		return 0;
	}
	puts("Data Send\n");
    
    return 1;
}



int sock_to_in_buffer(Conn *c){
    //recieve a maximum of IN_BUFF_SIZE - 1 bytes
    ssize_t recv_size = recv(c->sock_fd, c->in_buffer , IN_BUFF_SIZE - 1, 0);

	if( recv_size < 0)
	{
		puts("recv failed");
        // null terminate in buffer
        c->in_buffer[0] = '\0';
        return 1;

	}else if(recv_size == 0){
        puts("peer closed connection");
        return 0;
    }

    // null terminate in buffer
    c->in_buffer[recv_size] = '\0';

	puts("Reply received\n");
    return 1;
}




/////////////////////////
//    conn pooling     //
/////////////////////////

void init_conn_pool(Conn_pool *conn_pool){
    pthread_mutex_init(&conn_pool->mutex,NULL);
    for (int i = 0; i < CONN_POOL_SIZE; i++){
        init_conn(&conn_pool->pool[i]);
    }
}

void free_conn_pool(Conn_pool *conn_pool){
    for (int i = 0; i < CONN_POOL_SIZE; i++){
        free_conn_buffers(&conn_pool->pool[i]);
    }

    pthread_mutex_destroy(&conn_pool->mutex);
}

Conn *get_free_conn(Conn_pool *conn_pool){

    pthread_mutex_lock(&conn_pool->mutex);
    
    for (int i = 0; i < CONN_POOL_SIZE; i++){
        if (conn_pool->pool[i].sock_fd==-1){

            pthread_mutex_unlock(&conn_pool->mutex);
            return &conn_pool->pool[i];
        }
    }

    pthread_mutex_unlock(&conn_pool->mutex);
    return NULL;
}