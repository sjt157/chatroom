#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"client.h"
#include"socket.h"
#include"types.h"

extern pthread_mutex_t client_exit;
pthread_mutex_t client_request=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t client_done=PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t subthread_exit=PTHREAD_MUTEX_INITIALIZER;
static msg_t msg;
static struct sockaddr_in server_addr;
static int socket_fd;

static void *client_send_notify(void *arg)
{
	msg_t notify;
	notify.type=MSG_TYPE_NOTIFY;
	strcpy(notify.name,msg.name); 
	while(pthread_mutex_trylock(&subthread_exit))
	{
		socket_sendto(socket_fd,&notify,sizeof(notify.type)+strlen(notify.name)+1,0,(struct sockaddr*)&server_addr,sizeof(server_addr));
		#if defined(__WIN32__)
			sleep(2000);
		#else
			sleep(2);
		#endif 
	}
	notify.type=MSG_TYPE_EXIT;
	socket_sendto(socket_fd,&notify,sizeof(notify.type)+strlen(notify.name)+1,0,(struct sockaddr*)&server_addr,sizeof(server_addr));
	return NULL;
}

void client_send_msg(char *s)
{
	strncpy(msg.buf,s,MAXLINE);
	return;
}

void *client_main(void *arg)
{
	socklen_t server_addr_len;
	pthread_t tid;
	#if defined(__WIN32__)
			gethostname(msg.name,sizeof(msg.name));
	#else
			strncpy(msg.name,getlogin(),sizeof(msg.name));
	#endif 
	socket_fd=socket_open(AF_INET,SOCK_DGRAM,0);
	
	int opt=1;
	setsockopt(socket_fd,SOL_SOCKET,SO_BROADCAST,&opt,sizeof(opt));
	memset(&server_addr,0,sizeof(server_addr));
	server_addr.sin_family=AF_INET;
	server_addr.sin_addr.s_addr=htonl(INADDR_BROADCAST);
	server_addr.sin_port=htons(SERVER_PORT);
	
	pthread_mutex_lock(&subthread_exit);
	ptread_create(&tid,NULL,client_send_notify,NULL);
	
	while(1)
	{
		pthread_mutex_lock(&client_request);
		if(pthread_mutex_trylock(&client_exit)==0)
		{
			break;
		}
		/*broadcast message to server*/
		msg.type=MSG_TYPE_CHAT;
		sock_sendto(socket_fd,&msg,sizeof(msg.type)+sizeof(msg.name)+strlen(msg.buf)+1,0,(struct sockaddr*)&server_addr,sizeof(server_addr));
		
		pthread_mutex_unlock(&client_done);
	}
	
	pthread_mutex_unlock(&subthread_exit);
	pthread_join(tid,NULL);
	
	return NULL;
	
	
}
