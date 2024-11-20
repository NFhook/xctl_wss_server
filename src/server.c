#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int callback_keepalive(struct lws *wsi, enum lws_callback_reasons reason,
                       void *user, void *in, size_t len) {
    switch (reason) {
        case LWS_CALLBACK_RECEIVE: {
            yyjson_doc *doc = yyjson_read((const char *)in, len, 0);
            if (!doc) {
                lwsl_err("Invalid JSON received\n");
                break;
            }
            yyjson_val *root = yyjson_doc_get_root(doc);
            yyjson_val *type = yyjson_obj_get(root, "type");
            if (type && strcmp(yyjson_get_str(type), "keepalive") == 0) {
                yyjson_mut_doc *res_doc = yyjson_mut_doc_new(NULL);
                yyjson_mut_val *res_root = yyjson_mut_obj(res_doc);
                yyjson_mut_doc_set_root(res_doc, res_root);
                yyjson_mut_obj_add_str(res_doc, res_root, "type", "ack");

                size_t out_len;
                char *response = yyjson_mut_write(res_doc, 0, &out_len);
                lws_write(wsi, (unsigned char *)response + LWS_PRE, out_len, LWS_WRITE_TEXT);

                free(response);
                yyjson_mut_doc_free(res_doc);
            }
            yyjson_doc_free(doc);
            break;
        }

        case LWS_CALLBACK_TIMER: {
            lws_callback_on_writable(wsi);
            lws_set_timer_usecs(wsi, KEEPALIVE_INTERVAL_SEC * LWS_USEC_PER_SEC);
            break;
        }

        default:
            break;
    }
    return 0;
}
