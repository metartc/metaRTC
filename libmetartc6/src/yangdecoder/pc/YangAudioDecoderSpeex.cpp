//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangdecoder/pc/YangAudioDecoderSpeex.h>


void YangAudioDecoderSpeex::loadLib(){
	yang_speex_lib_get_mode=(const SpeexMode * (*) (int32_t mode))m_lib.loadFunction("speex_lib_get_mode");
	yang_speex_decoder_ctl=(int32_t (*)(void *state, int32_t request, void *ptr))m_lib.loadFunction("speex_decoder_ctl");
	yang_speex_decoder_init=(void* (*)(const SpeexMode *mode))m_lib.loadFunction("speex_decoder_init");
	yang_speex_bits_init=(void (*)(SpeexBits *bits))m_lib.loadFunction("speex_bits_init");
	yang_speex_bits_reset=(void (*)(SpeexBits *bits))m_lib.loadFunction("speex_bits_reset");
	yang_speex_bits_read_from=(int32_t (*)(SpeexBits *bits, const char *bytes, int32_t len))m_lib.loadFunction("speex_bits_read_from");
	yang_speex_decode_int=(int32_t (*)(void *state, SpeexBits *bits, spx_int16_t *out))m_lib.loadFunction("speex_decode_int");
	yang_speex_decoder_destroy=(void (*)(void *state))m_lib.loadFunction("speex_decoder_destroy");
	yang_speex_bits_destroy=(void (*)(SpeexBits *bits))m_lib.loadFunction("speex_bits_destroy");
}

void YangAudioDecoderSpeex::unloadLib(){
	yang_speex_lib_get_mode=NULL;
	yang_speex_decoder_init=NULL;
	yang_speex_bits_init=NULL;
	yang_speex_bits_reset=NULL;
	yang_speex_bits_read_from=NULL;
	yang_speex_decode_int=NULL;
	yang_speex_decoder_destroy=NULL;
	yang_speex_bits_destroy=NULL;
}

YangAudioDecoderSpeex::YangAudioDecoderSpeex(YangAudioParam *pcontext) {
	m_context=pcontext;

	m_channel = 1;

	isConvert = 0;
	m_frameSize=320;


	m_quality=8;
	m_state=NULL;
	ret=0;
	m_out=0;
	m_bits=NULL;
	unloadLib();

}

YangAudioDecoderSpeex::~YangAudioDecoderSpeex() {
	yang_deleteA(m_out);

	closedec();
	unloadLib();
	m_lib.unloadObject();

}
void YangAudioDecoderSpeex::initSpeexPara(){
	//if(m_mode==speex_nb_mode){
	//	m_quality=6;
	//	m_frameSize=160;
	//}
	//if(m_mode==speex_wb_mode){
			//m_quality=8;
			//m_frameSize=320;
	//	}
//	if(m_mode==speex_uwb_mode){
		//	m_quality=10;
			//m_frameSize=640;
	//	}
}
void YangAudioDecoderSpeex::init() {

	if(m_isInit) return;
	m_lib.loadObject("libspeex");
	loadLib();
	m_bits=new SpeexBits();
	m_state = yang_speex_decoder_init(yang_speex_lib_get_mode(SPEEX_MODEID_WB));//speex_wb_mode &speex_nb_mode);
	initSpeexPara();
	 m_quality=10;
	 yang_speex_decoder_ctl(m_state, SPEEX_SET_QUALITY, &m_quality);
	// int32_t tmp=1;
	 m_frameSize=320;
	   // speex_decoder_ctl(m_state, SPEEX_SET_ENH, &tmp);
	  yang_speex_bits_init(m_bits);
	   // callback.callback_id = SPEEX_INBAND_CHAR;
	  //  callback.func = speex_std_char_handler;
	  //  callback.data = stderr;
	  //  speex_decoder_ctl(m_state, SPEEX_SET_HANDLER, &callback);

	   // speex_decoder_ctl(m_state, SPEEX_GET_LOOKAHEAD, &tmp);


	  //  speex_decoder_ctl(m_state, SPEEX_GET_FRAME_SIZE, &m_frameSize);
	   // m_output=new float[m_frameSize];
	    m_out=new short[m_frameSize];
	    m_alen=640;
	m_isInit=1;

}

int32_t YangAudioDecoderSpeex::decode(YangFrame* pframe,YangDecoderCallback* pcallback){

if(!m_state) return 1;
      yang_speex_bits_read_from(m_bits, (const char*)pframe->payload, pframe->nb);
      ret=yang_speex_decode_int(m_state, m_bits, m_out);

      if(!ret&&pcallback){
    	  pframe->payload=(uint8_t*)m_out;
    	  pframe->nb=640;
     	 pcallback->onAudioData(pframe);
     	 return Yang_Ok;
      }
   return 1;
}

void YangAudioDecoderSpeex::closedec() {
	  if(m_state) yang_speex_decoder_destroy(m_state);
	  m_state=NULL;
	  if(m_bits) yang_speex_bits_destroy(m_bits);
	  m_bits=NULL;
}

