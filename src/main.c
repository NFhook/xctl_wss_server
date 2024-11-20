#include <libwebsockets.h>
#include "server.h"

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

    while (lws_service(context, 1000) >= 0);

    lws_context_destroy(context);
    return 0;
}
