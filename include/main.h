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