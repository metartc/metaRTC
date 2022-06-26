//
// Copyright (c) 2019-2022 yanggaofeng
//

#include "avformat.h"

#include "libavutil/avstring.h"
#include "libavutil/opt.h"
#include "libavutil/time.h"

#include "libavformat/avformat.h"
#include "libavformat/url.h"

#include <pthread.h>
#include "libavformat/YangMetaConnection.h"
#define SDP_MAX_SIZE 16384


typedef struct WEBRTCContext {
	const AVClass *class;
	AVFormatContext *avctx;
	YangMetaConnection *handle;
	YangVideoCodec video_codec;
	YangAudioCodec audio_codec;
	int video_stream_index;
	int audio_stream_index;
	// video info
	char *extradata;
	int extradata_size;
	// audio info
	int sample_rate;
	int num_channels;

	YangFrame video_frame;
	YangFrame audio_frame;

	int  log_offset;
	int  error_code;

	char *server_address;
	int min_delay_ms;
	int time_base_den;
	int is_started;
	int dump_file;
	pthread_mutex_t mutex;
	pthread_cond_t condition;

} WEBRTCContext;

static void g_ff_rtc_setPlayMediaConfig(void* user,int32_t puid,YangAudioParam *remote_audio,YangVideoParam *remote_video){
	if(user==NULL) return;
	//WEBRTCContext *s = (WEBRTCContext*)user;
	//AVFormatContext *h = s->avctx;



}

static void g_ff_rtc_sendRequest(void* user,int32_t puid,uint32_t ssrc,YangRequestType req){
	if(user==NULL) return;

	WEBRTCContext *s = (WEBRTCContext*)user;
	AVFormatContext *h = s->avctx;
	if(req==Yang_Req_Connected){
		s->is_started=1;
		pthread_cond_signal(&s->condition);
		pthread_mutex_unlock(&s->mutex);
		av_log(h, AV_LOG_INFO, "webrtc connect rtc server success!\n");
	}
}


static void g_ff_rtc_receiveAudio(void* user,YangFrame *audioFrame){
	if(user==NULL) return;
	WEBRTCContext *s = (WEBRTCContext*)user;

}
static void g_ff_rtc_receiveVideo(void* user,YangFrame *videoFrame){
	if(user==NULL) return;



}
static int packet_queue_wait_start( WEBRTCContext *s, int64_t timeout) {
	int started;
	int loop = (timeout >= 0) ? FFMAX(timeout / 100000, 1) : -1;

	for (int i = 0; loop > 0 && i < loop; i++) {
		if (ff_check_interrupt(&s->avctx->interrupt_callback))
			return AVERROR_EXIT;

		pthread_mutex_lock(&s->mutex);

		if (!s->is_started) {
			int64_t t = av_gettime() + 100000;
			struct timespec tv = { .tv_sec  =  t / 1000000,
					.tv_nsec = (t % 1000000) * 1000 };

			pthread_cond_timedwait(&s->condition, &s->mutex, &tv);

		}
		started = s->is_started;
		pthread_mutex_unlock(&s->mutex);

		if (started)
			return 0;
	}

	return AVERROR(ETIMEDOUT);
}




