#pragma once

#define KEYSIZE 2048 // in bits

#define PRIVATE_KEY_NAME "private.pem"
#define PUBLIC_KEY_NAME "public.pem"
#include <openssl/rsa.h>


RSA *get_keypair(char* path);

RSA *get_public_key_rsa(char *public_key);
char *get_public_key_str(RSA *rsa);

#define public_decrypt(rsa,src,dest) RSA_public_decrypt(RSA_size(rsa), src, dest, rsa, RSA_PKCS1_PADDING)
#define public_encrypt(rsa,src,dest) RSA_public_encrypt(strlen(src) + 1, src, dest, rsa, RSA_PKCS1_PADDING)

#define private_decrypt(rsa,src,dest) RSA_private_decrypt(RSA_size(rsa), src, dest, rsa, RSA_PKCS1_PADDING)
#define private_encrypt(rsa,src,dest) RSA_private_encrypt(strlen(src) + 1, src, dest, rsa, RSA_PKCS1_PADDING)
