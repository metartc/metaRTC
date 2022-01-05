#include <yangavutil/audio/YangSwResample.h>
#include <yangutil/sys/YangLog.h>
YangSwResample::YangSwResample()
{
    swr_ctx = NULL;
    m_swrData=NULL;
	m_channel=2;
	m_inSample=48000;
	m_outSample=44100;
	m_frameSize=441;
	m_contextt=0;

}
YangSwResample::~YangSwResample()
{
    if (swr_ctx) {
        swr_free(&swr_ctx);
    }
    if (m_swrData) {
         av_freep(&m_swrData[0]);
         free(m_swrData);
         m_swrData = NULL;
     }


}
int YangSwResample::init(int32_t pchannel,int32_t pinsample,int32_t poutsample,int32_t pframeSize){
	if(m_contextt) return Yang_Ok;
    m_inSample=pinsample;
    m_outSample=poutsample;
    m_channel=pchannel;
    m_frameSize=pframeSize;

        swr_ctx = swr_alloc_set_opts(NULL, AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_S16, m_inSample,
        		AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_S16, m_outSample, 0, NULL);
        if (!swr_ctx) {
             return yang_error_wrap(1, "YangSwResample fail to swr_alloc\n");

         }
        int error;
        char err_buf[AV_ERROR_MAX_STRING_SIZE] = {0};
        if ((error = swr_init(swr_ctx)) < 0) {
             return yang_error_wrap(1,"open swr(%d:%s)", error, av_make_error_string(err_buf, AV_ERROR_MAX_STRING_SIZE, error));
        }

        if (!(m_swrData = (uint8_t **)calloc(m_channel, sizeof(*m_swrData)))) {
              return yang_error_wrap(1, "alloc swr buffer");
          }

        if ((error = av_samples_alloc(m_swrData, NULL, m_channel, m_frameSize, AV_SAMPLE_FMT_S16, 0)) < 0) {
               return yang_error_wrap(1, "alloc swr buffer(%d:%s)", error,
                   av_make_error_string(err_buf, AV_ERROR_MAX_STRING_SIZE, error));
           }
        m_contextt=1;
        return Yang_Ok;

}

void YangSwResample::resample(const uint8_t *pin,uint32_t  pinLen,uint8_t* pout,uint32_t  *poutLen){
	int frame_size = swr_convert(swr_ctx, m_swrData, m_frameSize, &pin, pinLen);
	*poutLen=frame_size;
	memcpy(pout,*m_swrData,frame_size<<2);

}
