//
// Copyright (c) 2019-2022 yanggaofeng
//

#include "libavutil/avstring.h"
#include "libavutil/opt.h"
#include "libavutil/time.h"

#include "libavformat/avformat.h"
#include "libavformat/url.h"
#include <pthread.h>

#include "libavformat/YangMetaConnection.h"

typedef struct PacketQueue {
    AVPacketList *first_pkt, *last_pkt;
    int is_started;
    int nb_packets;
    int abort_request;
    pthread_mutex_t mutex;
    pthread_cond_t condition;
} PacketQueue;

typedef struct WEBRTCContext {
    const AVClass *class;
    AVFormatContext *avctx;
    YangMetaConnection *handle;
    int video_stream_index_in;
    int video_stream_index_out;
    int audio_stream_index_in;
    int audio_stream_index_out;
    YangVideoCodec video_codec;
    YangAudioCodec audio_codec;
    // video info
    char *extradata;
    int extradata_size;
    // audio info
    int sample_rate;
    int num_channels;

    AVPacket video_pkt;
    AVPacket audio_pkt;
    PacketQueue queue;
    int max_queue_size;
    int  log_offset;
    int  error_code;

    char *server_address;
    int min_delay_ms;

	int dump_file;
    int handleMeta;
    char video_header[4];
} WEBRTCContext;


static void packet_queue_abort(PacketQueue *q)
{
    pthread_mutex_lock(&q->mutex);
    q->abort_request = 1;
    pthread_cond_signal(&q->condition);
    pthread_mutex_unlock(&q->mutex);
}
static int packet_queue_put(PacketQueue *q, AVPacket *pkt, WEBRTCContext *s)
{
    AVPacketList *pkt1;

    pthread_mutex_lock(&q->mutex);
    if (q->abort_request) {
        av_packet_unref(pkt);
        pthread_mutex_unlock(&q->mutex);
        return -1;
    }

    if (s->max_queue_size > 0 && q->nb_packets >= s->max_queue_size) {
        av_log(s->avctx, AV_LOG_INFO, "packet queue overflow, %d/%d, drop old packet\n", q->nb_packets, s->max_queue_size);
        pkt1 = q->first_pkt;
        q->first_pkt = pkt1->next;
        av_packet_unref(&pkt1->pkt);
        av_free(pkt1);
        q->nb_packets--;
    }

    pkt1 = av_malloc(sizeof(AVPacketList));
    pkt1->pkt = *pkt;
    pkt1->next = NULL;

    if (!q->last_pkt)
        q->first_pkt = pkt1;
    else
        q->last_pkt->next = pkt1;
    q->last_pkt = pkt1;

    q->is_started = 1;
    q->nb_packets++;
    pthread_cond_signal(&q->condition);

    pthread_mutex_unlock(&q->mutex);
    return 0;
}

static int packet_queue_get(PacketQueue *q, WEBRTCContext *s, AVPacket *pkt)
{
    AVPacketList *pkt1;
    int ret = 0;

    while (!ret) {
        if (ff_check_interrupt(&s->avctx->interrupt_callback)) {
            packet_queue_abort(q);
            ret = AVERROR_EXIT;;
            break;
        }

        pthread_mutex_lock(&q->mutex);
        if (s->error_code) {
             pthread_mutex_unlock(&q->mutex);
             return s->error_code;
        }
        pkt1 = q->first_pkt;
        if (pkt1) {
            q->first_pkt = pkt1->next;
            if (!q->first_pkt)
                q->last_pkt = NULL;
            q->nb_packets--;
            *pkt = pkt1->pkt;
            av_free(pkt1);
            ret = 1;
        } else {
            int64_t t = av_gettime() + 100000;
            struct timespec tv = { .tv_sec  =  t / 1000000,
                                   .tv_nsec = (t % 1000000) * 1000 };
            pthread_cond_timedwait(&q->condition, &q->mutex, &tv);
        }
        pthread_mutex_unlock(&q->mutex);
    }

    return ret;
}



