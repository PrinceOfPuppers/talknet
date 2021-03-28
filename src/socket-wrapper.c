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

int send_message(Conn *c, char *message){
	//Send some data
    uint16_t len = (uint16_t)strlen(message);

    if (len > (OUT_BUFF_SIZE - HEADER_SIZE)){
       	puts("Message too large to send");
		return 0; 
    }

    // write len to first 2 bytes
    c->out_buffer[1] = (len >> 8) & 0xFF;
    c->out_buffer[0] = len & 0xFF;


    
    strncpy(c->out_buffer+2,message,len);

	if( send(c->sock_fd, c->out_buffer , len + 2, 0) < 0)
	{
		puts("Send failed");
		return 0;
	}

	puts("Data Send\n");
    return 1;
}

void flush_socket_buffer(int fd, char *write_buffer, int buff_size){
    // must clear tcp buffer so next message can be read properly 
    while(recv(fd, write_buffer , buff_size, MSG_DONTWAIT) > 0){}
    write_buffer[0] = '\0';
}

int sock_to_in_buffer(Conn *c){

    ssize_t recv_size = recv(c->sock_fd, c->in_buffer , HEADER_SIZE, 0);

    if(recv_size == 0){
        puts("peer closed connection");
        return 0;
    }

    // derefrence first 2 bytes into u16
    uint16_t len = *(uint16_t *)c->in_buffer;


    if(recv_size != HEADER_SIZE || len > IN_BUFF_SIZE -1){
        puts("Unable to Recieve Header or Message Size is Too Large");
        flush_socket_buffer(c->sock_fd, c->in_buffer, IN_BUFF_SIZE);
        return 1;
    }

    recv_size = recv(c->sock_fd, c->in_buffer, len, MSG_DONTWAIT);

	if( recv_size < 0)
	{
		puts("recv failed");
        flush_socket_buffer(c->sock_fd, c->in_buffer, IN_BUFF_SIZE);
        return 1;
	}

    // null terminate in buffer
    c->in_buffer[recv_size+1] = '\0';
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