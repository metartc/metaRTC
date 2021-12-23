
#ifndef _WIN32
#include <alsa/asoundlib.h>
#endif
#include <yangaudiodev/YangAudioDeviceQuery.h>
#include "yangutil/sys/YangLog.h"
YangAudioDeviceQuery::YangAudioDeviceQuery() {


}

YangAudioDeviceQuery::~YangAudioDeviceQuery() {

}


vector<YangAudioDeivce>* YangAudioDeviceQuery::getCaptureDeviceList(){
    #ifndef _WIN32
	getDeviceList((int)SND_PCM_STREAM_CAPTURE,&m_captureDeviceList);
    #endif
	return &m_captureDeviceList;
}
vector<YangAudioDeivce>* YangAudioDeviceQuery::getPlayDeviceList(){
    #ifndef _WIN32
	getDeviceList((int)SND_PCM_STREAM_PLAYBACK,&m_playDeviceList);
    #endif
	return &m_playDeviceList;
}
#ifndef _WIN32
void YangAudioDeviceQuery::getDeviceList(int32_t pstream,vector<YangAudioDeivce>* plist) {
	snd_pcm_stream_t stream=(snd_pcm_stream_t)pstream;
	snd_ctl_t *handle;
	int32_t card, err, dev, idx;
	snd_ctl_card_info_t *info;
	snd_pcm_info_t *pcminfo;
	snd_ctl_card_info_alloca(&info);
	snd_pcm_info_alloca(&pcminfo);
	card = -1;
	int32_t aindex=0;
	while (1) {
		if (snd_card_next(&card) < 0 || card < 0) {
			yang_error("no soundcards found...");
			return;
		}

		//printf("**** List of %s Hardware Devices ****\n",snd_pcm_stream_name(stream));
		char name[32];
		sprintf(name, "hw:%d", card);
		if ((err = snd_ctl_open(&handle, name, 0)) < 0) {
			yang_error("control open (%i): %s", card, snd_strerror(err));
			return;
		}
		if ((err = snd_ctl_card_info(handle, info)) < 0) {
			yang_error("control hardware info (%i): %s", card,
					snd_strerror(err));
			snd_ctl_close(handle);
			return;
		}

		dev = -1;
		while (1) {
			uint32_t  count;
			if (snd_ctl_pcm_next_device(handle, &dev) < 0)
				yang_error("\nsnd_ctl_pcm_next_device");
			if (dev < 0)
				break;
			snd_pcm_info_set_device(pcminfo, dev);
			snd_pcm_info_set_subdevice(pcminfo, 0);
			snd_pcm_info_set_stream(pcminfo, stream);
			if ((err = snd_ctl_pcm_info(handle, pcminfo)) < 0) {
				if (err != -ENOENT)
					yang_error("\ncontrol digital audio info (%i): %s", card,
							snd_strerror(err));
				continue;
			}
			//printf("card %i: %s [%s], device %i: %s [%s]\n", card,
			//		snd_ctl_card_info_get_id(info),
			//		snd_ctl_card_info_get_name(info), dev,
			//		snd_pcm_info_get_id(pcminfo),
			//		snd_pcm_info_get_name(pcminfo));
			count = snd_pcm_info_get_subdevices_count(pcminfo);
			//printf("  Subdevices: %i/%i\n",snd_pcm_info_get_subdevices_avail(pcminfo), count);
			for (idx = 0; idx < (int) count; idx++) {
				snd_pcm_info_set_subdevice(pcminfo, idx);
				if ((err = snd_ctl_pcm_info(handle, pcminfo)) < 0) {
					yang_error("control digital audio playback info (%i): %s",
							card, snd_strerror(err));
				} else {
					plist->push_back(YangAudioDeivce());
					char sn[64]={0};
					sprintf(sn,"%s [%s]",snd_ctl_card_info_get_id(info),snd_ctl_card_info_get_name(info));
					plist->back().name=string(sn);
					char sn1[64]={0};
					sprintf(sn1,"%s [%s]",snd_pcm_info_get_id(pcminfo),snd_pcm_info_get_name(pcminfo));
					plist->back().deviceName=string(sn1);
					plist->back().subName=string(snd_pcm_info_get_subdevice_name(pcminfo));
					plist->back().aIndex=aindex;
					plist->back().aSubIndex=dev;
					plist->back().aIdx=idx;
					//printf("  Subdevice #%i: %s\n", idx,snd_pcm_info_get_subdevice_name(pcminfo));
				}
			}
		}
		snd_ctl_close(handle);
		aindex++;
	}

}
#endif
