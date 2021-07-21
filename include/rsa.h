#pragma once

#define KEYSIZE 4096 // in bits

#define PRIVATE_KEY_NAME "private.pem"
#define PUBLIC_KEY_NAME "private.pem"
#include <openssl/rsa.h>


RSA *get_keypair(char* path);