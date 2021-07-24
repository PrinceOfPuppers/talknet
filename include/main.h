#pragma once

#include "server.h"
#include "client.h"
#include "helpers.h"
#include "rsa.h"
#include "aes.h"

struct Server_client_args{
    RSA *keypair;
};
typedef struct Server_client_args Server_client_args;