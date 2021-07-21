#include "rsa.h"
#include <sys/random.h>
#include <sys/stat.h>
#include <errno.h>

#include <openssl/pem.h>
#include <openssl/sha.h>
#include <openssl/rsa.h>
#include <openssl/rand.h>

#include <unistd.h>
#include <string.h>

#include "helpers.h"

int gen_keypair(char *publicPath, char *privatePath){

    int success = 1;

    RSA *r = RSA_new();

    BIGNUM *e;
    e = BN_new();
    
	BIO *public_fp = NULL;
    BIO *private_fp = NULL;


    if(RAND_load_file("/dev/random", 32) != 32) {
        success = 0;
        goto cleanup;
    }


    if(!RSA_generate_key_ex(r,KEYSIZE,e,NULL)){
        success = 0;
        goto cleanup;
    }
    
    public_fp = BIO_new_file(publicPath, "w+");
    if(!PEM_write_bio_RSAPublicKey(public_fp, r)){
        success = 0;
        goto cleanup;
    }


    private_fp = BIO_new_file(privatePath, "w+");
    if(!PEM_write_bio_RSAPrivateKey(private_fp, r, NULL, NULL, 0, NULL, NULL)){
        success = 0;
        goto cleanup;
    }

    cleanup:
	    BIO_free_all(public_fp);
	    BIO_free_all(private_fp);
	    RSA_free(r);
        BN_free(e);
        CRYPTO_cleanup_all_ex_data();


    return success;
}

RSA *get_keypair(char *path){
    
    char *publicPath  = malloc( sizeof(char)*(strlen(path) + 1 + strlen(PUBLIC_KEY_NAME)  + 1) );
    char *privatePath = malloc( sizeof(char)*(strlen(path) + 1 + strlen(PRIVATE_KEY_NAME) + 1) );

    sprintf(publicPath, "%s/%s",path,PUBLIC_KEY_NAME);
    sprintf(privatePath,"%s/%s",path,PRIVATE_KEY_NAME);

    if( !(access( publicPath, F_OK ) == 0) | !(access( privatePath, F_OK ) == 0) ) {

        int status = mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

        if( (status != 0) | (errno != EEXIST) ){
            //TODO: do somthing about fail case
        }

        //TODO: do somthing about fail case
        gen_keypair(publicPath,privatePath);

    }

    FILE *public_FP = fopen(publicPath,"rt");
    RSA *rsa = PEM_read_RSAPublicKey(public_FP,NULL,NULL,NULL);


    FILE *private_FP = fopen(privatePath,"rt");
    rsa = PEM_read_RSAPrivateKey(private_FP,rsa,NULL,NULL);


    close(public_FP);
    close(private_FP);
    
    free(privatePath);
    free(publicPath);
    return rsa;
}

#define rsa_public_decrypt(rsa,src,dest) RSA_public_decrypt(RSA_size(rsa), src, dest, rsa, RSA_PKCS1_OAEP_PADDING)
#define rsa_public_encrypt(rsa,src,dest) RSA_public_encrypt(RSA_size(rsa), src, dest, rsa, RSA_PKCS1_OAEP_PADDING)

#define rsa_private_decrypt(rsa,src,dest) RSA_private_decrypt(RSA_size(rsa), src, dest, rsa, RSA_PKCS1_OAEP_PADDING)
#define rsa_private_encrypt(rsa,src,dest) RSA_private_encrypt(RSA_size(rsa), src, dest, rsa, RSA_PKCS1_OAEP_PADDING)




