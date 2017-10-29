#ifndef TYPES_H
#define TYPES_H
 
 #include"socket.h"
 
 #define SERVER_PORT 7201
 
 #define MAXNAME 16
 #define MAXLINE 128
 
 typedef enum msg_type{
 	MSG_TYPE_NOTIFY=0,
 	MSG_TYPE_CHAT,
 	MSG_TYPE_EXIT,
 }msg_type_t;
 
 typedef struct message{
 	msg_type_t type;
 	char name[MAXNAME];
 	char buf[MAXLINE];
 	char ip[INET_ADDRSTRLEN];
 }msg_t;
 
 typedef struct online{
 	char buf[MAXLINE+INET_ADDRSTRLEN+8];
 	int time;
 	struct online_t *next;
 }online_t;


#endif 
