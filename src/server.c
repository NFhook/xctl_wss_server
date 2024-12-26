#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


int callback_keepalive(struct lws *wsi, enum lws_callback_reasons reason,
                       void *user, void *in, size_t len) {
    switch (reason) {
        case LWS_CALLBACK_RECEIVE: {
            // receive
            const char *recv_msg = (const char *)in;
            yyjson_doc *doc = yyjson_read(recv_msg, len, 0);
            if (!doc) {
                lwsl_err("Invalid JSON received\n");
                return -1;
            }

            yyjson_val *root = yyjson_doc_get_root(doc);
            yyjson_val *type = yyjson_obj_get(root, "type");
            if (type && strcmp(yyjson_get_str(type), "keepalive") == 0) {
                // create response
                yyjson_mut_doc *reply_doc = yyjson_mut_doc_new(NULL);
                yyjson_mut_val *reply_root = yyjson_mut_obj(reply_doc);
                yyjson_mut_doc_set_root(reply_doc, reply_root);
                yyjson_mut_obj_add_str(reply_doc, reply_root, "type", "ack");
                yyjson_mut_obj_add_uint(reply_doc, reply_root, "timestamp", time(NULL));

                // JSON
                char *reply_msg = yyjson_mut_write(reply_doc, 0, NULL);
                if (reply_msg) {
                    unsigned char buffer[LWS_PRE + 1024];
                    size_t msg_len = strlen(reply_msg);

                    if (msg_len < 1024) {
                        memcpy(&buffer[LWS_PRE], reply_msg, msg_len);
                        lws_write(wsi, &buffer[LWS_PRE], msg_len, LWS_WRITE_TEXT);
                    } else {
                        lwsl_err("Reply message too large\n");
                    }
                    free(reply_msg);
                }
                yyjson_mut_doc_free(reply_doc);
            }
            yyjson_doc_free(doc);
            break;
        }

        case LWS_CALLBACK_ESTABLISHED: {
            // client estalibsh
            lwsl_user("Connection established\n");
            break;
        }

        case LWS_CALLBACK_CLOSED: {
            // client close 
            lwsl_user("Connection closed\n");
            break;
        }

        case LWS_CALLBACK_TIMER: {
            // timer
            lws_callback_on_writable(wsi);
            lws_set_timer_usecs(wsi, KEEPALIVE_INTERVAL_SEC * LWS_USEC_PER_SEC);
            break;
        }

        case LWS_CALLBACK_SERVER_WRITEABLE: {
            // 服务器可写回调
            break;
        }

        default:
            break;
    }
    return 0;
}

