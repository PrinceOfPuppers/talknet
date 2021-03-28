#include "server.h"
#include "client.h"
#include <stdio.h>
#include <stdlib.h>

    
int main(int argc , char *argv[])
{
    //pthread_mutex_init(&stdout_mutex,NULL);

    Conn_pool conn_pool;
    init_conn_pool(&conn_pool);

    switch(atoi(argv[1])){
        case(0):{ // server
            Conn listen_conn;
            init_conn(&listen_conn);
    
            await_connections(&listen_conn,&conn_pool);

            free_conn_buffers(&listen_conn);
            break;
        }
        case(1):{ // client
            pthread_t listener_thread_id = connect_to_peer(&conn_pool,"127.0.0.1",LISTEN_PORT);

            pthread_join(listener_thread_id, NULL);

            break;
        }
    }
    free_conn_pool(&conn_pool);

}