#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "server.h"
#include "socket.h"
#include "types.h"
extern void transform_msg(msg_t *p);

pthread_mutex_t server_request=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t server_done=PTHREAD_MUTEX_INITIALIZER;

void *server_main(void *arg)
{
	struct sockaddr_in my_addr,client_addr;
	socklen_t client_addr_len;
	int socket_fd;
	msg_t msg;
	char buf[1024];
	char str[INET_ADDRSTRLEN];
	int i,n;
	
	socket_fd=socket_open(AF_INET,SOCK_DGRAM,0);
	memset(&my_addr,0,sizeof(my_addr));
	my_addr.sin_family=AF_INET;
	my_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	my_addr.sin_port=htons(SERVER_PORT);
	
	socket_bind(socket_fd,(struct sockaddr*)&my_addr,sizeof(my_addr));
	
	while(1)
	{
		client_addr_len=sizeof(client_addr);
		n=socket_recvfrom(socket_fd,&msg,sizeof(msg),0,(struct sockaddr*)&client_addr,&client_addr_len);
		inet_ntop(AF_INET,&client_addr.sin_addr,msg.ip,sizeof(msg.ip));
		pthread_mutex_lock(&server_request);
		transform_msg(&msg);
		pthread_mutex_unlock(&server_done);
		
		
	 } 
	 return NULL;
}
