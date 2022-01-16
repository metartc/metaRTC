/*
 * yangavctype.h
 *
 *  Created on: 2021年12月26日
 *      Author: yang
 */

#ifndef INCLUDE_YANGUTIL_YANGAVCTYPE_H_
#define INCLUDE_YANGUTIL_YANGAVCTYPE_H_
#include <stdint.h>
typedef struct{
	int32_t mediaType;
	int32_t uid;
	int32_t frametype;
	int32_t nb;
	int64_t pts;
	int64_t dts;
	uint8_t* payload;
}YangFrame;

void yang_frame_copy(YangFrame* src, YangFrame* dst);
void yang_frame_copy_buffer(YangFrame* src, YangFrame* dst);
void yang_frame_copy_nobuffer(YangFrame* src, YangFrame* dst);



#endif /* INCLUDE_YANGUTIL_YANGAVCTYPE_H_ */
