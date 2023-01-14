//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangipc/YangIpcEncoder.h>
#include <yangipc_jz/YangJzCommon.h>

#include <yangutil/yangavinfotype.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangEndian.h>
#include <stdlib.h>
#include <malloc.h>

#include <imp/imp_common.h>
#include <imp/imp_system.h>
#include <imp/imp_framesource.h>
#include <imp/imp_encoder.h>


extern struct chn_conf chn[];
void yang_jzEncoder_sendMsgToEncoder(YangEncoderSession* session,YangRequestType request){

	if(request==Yang_Req_Sendkeyframe){
		int ret = 0;
		ret = IMP_Encoder_RequestIDR(0);
		yang_trace("IMP_Encoder_RequestIDR rel:%d\n", ret);
	}else if(request==Yang_Req_HighLostPacketRate){

	}else if(request==Yang_Req_LowLostPacketRate){

	}
}

int yang_jzEncoder_save_stream(YangVideoEncoderBuffer2* buf,YangFrame* frame, IMPEncoderStream *stream)
{
	int ret, i, nr_pack = stream->packCount;
	uint8_t* tmp=frame->payload;
	int32_t isKeyframe=nr_pack>1?1:0;
	int32_t nb=0;
	//yang_debug( "----------packCount=%d, stream->seq=%u start----------\n", stream->packCount, stream->seq);
	
	for (i = 0; i < nr_pack; i++) {
		//yang_debug( "[%d]:%10u,%10lld,%10u,%10u,%10u\n", i, stream->pack[i].length, stream->pack[i].timestamp, stream->pack[i].frameEnd, *((uint32_t *)(&stream->pack[i].nalType)), stream->pack[i].sliceType);
		IMPEncoderPack *pack = &stream->pack[i];
		if(pack->length){
			uint32_t remSize = stream->streamSize - pack->offset;
			if(remSize < pack->length){
				memcpy(tmp, (void *)(stream->virAddr + pack->offset), remSize);
				memcpy(tmp+remSize, (void *)stream->virAddr, pack->length - remSize);

			}else {
				memcpy(tmp, (void *)(stream->virAddr + pack->offset), pack->length);
			}
		}
		frame->pts=frame->dts=pack->timestamp;
		
		if(isKeyframe){
			yang_put_be32((char*)tmp,pack->length-4);
			nb+=pack->length;
			frame->frametype=1;
			if ((*(tmp + 4) & kNalTypeMask) == YangAvcNaluTypeIDR) {
				frame->nb= nb;
				buf->putEVideo(&buf->mediaBuffer,frame);
				//printf("key frame->pts:%lld\n", frame->pts);
				yang_trace("\nkey pts:%lld:", frame->pts);
				for(int k=0;k<50;k++){
					yang_trace("%02x,",frame->payload[k]);
				}
			}
			tmp+=pack->length;

			
		}else{
			frame->nb= pack->length-4;
			frame->payload=tmp+4;
			frame->frametype=0;
			buf->putEVideo(&buf->mediaBuffer,frame);

		}
		
		
		
	}
	//yang_debug( "----------packCount=%d, stream->seq=%u end----------\n", stream->packCount, stream->seq);
	return 0;
}

