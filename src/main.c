#include "server.h"
#include "client.h"
#include <stdio.h>
#include <stdlib.h>


#include "helpers.h"
#include "rsa.h"
#include "aes.h"


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




int main(int argc , char *argv[])
{   
    RSA *keypair = get_keypair("./build");

    test();

    exit(0);
    //Conn_pool conn_pool;
    //init_conn_pool(&conn_pool);
//
    //switch(atoi(argv[1])){
    //    case(0):{ // server
    //        Conn listen_conn;
    //        init_conn(&listen_conn);
    //
    //        await_connections(&listen_conn,&conn_pool);
//
    //        free_conn_buffers(&listen_conn);
    //        break;
    //    }
    //    case(1):{ // client
    //        pthread_t listener_thread_id = connect_to_peer(&conn_pool,"127.0.0.1",LISTEN_PORT);
//
    //        pthread_join(listener_thread_id, NULL);
//
    //        break;
    //    }
    //}
    //free_conn_pool(&conn_pool);

}