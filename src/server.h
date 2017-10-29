#ifndef SERVER_H
#define SERVER_H

#include <pthread.h>

void *server_main(void *arg);

extern pthread_mutex_t server_request;
extern pthread_mutex_t server_done;

#endif

