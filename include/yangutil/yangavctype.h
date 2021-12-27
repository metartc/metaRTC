/*
 * yangavctype.h
 *
 *  Created on: 2021年12月26日
 *      Author: yang
 */

#ifndef INCLUDE_YANGUTIL_YANGAVCTYPE_H_
#define INCLUDE_YANGUTIL_YANGAVCTYPE_H_
#include <stdint.h>
struct YangFrame{
	int32_t mediaType;
	int32_t uid;
	int32_t frametype;
	int32_t nb;
	int64_t timestamp;
	uint8_t* payload;
};

void yang_frame_copy( struct YangFrame* src,struct YangFrame* dst);
void yang_frame_copy_buffer(struct YangFrame* src,struct YangFrame* dst);
void yang_frame_copy_nobuffer(struct YangFrame* src,struct YangFrame* dst);



#endif /* INCLUDE_YANGUTIL_YANGAVCTYPE_H_ */
