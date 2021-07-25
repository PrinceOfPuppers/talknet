
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "server.h"
#include "client.h"
#include "helpers.h"
#include "rsa.h"
#include "aes.h"
#include "main.h"

#define SERVER_DIR "build/servers"
#define USER_DIR "build/user"

//int init_server(char *server_name){
//    if(!make_directory(SERVER_DIR, server_name)){
//        printf("Server %s Already Exists",server_name);
//        return 0;
//    }
//
//    return 1;
//}
//
//// one time initalization for a user
//int init_user(char *username,char *username_color){
//    if(!make_directory(SERVER_DIR, username)){
//        printf("User %s Already Exists",username);
//        return 0;
//    }
//
//    // save file with username, username_color
//
//    // generate public/private key
//
//    // save ip?
//
//    return 1;
//}



// runtime of server client connection from perspective of server
void server_client_main(Conn *c, void *server_client_args){
    while(sock_to_in_buffer(c,0)){

        // TODO: deal with conjoined packets
        pthread_mutex_lock(&stdout_mutex);
        printf("inbound: %s\n",c->in_buffer);
        pthread_mutex_unlock(&stdout_mutex);

    }
}

// runtime of server client connection from perspective of client
void client_server_main(Conn *c, void *client_server_main_args){

    send_message(c,"this is a very lengthy test to see what happens for messages larger than 255 characters, will there be an overflow, who the heck knows but by golly i am going to find out once i am done rambling to fill space in this ungodly long string that i am currently typing out. ");
    send_message(c,"test");
    
    while(sock_to_in_buffer(c,0)){

        // TODO: deal with conjoined packets
        pthread_mutex_lock(&stdout_mutex);
        printf("inbound: %s\n",c->in_buffer);
        pthread_mutex_unlock(&stdout_mutex);

    }

}

int main(int argc , char *argv[])
{   
    RSA *keypair = get_keypair("./build");
//
    //char *public_key = get_public_key_str(keypair);
    //RSA *test_rsa = get_public_key_rsa(public_key);
    //public_key = get_public_key_str(test_rsa);
//
    //printf("%s",public_key);
    //free(public_key);
    //exit(0);
    if (argc == 1){
        puts(">> A number is required, 0 for server 1 for client <<");
        exit(1);
    }

    Conn_pool conn_pool;
    init_conn_pool(&conn_pool);

    switch(atoi(argv[1])){
        case(0):{ // server

            Server_client_args server_client_args;
            server_client_args.keypair = keypair;
            await_connections(&conn_pool, server_client_main,&server_client_args);

            break;
        }
        case(1):{ // client

            Client_server_args client_server_args;
            client_server_args.keypair = keypair;
            pthread_t listener_thread_id = connect_to_peer(&conn_pool, "127.0.0.1", LISTEN_PORT, client_server_main, &client_server_args);
            
            if(listener_thread_id != -1){
                pthread_join(listener_thread_id, NULL);
            }
            break;
        }
    }
    
    free_conn_pool(&conn_pool);
    exit(0);
    return 0;
}