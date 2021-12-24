/*
 * YangStreamCapture.cpp
 *
 *  Created on: 2021年10月2日
 *      Author: yang
 */

#include <yangstream/YangStreamCapture.h>

YangStreamCapture::YangStreamCapture() {


}

YangStreamCapture::~YangStreamCapture() {

}



uint8_t* YangStreamCapture::getVideoData(){
	return m_video.getVideoData();
}
int32_t YangStreamCapture::getVideoLen(){
	return m_video.getVideoLen();
}
int32_t YangStreamCapture::getVideoFrametype(){
	return m_video.getFrametype();
}
int64_t YangStreamCapture::getVideoTimestamp(){
	return m_video.getTimestamp();
}


uint8_t* YangStreamCapture::getAudioData(){
	return m_audio.getAudioData();
}
int32_t YangStreamCapture::getAudioLen(){
	return m_audio.getAudioLen();
}
int64_t YangStreamCapture::getAudioTimestamp(){
	return m_audio.getTimestamp();
}
YangAudioEncDecType YangStreamCapture::getAudioType(){
	return m_audio.getAudioType();
}


void YangStreamCapture::initVideo(int32_t transtype){
	m_video.init(transtype);
}
void YangStreamCapture::setVideoData(YangFrame* videoFrame,YangVideoEncDecType videoType){
	m_video.setVideoData(videoFrame,videoType);
}
void YangStreamCapture::setVideoMeta(uint8_t* p,int32_t plen,YangVideoEncDecType videoType){
	m_video.setVideoMeta(p,plen,videoType);
}
void YangStreamCapture::setVideoFrametype(int32_t frametype){
	m_video.setFrametype(frametype);
}
void YangStreamCapture::setMetaTimestamp(int64_t timestamp){
	m_video.setMetaTimestamp(timestamp);
}

void YangStreamCapture::setAudioData(YangFrame* audioFrame){
	m_audio.setAudioData(audioFrame);
}
void YangStreamCapture::setAudioMetaData(uint8_t* p,int32_t plen){
	m_audio.setAudioMetaData(p,plen);
}
void YangStreamCapture::setAudioFrametype(int32_t frametype){
	m_audio.setFrametype(frametype);
}

void YangStreamCapture::initAudio(int32_t transType,int32_t sample,int32_t channel,YangAudioEncDecType audioType){
	m_audio.init(transType,sample,channel,audioType);
}
