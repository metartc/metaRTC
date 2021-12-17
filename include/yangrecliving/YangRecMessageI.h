#ifndef INCLUDE_YANGRECLIVING_YANGRECMESSAGEI_H_
#define INCLUDE_YANGRECLIVING_YANGRECMESSAGEI_H_
#include "yangutil/sys/YangSysMessageI.h"
enum YangRecMessageType {
	YangM_Rec_Shutdown,
	YangM_Rec_ConnectServer,
	YangM_Rec_DisconnectServer,
	YangM_Rec_ConnectServerInterrupt,

	YangM_Rec_PushMediaServerConnect,
	YangM_Rec_PlayMediaServerConnect,
	YangM_Rec_PushMediaServerError,
	YangM_Rec_PlayMediaServerError,

	YangM_Rec_Start,
	YangM_Rec_Stop,

	YangM_Rec_Setvr,
	YangM_Rec_UnSetvr,
	YangM_Rec_Pubaudio,
	YangM_Rec_Pubvideo,
	YangM_Rec_UnPubaudio,
	YangM_Rec_UnPubvideo,
	YangM_Rec_Film_Start,
	YangM_Rec_Film_Stop,
	YangM_Rec_Pg_Start,
	YangM_Rec_pg_Stop,
	YangM_Rec_Screen_Start,
	YangM_Rec_Screen_Stop
};




#endif /* INCLUDE_YANGRECLIVING_YANGRECMESSAGEI_H_ */
