#include <libwebsockets.h>
#include "server.h"

static int interrupted = 0;

static void sigint_handler(int sig) {
    interrupted = 1;
}

int main(int argc, char **argv) {

    struct lws_context_creation_info info;
    struct lws_context *context;

    static struct lws_protocols protocols[] = {
        {
            "keepalive-protocol",
            callback_keepalive,
            0,
            1024,
        },
        {NULL, NULL, 0, 0}
    };

    memset(&info, 0, sizeof(info));
    info.port = 8080;
    info.protocols = protocols;

    context = lws_create_context(&info);
    if (!context) {
        lwsl_err("Failed to create lws context\n");
        return -1;
    }

    lwsl_notice("WebSocket server started on port 8080\n");

    signal(SIGINT, sigint_handler);
    
    lwsl_user("Starting server on port: %d\n", info.port);
    
    
    //while (lws_service(context, 1000) >= 0);
    while(!interrupted) {
	lws_service(context, 1000);
    }

    lws_context_destroy(context);
    
    lwsl_user("Server stoped!\n");
    return 0;
}
