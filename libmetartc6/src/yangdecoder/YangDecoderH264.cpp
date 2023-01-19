//
// Copyright (c) 2019-2022 yanggaofeng
//

#include "YangDecoderH264.h"
#if !Yang_Enable_H264Decoder_So
extern "C"{
	#include <h264decoder/libavcodec/avcodec.h>
}
extern AVCodec ff_h264_decoder;
extern AVCodecParser ff_h264_parser;
struct YangH264Context2{
	AVCodec *m_codec;
	AVCodecContext *m_codecCtx;
	AVCodecParserContext *m_parser;
	AVPacket packet;
	AVFrame *m_frame;

};



YangDecoderH264::YangDecoderH264(){
    m_context=(YangH264DecContext*)calloc(sizeof(struct YangH264DecContext),1);
	m_context->context=calloc(1,sizeof(struct YangH264Context2));
}
YangDecoderH264::~YangDecoderH264(){
	decode_close();
}

void YangDecoderH264::init(uint8_t *headers, int headerLen) {
    struct YangH264Context2 *p264=(struct YangH264Context2 *)m_context->context;
	avcodec_register(&ff_h264_decoder);
	av_register_codec_parser(&ff_h264_parser);

	p264->m_codec = avcodec_find_decoder(AV_CODEC_ID_H264);
	p264->m_codecCtx = avcodec_alloc_context3(p264->m_codec);

	p264->m_codecCtx->extradata = (uint8_t*) av_malloc(
			headerLen + AV_INPUT_BUFFER_PADDING_SIZE);
	p264->m_codecCtx->extradata_size = headerLen;
	memcpy(p264->m_codecCtx->extradata, headers, headerLen);

	p264->m_parser = av_parser_init(AV_CODEC_ID_H264);
	if (!p264->m_parser) {
		fprintf(stderr, "Could not create H264 parser\n");
		exit(1);
	}
	int ret = avcodec_open2(p264->m_codecCtx, p264->m_codec, NULL);
	if (ret < 0)
		printf("\navcodec_open2 failure\n");
	//AVPixelFormat fmt = AV_PIX_FMT_YUV420P;


	av_yang_initExtra(p264->m_parser,p264->m_codecCtx,&m_context->m_width,&m_context->m_height,&m_context->m_fps);
	m_context->yLen = m_context->m_width * m_context->m_height ;
	m_context->uLen = m_context->yLen / 4;
	m_context->allLen = m_context->yLen * 3 / 2;
	p264->m_frame = av_frame_alloc();

	av_init_packet(&p264->packet);

	//m_isInit = 1;
}



int YangDecoderH264::decode(int isIframe,unsigned char *pData, int nSize,enum YangYuvType pyuvType,
		unsigned char *dest, int *pnFrameReturned) {
	struct YangH264Context2 *p264=(struct YangH264Context2 *)m_context->context;
	uint8_t *data = NULL;
			int t_size = 0;
			int bytes_used = av_parser_parse2(p264->m_parser, p264->m_codecCtx, &data, &t_size, pData,
					nSize, 0, 0, AV_NOPTS_VALUE);

			if (t_size == 0) {
					 if (bytes_used == nSize) {
				                av_parser_parse2(p264->m_parser, p264->m_codecCtx, &data, &t_size, pData, nSize, 0, 0, AV_NOPTS_VALUE);

				            }
			}
			if (bytes_used > 0) {
						av_init_packet(&p264->packet);
						p264->packet.data = data;
						p264->packet.size = t_size;
						int got_frame = 0;
							int len = avcodec_decode_video2(p264->m_codecCtx, p264->m_frame, &got_frame, &p264->packet);
							if (len < 0) {
								return 1;
							}

							if (got_frame) {


							   for (int i = 0; i < m_context->m_height; i++) {
							        memcpy(dest + i * m_context->m_width, p264->m_frame->data[0] + i * p264->m_frame->linesize[0], m_context->m_width);
							   }
							  for (int i = 0; i < m_context->m_height / 2; i++) {
							      memcpy(dest + m_context->yLen+i * m_context->m_width / 2,p264-> m_frame->data[1] + i * p264->m_frame->linesize[1], m_context->m_width / 2);
							   }
							   for (int i = 0; i < m_context->m_height / 2; i++) {
							       memcpy(dest + m_context->yLen+ m_context->uLen+ i * m_context->m_width / 2, p264->m_frame->data[2] + i * p264->m_frame->linesize[1], m_context->m_width / 2);
							  }

								*pnFrameReturned = m_context->allLen;
							}
					}
			p264=NULL;
			data=NULL;
		return 0;
}


void YangDecoderH264::decode_close() {
	if(m_context){
		struct YangH264Context2 *p264=(struct YangH264Context2 *)m_context->context;
		av_frame_free(&p264->m_frame);
		p264->m_frame = NULL;
		if (p264->m_codecCtx) {
			avcodec_close(p264->m_codecCtx);
			av_free(p264->m_codecCtx);
		}


		p264->m_codecCtx = NULL;
		if(p264->m_parser)	av_parser_close(p264->m_parser);
		p264->m_parser=NULL;
		av_frame_free(&p264->m_frame);
		p264=NULL;

		if(m_context->context) free(m_context->context);
		m_context->context=NULL;
		free(m_context);
		m_context=NULL;
	}

}
#endif
