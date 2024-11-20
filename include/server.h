#ifndef XCTL_WSS_SERVER_H
#define XCTL_WSS_SERVER_H


#include <libwebsockets.h>
#include <yyjson.h>

#define KEEPALIVE_INTERVAL_SEC 10


// callback function.
int callback_keepalive(struct lws *wsi, enum lws_callback_reasons reason,
                        void *user, void *in, size_t len);

                        

#endif