static int packet_queue_wait_start(PacketQueue *q, WEBRTCContext *s, int64_t timeout) {
    int started;
    int loop = (timeout >= 0) ? FFMAX(timeout / 100000, 1) : -1;

    for (int i = 0; loop > 0 && i < loop; i++) {
        if (ff_check_interrupt(&s->avctx->interrupt_callback))
            return AVERROR_EXIT;

        pthread_mutex_lock(&q->mutex);
        if (s->error_code) {
             pthread_mutex_unlock(&q->mutex);
             return s->error_code;
        }
        if (!q->is_started) {
            int64_t t = av_gettime() + 100000;
            struct timespec tv = { .tv_sec  =  t / 1000000,
                    .tv_nsec = (t % 1000000) * 1000 };
            pthread_cond_timedwait(&q->condition, &q->mutex, &tv);
        }
        started = q->is_started;
        pthread_mutex_unlock(&q->mutex);
        if (started)
            return 0;
    }

    return AVERROR(ETIMEDOUT);
}


static void g_ff_rtc_setPlayMediaConfig(void* user,int32_t puid,YangAudioParam *remote_audio,YangVideoParam *remote_video){
	if(user==NULL) return;
	WEBRTCContext *s = (WEBRTCContext*)user;
	AVFormatContext *h = s->avctx;

	//audio.
	s->audio_codec = remote_audio->encode;
	if (s->audio_codec != Yang_AED_AAC && s->audio_codec != Yang_AED_OPUS) {
		av_log(h, AV_LOG_ERROR,
		"OnAudioInfoCallback, unknown audio codec %d\n",
		s->audio_codec);
		return;
	}
	s->sample_rate = remote_audio->sample;
	s->num_channels = remote_audio->channel;
	av_log(h, AV_LOG_INFO, "AudioInfo, audio codec %d, sample rate %d, num channels %d\n",
	s->audio_codec, s->sample_rate, s->num_channels);
	if (h->ctx_flags & AVFMTCTX_NOHEADER) {
		AVPacket *pkt1 = &s->audio_pkt;
		av_new_packet(pkt1, 0);
		pkt1->stream_index = s->audio_stream_index_in;
		packet_queue_put(&s->queue, pkt1, s);
	
	}
	
	//video
	s->video_codec = remote_video->encode;
	
	if (s->video_codec != Yang_VED_264 && s->video_codec != Yang_VED_265) {
		av_log(h, AV_LOG_ERROR,
		"OnVideoInfoCallback, unknown video codec %d\n",
		s->video_codec);
		return;
	}
	av_log(h, AV_LOG_INFO, "VideoInfo, video codec %d\n", remote_video->encode);
	

}

static void g_ff_rtc_sendRequest(void* user,int32_t puid,uint32_t ssrc,YangRequestType req){
	if(user==NULL) return;
	 WEBRTCContext *s = (WEBRTCContext*)user;
	 AVFormatContext *h = s->avctx;
	 if(req==Yang_Req_Connected){
			av_log(h, AV_LOG_INFO, "webrtc connect rtc server success!\n");
		}

}
static void g_ff_rtc_receiveAudio(void* user,YangFrame *audioFrame){
	if(user==NULL) return;
	 WEBRTCContext *s = (WEBRTCContext*)user;
	    AVPacket *pkt = &s->audio_pkt;

	    av_new_packet(pkt, audioFrame->nb);
	   //  memcpy(pkt->data, s->video_header, 4);
	    memcpy(pkt->data, audioFrame->payload, audioFrame->nb);
	    pkt->stream_index = s->audio_stream_index_in;
	    pkt->dts = audioFrame->pts;
	    pkt->pts = audioFrame->pts;
	    packet_queue_put(&s->queue, pkt, s);
}
static void g_ff_rtc_receiveVideo(void* user,YangFrame *videoFrame){
	if(user==NULL) return;

	WEBRTCContext *s = (WEBRTCContext*) user;
	AVFormatContext *h = s->avctx;
	if (videoFrame->frametype == YANG_Frametype_Spspps) {
		if(s->extradata_size >0 ) return;
		uint8_t headers[128];
		memset(headers, 0, 128);
		int32_t headerLen = 0;
		if (s->handle->parseHeader)
			s->handle->parseHeader(s->video_codec, videoFrame->payload, headers,&headerLen);
		if (headerLen > 0) {
			s->extradata = av_malloc(headerLen);
			if (!s->extradata) {
				s->error_code = AVERROR(ENOMEM);
				return;
			}
			memcpy(s->extradata, headers, headerLen);
			s->extradata_size = headerLen;

			//just a fake video packet for create stream
			if (h->ctx_flags & AVFMTCTX_NOHEADER) {
				AVPacket *pkt = &s->video_pkt;
				av_new_packet(pkt, 0);
				pkt->stream_index = s->video_stream_index_in;
				packet_queue_put(&s->queue, pkt, s);
			}
		}
		return;
	}
	if(s->extradata_size ==0 ) return;
	   AVPacket *pkt = &s->video_pkt;
		
	    av_new_packet(pkt, videoFrame->nb);
	    memcpy(pkt->data, videoFrame->payload, videoFrame->nb);
	    memcpy(pkt->data, s->video_header, 4);
	 
	    pkt->stream_index = s->video_stream_index_in;
	    pkt->dts = videoFrame->pts;
	    pkt->pts = videoFrame->pts;
	    packet_queue_put(&s->queue, pkt, s);

}

