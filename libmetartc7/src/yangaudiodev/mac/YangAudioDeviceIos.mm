//
// Copyright (c) 2019-2025 yanggaofeng
//
#include "YangAudioDeviceIos.h"
#if Yang_OS_IOS

#include <yangutil/sys/YangLog.h>
#import <Foundation/Foundation.h>
#import <AudioToolbox/AudioToolbox.h>
#import <AVFAudio/AVAudioSession.h>

static int32_t yang_ios_audio_getDefaultDevice(YangIosAudio* audio){
	uint32_t flag=1;
	OSStatus status;
	AudioUnitElement busType=audio->isInput?1:0;
	AudioComponentDescription audioDesc;
	audioDesc.componentType = kAudioUnitType_Output;
	audioDesc.componentSubType = kAudioUnitSubType_RemoteIO;//kAudioUnitSubType_RemoteIO;//kAudioUnitSubType_VoiceProcessingIO;
	audioDesc.componentManufacturer = kAudioUnitManufacturer_Apple;
	audioDesc.componentFlags = 0;
	audioDesc.componentFlagsMask = 0;

	AudioComponent audioComponent = AudioComponentFindNext(NULL, &audioDesc);

	AudioComponentInstanceNew(audioComponent, &audio->audioUnit);

	status = AudioUnitSetProperty(audio->audioUnit,
			kAudioOutputUnitProperty_EnableIO,
			audio->isInput?kAudioUnitScope_Input:kAudioUnitScope_Output,
					busType,&flag,sizeof(flag));

	if(status!=noErr){
		if(audio->isInput)
			yang_error("ios input audio fail(%d)",(int32_t)status);
		else
			yang_error("ios output audio fail(%d)",(int32_t)status);
		return 1;
	}

	return Yang_Ok;
}


int32_t yang_ios_audio_init(YangIosAudio* audio,uint32_t sample,uint32_t channel)
{
	OSStatus status=noErr;
	AudioUnitElement busType=audio->isInput?1:0;
	AudioStreamBasicDescription desc;
	uint32_t size = sizeof(desc);
	int32_t tryTimes = 0;
	yang_memset(&desc,0,sizeof(desc));
    
    if(yang_ios_audio_getDefaultDevice(audio)!=Yang_Ok)
        return 1;

	if(audio->isInput){
		status=AudioUnitSetProperty(audio->audioUnit,
				kAudioOutputUnitProperty_SetInputCallback,
				kAudioUnitScope_Global, busType,
				&audio->callback, sizeof(audio->callback));	
	}else{

		status = AudioUnitSetProperty(
				audio->audioUnit, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Input,
				busType, &audio->callback, sizeof(audio->callback));
	}
	yang_memset(&desc,0,sizeof(desc));

	desc.mSampleRate = (Float64 )sample;
	desc.mFormatID   = kAudioFormatLinearPCM;
#if Yang_Enable_MacAudioFloat
    desc.mFormatFlags = kAudioFormatFlagIsFloat | kAudioFormatFlagIsPacked;
    desc.mFramesPerPacket = 1;
    desc.mChannelsPerFrame = channel;
    desc.mBitsPerChannel = 32;
    desc.mBytesPerPacket = desc.mBytesPerFrame = desc.mChannelsPerFrame * sizeof(float);
    desc.mBytesPerFrame = desc.mChannelsPerFrame * sizeof(float);
#else
    desc.mFormatFlags      = kLinearPCMFormatFlagIsSignedInteger|kLinearPCMFormatFlagIsPacked;
    desc.mFramesPerPacket = 1;
    desc.mChannelsPerFrame = channel;
    desc.mBitsPerChannel   = 16;
    desc.mBytesPerPacket = desc.mBytesPerFrame = desc.mChannelsPerFrame * sizeof(int16_t);
    desc.mBytesPerFrame = desc.mChannelsPerFrame * sizeof(int16_t);
#endif
	if(audio->isInput){
		status= AudioUnitSetProperty(audio->audioUnit, kAudioUnitProperty_StreamFormat,
				kAudioUnitScope_Output, busType, &desc, size);
	}else{
		status=AudioUnitSetProperty(audio->audioUnit, kAudioUnitProperty_StreamFormat,
				kAudioUnitScope_Input, busType, &desc, size);
	}

	if(status!=noErr){
		yang_error("get ios %s format failed(%d),sample=%d,channel=%d",audio->isInput?(char*)"capture":(char*)"render",(int)status,sample,channel);
		return 1;
	}

	status = AudioUnitInitialize(audio->audioUnit);
	while(status!=noErr){
		++tryTimes;
		if (tryTimes == 5)
			break;

		yang_usleep(100*1000);

		status = AudioUnitInitialize(audio->audioUnit);
	}

	if (status != noErr) {
		//kAudioUnitErr_FailedInitialization
		yang_error("ios init (%s) audio unit error(%d)", audio->isInput?(char*)"capture":(char*)"render",(int)status);
		return 1;
	}

	return Yang_Ok;
}

void yang_ios_audio_uninit(YangIosAudio* audio){
	AudioUnitUninitialize(audio->audioUnit);
	AudioComponentInstanceDispose(audio->audioUnit);
}


int32_t yang_ios_init_AudioSession(uint32_t *sample,int32_t audioType){
	AVAudioSession *audioSession = [AVAudioSession sharedInstance];
	NSError *error = nil;
    // 设置音频会话类别
    if(audioType==0){
        [audioSession setCategory:AVAudioSessionCategoryRecord error:&error];
    }else if(audioType==1){
        [audioSession setCategory:AVAudioSessionCategoryPlayback error:&error];
    }
   
    if (error) {
        NSLog(@"设置音频会话类别失败: %@", error);
        return 1;
    }
    
    // 激活音频会话
    [audioSession setActive:YES error:&error];
    if (error) {
        NSLog(@"激活音频会话失败: %@", error);
        return 1;
    }
    
    //*sample = audioSession.sampleRate;
	return Yang_Ok;
}

#endif
