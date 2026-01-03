//
// Copyright (c) 2019-2023 yanggaofeng
//

#ifndef SRC_YANGICE_YANGICEDEF_H_
#define SRC_YANGICE_YANGICEDEF_H_
#include <yangutil/sys/YangSocket.h>
typedef struct{
	yangbool stunRequestOk;
	YangIpFamilyType familyType;
	int32_t serverPort;

	YangIpAddress stunAddress;
	YangIpAddress turnAddress;

	char username[64];
	char password[64];
	char serverIp[64];
}YangIceServer;



#endif /* SRC_YANGICE_YANGICEDEF_H_ */