static int packet_queue_init(PacketQueue *q, void *logctx)
{
    int ret;

    memset(q, 0, sizeof(PacketQueue));
    ret = pthread_mutex_init(&q->mutex, NULL);
    if (ret != 0) {
        ret = AVERROR(ret);
        av_log(logctx, AV_LOG_ERROR, "pthread_mutex_init failed : %s\n", av_err2str(ret));
        return ret;
    }
    ret = pthread_cond_init(&q->condition, NULL);
    if (ret != 0) {
        pthread_mutex_destroy(&q->mutex);
        ret = AVERROR(ret);
        av_log(logctx, AV_LOG_FATAL, "pthread_cond_init failed : %s\n", av_err2str(ret));
        return ret;
    }

    return 0;
}

static void packet_queue_destroy(PacketQueue *q)
{
    AVPacketList *pkt, *nextpkt;

    for (pkt = q->first_pkt; pkt; pkt = nextpkt) {
        nextpkt = pkt->next;
        av_packet_unref(&pkt->pkt);
        av_freep(&pkt);
    }
    q->last_pkt = NULL;
    q->first_pkt = NULL;
    q->nb_packets = 0;
    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->condition);
}



/* keep it the same as avpriv_set_pts_info */
static void set_stream_pts_info(AVStream *s, int pts_wrap_bits,
                         unsigned int pts_num, unsigned int pts_den)
{
    AVRational new_tb;
    if (av_reduce(&new_tb.num, &new_tb.den, pts_num, pts_den, INT_MAX)) {
        if (new_tb.num != pts_num)
            av_log(NULL, AV_LOG_DEBUG,
                   "st:%d removing common factor %d from timebase\n",
                   s->index, pts_num / new_tb.num);
    } else
        av_log(NULL, AV_LOG_WARNING,
               "st:%d has too large timebase, reducing\n", s->index);

    if (new_tb.num <= 0 || new_tb.den <= 0) {
        av_log(NULL, AV_LOG_ERROR,
               "Ignoring attempt to set invalid timebase %d/%d for st:%d\n",
               new_tb.num, new_tb.den,
               s->index);
        return;
    }
    s->time_base     = new_tb;
#if FF_API_LAVF_AVCTX
    s->codec->pkt_timebase = new_tb;
#endif
    s->pts_wrap_bits = pts_wrap_bits;
}




static int webrtc_open(AVFormatContext *h, const char *uri)
{
    WEBRTCContext *s = h->priv_data;
    av_log(h, AV_LOG_INFO, "webrtc_open %s\n", uri);
    packet_queue_init(&s->queue, h);
    av_init_packet(&s->video_pkt);
    av_init_packet(&s->audio_pkt);


    s->video_codec = -1;
    s->audio_codec = -1;
	s->video_header[0]=0x00;
	s->video_header[1]=0x00;
	s->video_header[2]=0x00;
	s->video_header[3]=0x01;
	s->handleMeta=0;
    if (!av_strstart(uri, "webrtc://", NULL))  {
        packet_queue_destroy(&s->queue);
        return AVERROR(EINVAL);
    }




    s->handle =(YangMetaConnection*) calloc(sizeof(YangMetaConnection),1);
    yang_create_metaConnection(s->handle);





    YangStreamConfig stream;
    memset(&stream,0,sizeof(YangStreamConfig));
    stream.rtcCallback.context=s;
    stream.rtcCallback.setMediaConfig=g_ff_rtc_setPlayMediaConfig;
    stream.rtcCallback.sendRequest=g_ff_rtc_sendRequest;
    stream.recvCallback.context=s;
    stream.recvCallback.receiveAudio=g_ff_rtc_receiveAudio;
    stream.recvCallback.receiveVideo=g_ff_rtc_receiveVideo;

	if(s->handle->init) s->handle->init(s->handle->session,&stream,s);
    if(s->handle->initParam) s->handle->initParam(s->handle->session,uri,Yang_Stream_Play);
//Yang_Server_Srs/Yang_Server_Zlm/Yang_Server_P2p
    int32_t mediaServer=Yang_Server_Srs;
    if(s->handle->connectSfuServer(s->handle->session,mediaServer)!=0){
    	av_log(h, AV_LOG_ERROR, "connect failed! uri= %s\n",uri);
    }


    av_log(h, AV_LOG_INFO, "webrtc_open exit\n");
    return 0;
}

