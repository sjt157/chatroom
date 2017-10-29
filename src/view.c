#include <stdio.h>
#include <string.h>

#include<iup.h>

#include "string_value.h"
#include "client.h"
#include "server.h"


static void btn_send_action(Ihandle *btn)
{
    Ihandle *ml = IupGetHandle("ml_input");
    char *msg = IupGetAttribute(ml, "VALUE");
    char *p;
    msg = msg + strspn(msg, " ");   /* trim the begining spaces */
    if (msg && *msg && IupGetInt(btn, "ACTIVE")) {
        pthread_mutex_lock(&client_done);
        client_send_msg(msg);
        IupSetAttribute(ml, "VALUE", "");
        pthread_mutex_unlock(&client_request);

        IupSetAttribute(btn, "ACTIVE", "NO");
        IupSetAttribute(IupGetHandle("timer_btn"), "RUN", "YES");
    }

    return;
}

static int ml_input_action(Ihandle *self, int c, char *after)
{
    if (c == '\n') {
        btn_send_action(IupGetHandle("btn_send"));
        return IUP_IGNORE;
    } else {
        return c;
    }
}

void setup_view(void)
{
    Ihandle *dlg;
    Ihandle *frame_input, *frame_output, *frame_online;
    Ihandle *ml_input, *ml_output, *ml_online;
    Ihandle *btn_send;

    ml_output = IupMultiLine(NULL);
    IupSetAttributes(ml_output,
        "EXPAND = NO,"
        "WORDWRAP = YES,"
        "SIZE = 360x180,"
        "FORMATTING = YES,"
        "AUTOHIDE = YES,"
        "TABSIZE = 4,"
        "READONLY = YES,"
    );
    IupSetHandle("ml_output", ml_output);

    frame_output = IupFrame(ml_output);
    IupSetAttribute(frame_output, "TITLE", frame_output_title);

    ml_input = IupMultiLine(NULL);
    IupSetAttributes(ml_input,
            "EXPAND = NO,"
            "WORDWRAP = YES,"
            "AUTOHIDE = YES,"
            "SIZE = 320x40,"
            "FORMATTING = YES,"
            "TABSIZE = 4,"
    );
    IupSetCallback(ml_input, "ACTION", (Icallback) ml_input_action);
    IupSetHandle("ml_input", ml_input);

    btn_send = IupButton(btn_send_title, NULL);
    IupSetAttribute(btn_send, "EXPAND", "NO");
    IupSetAttribute(btn_send, "SIZE", "40x40");
    IupSetCallback(btn_send, "ACTION", (Icallback) btn_send_action);
    IupSetHandle("btn_send", btn_send);

    frame_input = IupFrame(
        IupHbox(ml_input, btn_send, NULL)
    );
    IupSetAttribute(frame_input, "TITLE", frame_input_title);

    ml_online = IupMultiLine(NULL);
    IupSetAttribute(ml_online, "EXPAND", "YES");
    IupSetAttribute(ml_online, "SCROLLBAR", "VERTICAL");
    IupSetAttribute(ml_online, "VISIBLECOLUMNS", "12");
    IupSetAttribute(ml_online, "SIZE", "128x");
    IupSetAttribute(ml_online, "AUTOHIDE", "YES");
    IupSetAttribute(ml_online, "FORMATTING", "YES");
    IupSetAttribute(ml_online, "READONLY", "YES");
    IupSetHandle("ml_online", ml_online);

    frame_online = IupFrame(ml_online);
    IupSetAttribute(frame_online, "TITLE", frame_online_title);

    dlg = IupDialog(IupHbox(
        IupVbox(frame_output, frame_input, NULL),
        frame_online,
        NULL
    ));
    IupSetAttribute(dlg, "TITLE", dlg_title);
    IupSetAttribute(dlg, "RESIZE", "NO");
    IupSetHandle("dlg", dlg);

    IupShow(dlg);

    return;
}

