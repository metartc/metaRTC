//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGH264DEC_H_
#define SRC_YANGH264DEC_H_
#include <yangutil/yangavtype.h>

struct YangH264DecContext {
	int m_width, m_height, m_fps;
	int yLen;
	int uLen;
	int allLen;
	void *context;
};

struct YangH264DecContext* getYangH264DecContext();

void init_H264DecCont(struct YangH264DecContext *cont, uint8_t *headers,
		int headerLen);
int decode(struct YangH264DecContext *cont, int isIframe, unsigned char *pData,
		int nSize, enum YangYuvType pyuvType, unsigned char *dest,
		int *pnFrameReturned);
void decode_close(struct YangH264DecContext *cont);

#endif /* SRC_YANGH264DEC_H_ */