void* yang_jzEncoder_start_thread(void *obj)
{
	int val, i, chnNum, ret;
	char stream_path[64];
	IMPEncoderEncType encType;
	int stream_fd = -1, totalSaveCnt = 0;
	YangEncoderSession* session=(YangEncoderSession*)obj;
	val = (int) (((chn[0].payloadType >> 24) << 16) | chn[i].index);
	chnNum = val & 0xffff;
	encType = (IMPEncoderEncType)((val >> 16) & 0xffff);
	ret = sample_framesource_streamon();
	ret = IMP_Encoder_StartRecvPic(chnNum);
	if (ret < 0) {
		yang_error( "IMP_Encoder_StartRecvPic(%d) failed\n", chnNum);
		return NULL;
	}


	totalSaveCnt = NR_FRAMES_TO_SAVE;
	YangFrame videoFrame;
	memset(&videoFrame,0,sizeof(YangFrame));
	uint8_t buffer[1024*1024]={0};
	videoFrame.payload=buffer;
	while (session->isConvert == 1) {
		ret = IMP_Encoder_PollingStream(chnNum, 1000);
		if (ret < 0) {
			yang_error( "IMP_Encoder_PollingStream(%d) timeout\n", chnNum);
			continue;
		}

		IMPEncoderStream stream;
		/* Get H264 or H265 Stream */
		ret = IMP_Encoder_GetStream(chnNum, &stream, 1);
#ifdef SHOW_FRM_BITRATE
		int i, len = 0;
		for (i = 0; i < stream.packCount; i++) {
			len += stream.pack[i].length;
		}
		bitrate_sp[chnNum] += len;
		frmrate_sp[chnNum]++;

		int64_t now = IMP_System_GetTimeStamp() / 1000;
		if(((int)(now - statime_sp[chnNum]) / 1000) >= FRM_BIT_RATE_TIME){
			double fps = (double)frmrate_sp[chnNum] / ((double)(now - statime_sp[chnNum]) / 1000);
			double kbr = (double)bitrate_sp[chnNum] * 8 / (double)(now - statime_sp[chnNum]);

			printf("streamNum[%d]:FPS: %0.2f,Bitrate: %0.2f(kbps)\n", chnNum, fps, kbr);
			//fflush(stdout);

			frmrate_sp[chnNum] = 0;
			bitrate_sp[chnNum] = 0;
			statime_sp[chnNum] = now;
		}
#endif
		if (ret < 0) {
			yang_error( "IMP_Encoder_GetStream(%d) failed\n", chnNum);
			return NULL;
		}


		ret = yang_jzEncoder_save_stream(session->out_videoBuffer,&videoFrame, &stream);
		if (ret < 0) {
			close(stream_fd);
			return NULL;
		}


		IMP_Encoder_ReleaseStream(chnNum, &stream);
	}

	close(stream_fd);

	ret = IMP_Encoder_StopRecvPic(chnNum);
	if (ret < 0) {
		yang_error( "IMP_Encoder_StopRecvPic(%d) failed\n", chnNum);
		return NULL;
	}
	sample_framesource_streamoff();
	return NULL;
}
void yang_jzEncoder_start(YangEncoderSession* session)  {
	if(session->isStart) return;
	session->isStart = 1;

	if (pthread_create(&session->threadId, 0, yang_jzEncoder_start_thread, session)) {
		yang_error("YangThread::start could not start thread");
	}
	session->isStart = 0;
}

void yang_jzEncoder_stop(YangEncoderSession* session) {
	session->isConvert = 0;

}
int32_t yang_jzEncoder_init(YangEncoderSession* session) {
	int32_t ret=0;
	/* Step.1 System init */
	ret = sample_system_init();
	if (ret < 0) {
		yang_error("IMP_System_Init() failed\n");
		return -1;
	}

	/* Step.2 FrameSource init */
	ret = sample_framesource_init();
	if (ret < 0) {
		yang_error("FrameSource init failed\n");
		return -1;
	}
	int32_t i=0;
	/* Step.3 Encoder init */
	//	for (i = 0; i < FS_CHN_NUM; i++) {
	if (chn[i].enable) {
		ret = IMP_Encoder_CreateGroup(chn[i].index);
		if (ret < 0) {
			yang_error("IMP_Encoder_CreateGroup(%d) error !\n", chn[i].index);
			return -1;
		}
	}
	//}

	ret = sample_encoder_init();
	if (ret < 0) {
		yang_error("Encoder init failed\n");
		return -1;
	}

	/* Step.4 Bind */
	//for (i = 0; i < FS_CHN_NUM; i++) {
	if (chn[i].enable) {
		ret = IMP_System_Bind(&chn[i].framesource_chn, &chn[i].imp_encoder);
		if (ret < 0) {
			yang_error( "Bind FrameSource channel%d and Encoder failed\n",i);
			return -1;
		}
	}
	//}

	return Yang_Ok;


}

void yang_create_videoEncoder(YangEncoderVideo* encoder){
	YangEncoderSession* session=&encoder->session;

	session->isStart = 0;
	session->isConvert = 1;

	session->out_videoBuffer = NULL;

	init_chn();
	encoder->init=yang_jzEncoder_init;
	encoder->start=yang_jzEncoder_start;
	encoder->stop=yang_jzEncoder_stop;
	encoder->sendMsgToEncoder=yang_jzEncoder_sendMsgToEncoder;

}
void yang_destroy_videoEncoder(YangEncoderVideo* encoder){
	YangEncoderSession* session=&encoder->session;
	if (session->isConvert) {
		yang_jzEncoder_stop(session);
			while (session->isStart) {
				yang_usleep(1000);
			}
		}

		session->out_videoBuffer = NULL;


		int32_t ret=0;
		/* Step.b UnBind */

		if (chn[0].enable) {
			ret = IMP_System_UnBind(&chn[0].framesource_chn, &chn[0].imp_encoder);
			if (ret < 0) {
				yang_error( "UnBind FrameSource channel0 and Encoder failed\n");

			}
		}

		/* Step.c Encoder exit */
		ret = sample_encoder_exit();
		if (ret < 0) {
			yang_error("Encoder exit failed\n");
			return;
		}

		/* Step.d FrameSource exit */
		ret = sample_framesource_exit();
		if (ret < 0) {
			yang_error("FrameSource exit failed\n");
			return;
		}

		/* Step.e System exit */
		ret = sample_system_exit();
		if (ret < 0) {
			yang_error("sample_system_exit() failed\n");
			return;
		}
}



