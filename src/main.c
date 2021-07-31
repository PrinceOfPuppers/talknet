
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

int server_onetime_handshake(Conn *c, RSA *rsa){


    if(!sock_to_in_buffer(c,0)){
        return 0;
    }

    if (c->in_buffer[0] != HEAD_PUB_KEY[0]){
        puts("no pub_key");
        return 0;   
    }

    printf("inbound: %s\n",&c->in_buffer[3]);

    //RSA *client_pub = get_public_key_rsa(&c->in_buffer[3]);
//
    //char *data = "test";
    //char *test_encrypted = malloc(RSA_size(client_pub));
    //char *test_decrypted = malloc(4+1);
    //
    ////public_encrypt(client_pub,"test",test_encrypted);
    //RSA_public_encrypt(RSA_size(client_pub), data, test_encrypted, client_pub, RSA_PKCS1_OAEP_PADDING);
    //exit(0);
    //private_decrypt(rsa,test_encrypted,test_decrypted);
    //printf("after encryption decryption: %s\n",test_decrypted);

    return 1;
}

int client_onetime_handshake(Conn *c, RSA *rsa){
    send_message(c,HEAD_ONETIME_HANDSHAKE_INIT);

    char *pubkey = get_public_key_str(rsa);
    send_messages(c,2,HEAD_PUB_KEY,pubkey);
    free(pubkey);

    return 1;
}

// runtime of server client connection from perspective of server
void server_client_main(Conn *c, void *server_client_args){
    Server_client_args *args = (Server_client_args *)server_client_args;
    
    if(!sock_to_in_buffer(c,0)){
        puts("failed to get inital packet");
        return;
    }
    if (c->in_buffer[0] != HEAD_ONETIME_HANDSHAKE_INIT[0]){
        return;
        //printf("inbound: %s\n",&c->in_buffer[1]);
    }

    server_onetime_handshake(c, args->keypair);

    //free(args);
}

// runtime of server client connection from perspective of client
void client_server_main(Conn *c, void *client_server_main_args){
    Client_server_args *args = (Client_server_args *)client_server_main_args;

    client_onetime_handshake(c, args->keypair);
    //send_message(c,"this is a very lengthy test to see what happens for messages larger than 255 characters, will there be an overflow, who the heck knows but by golly i am going to find out once i am done rambling to fill space in this ungodly long string that i am currently typing out. ");
    //send_message(c,"test");
    //
    //while(sock_to_in_buffer(c,0)){
//
    //    // TODO: deal with conjoined packets
    //    pthread_mutex_lock(&stdout_mutex);
    //    printf("inbound: %s\n",c->in_buffer);
    //    pthread_mutex_unlock(&stdout_mutex);
//
    //}
    //free(args);

}

int main(int argc , char *argv[])
{   

    RSA *keypair = get_keypair("./build");

    char *encrypted = malloc(RSA_size(keypair)*sizeof(char) + 1);
    //encrypted[255] = '\0';
//
    char * msg = "this is a test 6498463198410321ASDFASDF";
    char *decrypted = malloc(strlen(msg));
    

    RSA_private_encrypt( strlen(msg) + 1, (unsigned char *)msg, (unsigned char *)encrypted, keypair, RSA_PKCS1_PADDING );
    
    printf("%i %i\n",RSA_size(keypair),strlen(encrypted));
    RSA_public_decrypt( RSA_size(keypair), (unsigned char *)encrypted, (unsigned char *)decrypted, keypair, RSA_PKCS1_PADDING );
    printf("%s\n",decrypted);

    //free(keypair);
    free(encrypted);
    //puts("test");
    free(keypair);
    //free(msg);
    free(decrypted);
    CRYPTO_cleanup_all_ex_data();
    
    //CRYPTO_cleanup_all_ex_data();
    //if (argc == 1){
    //    puts(">> A number is required, 0 for server 1 for client <<");
    //    exit(1);
    //}
//
    //Conn_pool conn_pool;
    //init_conn_pool(&conn_pool);
//
    //switch(atoi(argv[1])){
    //    case(0):{ // server
//
    //        Server_client_args server_client_args;
    //        server_client_args.keypair = keypair;
    //        await_connections(&conn_pool, server_client_main,&server_client_args);
//
    //        break;
    //    }
    //    case(1):{ // client
//
    //        Client_server_args client_server_args;
    //        client_server_args.keypair = keypair;
    //        pthread_t listener_thread_id = connect_to_peer(&conn_pool, "127.0.0.1", LISTEN_PORT, client_server_main, &client_server_args);
    //        
    //        if(listener_thread_id != -1){
    //            pthread_join(listener_thread_id, NULL);
    //        }
    //        break;
    //    }
    //}
    //free_conn_pool(&conn_pool);
    //exit(0);
    //return 0;
    return 0;
}