//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef INCLUDE_YANGPUSH_YANGPUSHCOMMON_H_
#define INCLUDE_YANGPUSH_YANGPUSHCOMMON_H_


#define Yang_VideoSrc_Camera 0
#define Yang_VideoSrc_Screen 1
#define Yang_VideoSrc_OutInterface 2
enum YangPushMessageType {
	YangM_Push_StartAudioCapture,
	YangM_Push_StartVideoCapture,
	YangM_Push_StartScreenCapture,
    YangM_Push_StartOutCapture,
	YangM_Push_Connect,
	YangM_Push_Disconnect,
	YangM_Push_Record_Start,
	YangM_Push_Record_Stop,
	YangM_Push_SwitchToCamera,
	YangM_Push_SwitchToScreen,
	YangM_Sys_Setvr,
	YangM_Sys_UnSetvr

};

#endif /* INCLUDE_YANGPUSH_YANGPUSHCOMMON_H_ */