static int webrtc_close(AVFormatContext *h)
{
    av_log(h, AV_LOG_INFO, "webrtc_close\n");
    WEBRTCContext *s = h->priv_data;
    av_free(s->extradata);

    packet_queue_abort(&s->queue);

    yang_destroy_metaConnection(s->handle);
    yang_free(s->handle);

    packet_queue_destroy(&s->queue);

    av_log(h, AV_LOG_INFO, "webrtc_close exit\n");
    return 0;
}

static AVStream *create_stream(AVFormatContext *s, int codec_type)
{
    WEBRTCContext *h   = s->priv_data;
    av_log(s, AV_LOG_INFO, "create_stream, codec_type %d\n", codec_type);

    AVStream *st = avformat_new_stream(s, NULL);
    if (!st)
        return NULL;
    st->codecpar->codec_type = codec_type;

    if (s->nb_streams >= 2)
        s->ctx_flags &= ~AVFMTCTX_NOHEADER;

    if (codec_type == AVMEDIA_TYPE_VIDEO) {
        if (h->video_codec == Yang_VED_264) {
            st->codecpar->codec_id = AV_CODEC_ID_H264;
        } else if (h->video_codec == Yang_VED_265) {
            st->codecpar->codec_id = AV_CODEC_ID_H265;
        }
        st->need_parsing = AVSTREAM_PARSE_FULL;
        h->video_stream_index_out = st->index;

        if (h->extradata_size > 0) {
            st->codecpar->extradata = av_malloc(h->extradata_size + AV_INPUT_BUFFER_PADDING_SIZE);
            if (st->codecpar->extradata != NULL) {
                memset(st->codecpar->extradata + h->extradata_size, 0, AV_INPUT_BUFFER_PADDING_SIZE);
                memcpy(st->codecpar->extradata, h->extradata, h->extradata_size);
                st->codecpar->extradata_size = h->extradata_size;

            } else {
                st->codecpar->extradata_size = 0;
            }

        }
		set_stream_pts_info(st, 64, 1, 90000);
    }

    if (codec_type == AVMEDIA_TYPE_AUDIO) {
        if (h->audio_codec == Yang_AED_AAC) {
            st->codecpar->codec_id = AV_CODEC_ID_AAC;

        } else if (h->audio_codec == Yang_AED_OPUS) {
            st->codecpar->codec_id = AV_CODEC_ID_OPUS;
        }
        st->codecpar->sample_rate = h->sample_rate;
        st->codecpar->channels = h->num_channels;
        st->need_parsing = AVSTREAM_PARSE_HEADERS;
        h->audio_stream_index_out = st->index;
        set_stream_pts_info(st, 64, 1, h->sample_rate);

    }

    
    return st;
}

static int webrtc_probe(AVProbeData *p)
{
    if (av_strstart(p->filename, "webrtc:", NULL))
        return AVPROBE_SCORE_MAX;
    return 0;
}

