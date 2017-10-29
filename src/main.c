#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <iup.h>

#include "client.h"
#include "server.h"
#include "socket.h"
#include "types.h"

extern void setup_view(void);

pthread_t server_tid,client_tid;
pthread_mutex_t client_exit=PTHREAD_MUTEX_INITIALIZER;
msg_t message;

static online_t online;

static void init_client_server(void)
{
	#if defined(__WIN32__)
		WSADATA wsa;
		WSAStartup(MAKEWORD(2,2),&wsa);
	#endif
	pthread_mutex_lock(&client_request);
	pthread_mutex_lock(&client_exit);
	pthread_mutex_lock(&server_done);
	
	pthread_create(&client_tid,NULL,client_main,NULL);
	pthread_create(&server_tid,NULL,server_main,NULL);
	
	online_next=NULL;
	return;	
} 

static void update_ml_online(void)
{
	Ihandle *ml=IupGetHandle("ml_online");
	IupSetAttribute(ml,"VALUE","");
	online_t *p=online.next;
	
	while(p)
	{
		IupSetAttribute(ml,"APPEND",p->buf);
		p=p->next;
	}
	return;
}

static void update_offline(char *str)
{
	online_t *p=online.next;
	online_t *prev=&online;
	
	while(p)
	{
		if(strcmp(p->buf,str)==0)
		{
			prev->next=p->next;
			free(p);
			p=prev->next; 
		}
		else
		{
			prev=p;
			p=p->next;
		}
	}
	update_ml_online();
	
	return;
	
}

/**
 * @brief Called when other client send MSG_TYPE_NOTIFY message,
 *        and update the online list.
 *
 * @param [in] str String dedicates who has offlined in following format:
 *                 "hostname (ip address)".
 */
static void update_online(char *str)
{
    int isset = 0;
    online_t *p = online.next;

    while (p) {
        if (strcmp(p->buf, str) == 0) {
            isset = 1;
            break;
        }
        p = p->next;
    }
    if (!isset) {
        p = malloc(sizeof(online_t));
        strcpy(p->buf, str);
        p->next = online.next;
        online.next = p;
        update_ml_online();
    }
    p->time = 10;

    return;
}

/**
 * @brief Called by server thread when there is message recevice from client.
 */
void transform_msg(msg_t *p)
{
    message = *p;
    return;
}

/**
 * @brief Callback function for Timer. When called, first check is there
 *        new message sended from server. Then update UI.
 */
static void timer_msg_cb(void)
{
    static msg_t last_sender;
    char buf[1024];

    if (pthread_mutex_trylock(&server_done) == 0) {
        if (message.type == MSG_TYPE_NOTIFY) {
            snprintf(buf, 1024, "%s (%s)", message.name, message.ip);
            update_online(buf);
        } else if (message.type == MSG_TYPE_EXIT) {
            snprintf(buf, 1024, "%s (%s)", message.name, message.ip);
            update_offline(buf);
        } else {
            Ihandle *ml = IupGetHandle("ml_output");

            /*
             * if the message is sended by the same client,
             * don't show the client info again.
             */
            if (!(strcmp(last_sender.name, message.name) == 0
                && strcmp(last_sender.ip, message.ip) == 0))
            {
                last_sender = message;
                IupSetAttribute(ml, "APPEND", "");
                snprintf(buf, 1024, "- %s (%s) says:\n%s",
                    message.name, message.ip, message.buf);
            } else {
                snprintf(buf, 1024, "%s", message.buf);
            }

            IupSetAttribute(ml, "APPEND", buf);
            snprintf(buf, 32, "%d", IupGetInt(ml, "COUNT")+1);
            IupSetAttribute(ml, "SCROLLTO", buf);
        }
        pthread_mutex_unlock(&server_request);
    }
    return;
}

static void timer_online_cb(void)
{
    int isupdate = 0;
    online_t *p = online.next;
    online_t *prev = &online;

    while (p) {
        --p->time;
        if (p->time == 0) {
            prev->next = p->next;
            free(p);
            p = prev->next;
            isupdate = 1;
        } else {
            prev = p;
            p = p->next;
        }
    }

    if (isupdate) {
        update_ml_online();
    }

    return;
}

static void timer_btn_cb(Ihandle *timer)
{
    Ihandle *btn = IupGetHandle("btn_send");

    IupSetAttribute(btn, "ACTIVE", "YES");
    IupSetAttribute(timer, "RUN", "NO");

    return;
}

int main(int argc, char *argv[])
{
    init_client_server();

    IupOpen(&argc, &argv);

    setup_view();

    Ihandle *timer_msg = IupTimer();
    IupSetAttributes(timer_msg, "TIME = 1, RUN = YES");
    IupSetCallback(timer_msg, "ACTION_CB", (Icallback) timer_msg_cb);

    Ihandle *timer_online = IupTimer();
    IupSetAttributes(timer_online, "TIME = 1000, RUN = YES");
    IupSetCallback(timer_online, "ACTION_CB", (Icallback) timer_online_cb);

    Ihandle *timer_btn = IupTimer();
    IupSetAttributes(timer_btn, "TIME = 2000, RUN = NO");
    IupSetCallback(timer_btn, "ACTION_CB", (Icallback) timer_btn_cb);
    IupSetHandle("timer_btn", timer_btn);

    IupMainLoop();

    /* Clean up */
    IupDestroy(IupGetHandle("dlg"));
    IupDestroy(timer_msg);
    IupDestroy(timer_online);
    IupDestroy(timer_btn);

    IupClose();

    pthread_mutex_unlock(&client_exit);
    pthread_mutex_unlock(&client_request); /* send MSG_TYPE_EXIT message */
    pthread_join(client_tid, NULL);

#if defined(__WIN32__)
    WSACleanup();
#endif
    return 0;
}
