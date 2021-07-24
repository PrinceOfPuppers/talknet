
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    printf("in server_client_main\n");
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
    printf("test\n");

    switch(atoi(argv[1])){
        case(0):{ // server

            Server_client_args server_client_args;
            server_client_args.keypair = keypair;
            start_server(server_client_main,&server_client_args);

            break;
        }
        case(1):{ // client
            Conn_pool conn_pool;
            init_conn_pool(&conn_pool);
            pthread_t listener_thread_id = connect_to_peer(&conn_pool,"127.0.0.1",LISTEN_PORT);

            pthread_join(listener_thread_id, NULL);
            free_conn_pool(&conn_pool);
            break;
        }
    }
    
    return 0;
}