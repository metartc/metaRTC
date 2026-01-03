//
// Copyright (c) 2019-2023 yanggaofeng
//

#ifndef SRC_YANGICE_YANGICEUTIL_H_
#define SRC_YANGICE_YANGICEUTIL_H_
#include <yangice/YangIce.h>
typedef struct{
	YangIpAddress address;
	yangbool response;
	YangRtcStun* stun;
	YangIceServer* server;
}YangIceStunRequest;
int32_t yang_ice_stun_request(YangIceSession* session,YangIceServer* server,int32_t localport);


#endif /* SRC_YANGICE_YANGICEUTIL_H_ */
