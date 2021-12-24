#ifndef SRC_YANGH264DEC_H_
#define SRC_YANGH264DEC_H_
#include <stdint.h>
struct YangH264DecContext {
	int32_t m_width, m_height, m_fps;
	int32_t yLen;
	int32_t uLen;
	int32_t allLen;
	void *context;
};

struct YangH264DecContext* getYangH264DecContext();
void init_H264DecCont(struct YangH264DecContext *cont, uint8_t *headers,
		int32_t headerLen);
int32_t decode(struct YangH264DecContext *cont, int32_t isIframe, uint8_t *pData,
		int32_t nSize, uint8_t *dest, int32_t *pnFrameReturned);
void decode_close(struct YangH264DecContext *cont);

#endif /* SRC_YANGH264DEC_H_ */
