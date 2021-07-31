#pragma once

#include <openssl/rsa.h>

// arguments passed to the session between client and server, on the servers side
struct Server_client_args{
    RSA *keypair;
};
typedef struct Server_client_args Server_client_args;


// arguments passed to the session between client and server, on the clients side
struct Client_server_args{
    RSA *keypair;
};
typedef struct Client_server_args Client_server_args;


//presentation layer message headers

#define HEADER_SIZE 2

#define HEAD_ONETIME_HANDSHAKE_INIT   "0"
#define HEAD_EVERYTIME_HANDSHAKE_INIT "1"
#define HEAD_PUB_KEY                  "2"
#define HEAD_LOCAL_ID                 "3"
#define HEAD_ONETIME_CODE             "4"
#define HEAD_USERDATA                 "5"
#define HEAD_AES_KEY                  "6"
#define HEAD_MESSAGE                  "7"
// to add, leasure updates 