static int webrtc_read_header(AVFormatContext *s)
{
    WEBRTCContext *h = s->priv_data;
    int ret;

    av_log(s, AV_LOG_INFO, "webrtc_read_header, filename %s\n", s->filename);

    s->flags |= AVFMT_FLAG_GENPTS;
    s->ctx_flags |= AVFMTCTX_NOHEADER;
    s->fps_probe_size = 0;
    s->max_analyze_duration = FFMAX(s->max_analyze_duration, 5*AV_TIME_BASE);
    s->probesize = FFMAX(s->probesize, 512*1024);
    h->avctx = s;
    h->video_stream_index_in = 0;
    h->audio_stream_index_in = 1;
    h->video_stream_index_out = -1;
    h->audio_stream_index_out = -1;
    ret = webrtc_open(s, s->filename);
    if (ret) {
        av_log(s, AV_LOG_ERROR, "webrtc_read_header: webrtc_open failed, %s\n", av_err2str(ret));
        return ret;
    }

    // 5秒收不到数据，超时退出
    ret = packet_queue_wait_start(&h->queue, h, INT64_C(1000) * 5000);
    if (ret) {
        av_log(s, AV_LOG_ERROR, "webrtc_read_header wait failed, %s\n", av_err2str(ret));
        webrtc_close(s);
        return ret;
    }

    av_log(s, AV_LOG_INFO, "webrtc_read_header exit\n");
    return 0;
}

static int webrtc_read_packet(AVFormatContext *s, AVPacket *pkt)
{
    int ret, i;
    WEBRTCContext *h = s->priv_data;
    AVStream *st;

    do {
		ret = packet_queue_get(&h->queue, h, pkt);

        if (ret < 0)
            break;

        /* now find stream */
        for (i = 0; i < s->nb_streams; i++) {
            st = s->streams[i];
            if (pkt->stream_index == h->video_stream_index_in
                && st->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
                break;
            } else if (pkt->stream_index == h->audio_stream_index_in
                       && st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
                break;
            }
        }
        if (i == s->nb_streams) {
            static const enum AVMediaType stream_types[] = {AVMEDIA_TYPE_VIDEO, AVMEDIA_TYPE_AUDIO};
            st = create_stream(s, stream_types[pkt->stream_index]);
            if (!st) {
                av_packet_unref(pkt);
                ret = AVERROR(ENOMEM);
                break;
            }
        }

        if (pkt->size <= 0) {
            // drop fake packet
            av_packet_unref(pkt);
            continue;
        }

        if (pkt->stream_index == h->video_stream_index_in) {
            pkt->stream_index = h->video_stream_index_out;

        } else if (pkt->stream_index == h->audio_stream_index_in) {
            pkt->stream_index = h->audio_stream_index_out;

        } else {
            ret = 0;
        }

        if (!ret) {
            av_log(s, AV_LOG_INFO, "drop pkt with index %d and continue\n",
                   pkt->stream_index);
            av_packet_unref(pkt);
        }
    } while (!ret);

    ret = ret > 0 ? 0 : ret;
    if (ret)
        av_log(s, AV_LOG_WARNING, "webrtc_read_packet, %s\n", av_err2str(ret));
    return ret;
}

static int webrtc_read_close(AVFormatContext *s)
{
    av_log(s, AV_LOG_INFO, "webrtc_read_close\n");
    webrtc_close(s);
    av_log(s, AV_LOG_INFO, "webrtc_read_close exit\n");
    return 0;
}


#define OFFSET(x) offsetof(WEBRTCContext, x)
#define D AV_OPT_FLAG_DECODING_PARAM
#define E AV_OPT_FLAG_ENCODING_PARAM
static const AVOption options[] = {
    { "webrtc_log_offset", "Adjust WebRTC to FFmpeg logging level mapping, negative to reduce verbosity and vice versa", OFFSET(log_offset), AV_OPT_TYPE_INT, { .i64 = 0 }, -4, 3, D },
    { "webrtc_max_queue_size", "The maximum number of packets can hold before dropping, default to -1 for unlimited", OFFSET(max_queue_size), AV_OPT_TYPE_INT, { .i64 = -1 }, -1, INT_MAX, D },
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

AVInputFormat ff_webrtc_demuxer = {
    .name           = "webrtc",
    .long_name      = "webrtc demuxer",
    .priv_data_size = sizeof(WEBRTCContext),
    .read_probe     = webrtc_probe,
    .read_header    = webrtc_read_header,
    .read_packet    = webrtc_read_packet,
    .read_close     = webrtc_read_close,
    .extensions      = "webrtc",
    .priv_class     = &webrtc_class,
    .flags          = AVFMT_NOFILE,
};

#ifdef BUILD_AS_PLUGIN
void register_webrtc_demuxer()
{
    av_log(NULL, AV_LOG_INFO, "register_webrtc_demuxer\n");
    av_register_input_format(&ff_webrtc_demuxer);
}
#endif

