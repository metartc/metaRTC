//
// Copyright (c) 2019-2022 yanggaofeng
//
#include "libavutil/parseutils.h"
#include "libavutil/avstring.h"
#include "libavutil/opt.h"
#include "avformat.h"
#include "avio_internal.h"
#include "url.h"
#include "ip.h"

#include "libavformat/url.h"

#include <pthread.h>
#include "libavformat/YangMetaConnection.h"

typedef struct WEBRTCContext {
    const AVClass *class;

    YangMetaConnection *handle;
    YangVideoCodec video_codec;


  
    int  log_offset;
    int  error_code;

    char *server_address;
    int min_delay_ms;

	int is_started;
	int dump_file;

} WEBRTCContext;







static int webrtc_open(URLContext *h, const char *uri, int flags)
{
     WEBRTCContext *s = h->priv_data;
    av_log(h, AV_LOG_INFO, "protocol webrtc_open %s\n", uri);
 
	
    return 0;
}

static int webrtc_read(URLContext *h, uint8_t *buf, int size)
{
  
   return 0;
}

static int webrtc_write(URLContext *h, const uint8_t *buf, int size)
{
    WEBRTCContext *s = h->priv_data;
    int ret=0;
	
    return ret;
}

static int webrtc_close(URLContext *h)
{
	 av_log(h, AV_LOG_INFO, "protocol webrtc_close \n");

    return 0;
}




#define OFFSET(x) offsetof(WEBRTCContext, x)
#define D AV_OPT_FLAG_DECODING_PARAM
#define E AV_OPT_FLAG_ENCODING_PARAM
static const AVOption options[] = {
    { "webrtc_log_offset", "Adjust WebRTC to FFmpeg logging level mapping, negative to reduce verbosity and vice versa", OFFSET(log_offset), AV_OPT_TYPE_INT, { .i64 = 0 }, -4, 3, D },
    { "webrtc_max_queue_size", "The maximum number of packets can hold before dropping, default to -1 for unlimited", 20, AV_OPT_TYPE_INT, { .i64 = -1 }, -1, INT_MAX, D },
    { "webrtc_dump_file", "Dump video and audio raw data to file", OFFSET(dump_file), AV_OPT_TYPE_BOOL, { .i64 = 0 }, 0, 1, D },
    { "webrtc_server_address", "Webrtc server address", OFFSET(server_address), AV_OPT_TYPE_STRING, { .str = NULL }, 0, 0, D },
    { "webrtc_min_delay", "the duration of the data to be sent for the first rollback GOP", OFFSET(min_delay_ms), AV_OPT_TYPE_INT, { .i64 = 1000 }, -1, INT_MAX, D },
    { NULL }
};

static const AVClass webrtc_class = {
    .class_name = "webrtc",
    .item_name  = av_default_item_name,
    .option     = options,
    .version    = LIBAVUTIL_VERSION_INT,
};

const URLProtocol ff_webrtc_protocol = {
    .name                      = "webrtc",
    .url_open                  = webrtc_open,
    .url_read                  = webrtc_read,
    .url_write                 = webrtc_write,
    .url_close                 = webrtc_close,
   // .url_get_file_handle       = webrtc_get_file_handle,
    //.url_get_multi_file_handle = webrtc_get_multi_file_handle,
    .priv_data_size            = sizeof(WEBRTCContext),
    .flags                     = URL_PROTOCOL_FLAG_NETWORK,
    .priv_data_class           = &webrtc_class,
};
