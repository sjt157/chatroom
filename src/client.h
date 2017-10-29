#ifndef CLIENT_H

#include <pthread.h>

void client_send_msg(char *msg);
void *client_main(void *arg);

extern pthread_mutex_t client_request;
extern pthread_mutex_t client_done;

#endif

