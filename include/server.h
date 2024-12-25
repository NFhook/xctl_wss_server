#ifndef _SERVER_H_
#define _SERVER_H_




#include <libwebsockets.h>
#include <yyjson.h>

#include <signal.h>


#define KEEPALIVE_INTERVAL_SEC 10


// callback function.
int callback_keepalive(struct lws *wsi, enum lws_callback_reasons reason,
                        void *user, void *in, size_t len);

#endif
