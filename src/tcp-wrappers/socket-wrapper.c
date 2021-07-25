#include "socket-wrapper.h"

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>	//inet_addr
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t stdout_mutex = PTHREAD_MUTEX_INITIALIZER;

socklen_t SOCKADDER_IN_LEN = sizeof(struct sockaddr_in);

// simply initalizes conn with buffers
void init_conn(Conn *c){
    // sock_fd is also used as an indicator if the connection is live and a thread is running
    c->sock_fd = SOCK_FD_FREE;
    c->thread_id = 0;
    
    c->in_buffer = malloc(sizeof(char)*IN_BUFF_SIZE);
    c->in_buffer[IN_BUFF_SIZE-1] = '\0';
    c->out_buffer = malloc(sizeof(char)*OUT_BUFF_SIZE);
    c->in_buffer[OUT_BUFF_SIZE-1] = '\0';

    pthread_mutex_init(&c->disconnect_conn_mutex,NULL);
}

int connect_conn(Conn *c, char *ip, int port){

    c->sock_fd = socket(AF_INET , SOCK_STREAM , 0);

	if (c->sock_fd == SOCK_FD_ERR){
		puts("Could not create socket");
        return 0;
	}

    c->net_info.sin_addr.s_addr = inet_addr(ip);
	c->net_info.sin_family = AF_INET;
	c->net_info.sin_port = htons( port );

	//Connect to remote server
	if (connect(c->sock_fd , (struct sockaddr *)&c->net_info , sizeof(c->net_info)) < 0){
        puts("Unable to connect to server");
        c->sock_fd = SOCK_FD_FREE;
		return 0;
	}
    return 1;
}

// disconnectes socket if its connected, frees it if its reserved
void disconnect_conn(Conn *c){
    pthread_mutex_lock(&c->disconnect_conn_mutex);
    if ( (c->sock_fd != SOCK_FD_FREE) & (c->sock_fd != SOCK_FD_RESERVED) ){
        // TODO: send EOF
        close(c->sock_fd);
    }
    c->sock_fd = SOCK_FD_FREE;
    pthread_mutex_unlock(&c->disconnect_conn_mutex);
}

void free_conn_buffers(Conn *c){
    disconnect_conn(c);
    
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

int sock_to_in_buffer(Conn *c, int blocking){

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

    if(blocking){
        recv_size = recv(c->sock_fd, c->in_buffer, len, MSG_WAITFORONE);
    }else{
        recv_size = recv(c->sock_fd, c->in_buffer, len, MSG_DONTWAIT);
    }
    

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
        if (conn_pool->pool[i].sock_fd == SOCK_FD_FREE){

            // indicates socket is disconnected but reserved
            conn_pool->pool[i].sock_fd = SOCK_FD_RESERVED;

            pthread_mutex_unlock(&conn_pool->mutex);
            return &conn_pool->pool[i];
        }
    }

    pthread_mutex_unlock(&conn_pool->mutex);
    return NULL;
}




///////////////
// listeners //
///////////////

// setup conn as a listener
int listen_conn(Conn *lc, int port){

    lc->sock_fd = socket(AF_INET , SOCK_STREAM , 0);

	if (lc->sock_fd == SOCK_FD_ERR){
		printf("Could not create socket");
        return 0;
	}

    //Prepare the sockaddr_in structure
    lc->net_info.sin_family = AF_INET;
    lc->net_info.sin_addr.s_addr = INADDR_ANY;
    lc->net_info.sin_port = htons( port );

    //Bind
	if( bind(lc->sock_fd, (struct sockaddr *)&lc->net_info , sizeof(lc->net_info)) < 0){
		puts("bind failed");
		return 0;
	}
	puts("bind done");

    if (listen(lc->sock_fd,AWAIT_CONN_QUEUE_LEN)){
        puts("listen failed");
    }

    return 1;
}

// awaits a new connection on listener, blocking, returns established conn pntr
Conn *accept_conn(Conn *lc, Conn_pool *conn_pool){
    
    Conn *client = get_free_conn(conn_pool);

    while(1){
        client->sock_fd = accept(lc->sock_fd, (struct sockaddr *)&client->net_info, &SOCKADDER_IN_LEN);
        if(client->sock_fd == SOCK_FD_ERR){
            puts("Connection failed");
            disconnect_conn(client);
            continue;
        }
        break;
    }

    return client;
}