static int webrtc_open(AVFormatContext *h, const char *uri)
{
	WEBRTCContext *s = h->priv_data;
	av_log(h, AV_LOG_INFO, "webrtc_open %s\n", uri);
	s->video_stream_index=-1;
	s->audio_stream_index=-1;


	s->video_codec = AV_CODEC_ID_H264;
	s->audio_codec = AV_CODEC_ID_OPUS;



	if (!av_strstart(uri, "webrtc://", NULL))  {
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
	if(s->handle->initParam) s->handle->initParam(s->handle->session,uri,Yang_Stream_Publish);
	//Yang_Server_Srs/Yang_Server_Zlm/Yang_Server_P2p
	int32_t mediaServer=Yang_Server_Srs;
	if(s->handle->connectServer(s->handle->session,mediaServer)!=0){
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



	yang_destroy_metaConnection(s->handle);
	yang_free(s->handle);



	av_log(h, AV_LOG_INFO, "webrtc_close exit\n");
	return 0;
}

static void webrtc_deinit(struct AVFormatContext *s)
{
	printf("\n webrtc deinit>>>>>>>>>>>>>%s\n",s->filename);
}

static int webrtc_init(struct AVFormatContext *s)
{
	WEBRTCContext *h = s->priv_data;
	printf("\n webrtc init>>>>>>>>>>>>>>>>>>>%s\n",s->filename);
	h->avctx = s;
	h->video_stream_index= -1;
	h->audio_stream_index = -1;
	h->time_base_den=30;
	int ret = webrtc_open(s, s->filename);
	if (ret) {
		av_log(s, AV_LOG_ERROR, "webrtc_write_header: webrtc_open failed, %s\n", av_err2str(ret));
		return ret;
	}

	return ret;
}


static int webrtc_write_header(AVFormatContext *s)
{
	WEBRTCContext *h = s->priv_data;
	int ret;

	av_log(s, AV_LOG_INFO, "webrtc_write_header, filename %s\n", s->filename);

	// 5秒收不到数据，超时退出
	ret = packet_queue_wait_start( h, INT64_C(1000) * 5000);
	if (ret) {
		av_log(s, AV_LOG_ERROR, "webrtc_write_header wait failed, %s\n", av_err2str(ret));
		webrtc_close(s);
		return ret;
	}

	av_log(s, AV_LOG_INFO, "webrtc_write_header exit\n");
	return 0;
}

static int get_videodata_start(char* data){
	if(data[0]==0x00&&data[1]==0x00&&data[2]==0x00&&data[3]==0x01) return 0;
	//if(data[0]==0x00&&data[1]==0x00&&data[2]==0x01) return 3;
	return 1;
}

static int webrtc_write_packet(AVFormatContext *h, AVPacket *pkt)
{
	if(pkt==NULL) return 0;
	WEBRTCContext *s = h->priv_data;
	YangMetaConnection* metaconn=s->handle;

	int ret=0;
	if(s->video_stream_index==-1||s->audio_stream_index==-1){

		for(int i=0;i<h->nb_streams;i++){
			AVStream* st = h->streams[i];

			if(st->codecpar->codec_id ==AV_CODEC_ID_H264) {
				s->video_stream_index=st->index;	
				s->time_base_den=st->time_base.den;

				if(st->codecpar->extradata){
					metaconn->setExtradata(metaconn->context,Yang_VED_264,(uint8_t*)st->codecpar->extradata,st->codecpar->extradata_size);
				}			
			}
			if(st->codecpar->codec_id == AV_CODEC_ID_OPUS) s->audio_stream_index=st->index;
		}

	}



	if(pkt->stream_index==s->video_stream_index){
		//if(get_videodata_start(pkt->data)) return ret;	
		s->video_frame.nb=pkt->size;
		s->video_frame.payload=pkt->data;
		s->video_frame.pts=pkt->pts*1000000/s->time_base_den;

		ret=metaconn->publishVideo(metaconn->context,&s->video_frame);
	}else if(pkt->stream_index==s->audio_stream_index){
		s->audio_frame.nb=pkt->size;
		s->audio_frame.payload=pkt->data;
		s->audio_frame.pts=pkt->pts;
		ret=metaconn->publishAudio(metaconn->context,&s->audio_frame);
	}

	return ret;

}

static int webrtc_write_close(AVFormatContext *s)
{
	av_log(s, AV_LOG_INFO, "webrtc_write_close\n");
	webrtc_close(s);
	av_log(s, AV_LOG_INFO, "webrtc_write_close exit\n");
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
static const AVClass webrtc_muxer_class = {
		.class_name = "webrtc",
		.item_name  = av_default_item_name,
		.option     = options,
		.version    = LIBAVUTIL_VERSION_INT,
};

AVOutputFormat ff_webrtc_muxer = {
		.name           = "webrtc",
		.long_name      = "webrtc muxer",
		.priv_data_size    = sizeof(WEBRTCContext),
		.audio_codec       = AV_CODEC_ID_OPUS,
		.video_codec       = AV_CODEC_ID_H264,
		.init              = webrtc_init,
		.write_header      = webrtc_write_header,
		.write_packet      = webrtc_write_packet,
		.write_trailer     = webrtc_write_close,
		.deinit            = webrtc_deinit,
		.flags             = AVFMT_NOFILE | AVFMT_GLOBALHEADER,
		.priv_class        = &webrtc_muxer_class,
};
#ifdef BUILD_AS_PLUGIN
void register_webrtc_muxer()
{
	av_log(NULL, AV_LOG_INFO, "register_webrtc_muxer\n");
	av_register_input_format(&ff_webrtc_muxer);
}
#endif
