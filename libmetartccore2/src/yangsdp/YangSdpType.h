/*
 * YangSdpType.h
 *
 *  Created on: 2022年1月14日
 *      Author: yang
 */

#ifndef SRC_YANGSDP_YANGSDPTYPE_H_
#define SRC_YANGSDP_YANGSDPTYPE_H_
#include <stdint.h>
#include <yangutil/buffer/YangBuffer.h>
typedef struct{
	int32_t capacity;
	int32_t vlen;
	char** payload;
}YangStrVector;

#define yangsdpint uint32_t



void yang_insert_strVector(YangStrVector* vec,char* str);
void yang_init_strVector(YangStrVector* vec);
void yang_destroy_strVector(YangStrVector* vec);
void yang_clear_strVector(YangStrVector* vec);

#define yang_declare_struct(x) \
typedef struct{ \
	int32_t capacity; \
	int32_t vlen; \
	x* payload; \
}x##Vector; \

#define yang_declare_struct_init(x) \
void yang_init_##x##Vector(x##Vector* vec);\

#define yang_declare_struct_destroy(x) \
void yang_destroy_##x##Vector(x##Vector* vec);\

#define yang_declare_struct_insert(x) \
void yang_insert_##x##Vector(x##Vector* vec,x* value);\

#define yang_impl_struct_insert(x) \
void yang_insert_##x##Vector(x##Vector* vec,x* value){ \
	if(vec==NULL) return;\
	if(vec->vlen>=vec->capacity){\
		x* tmp=(x*)calloc(sizeof(x),1);\
		memcpy(tmp,vec->payload,sizeof(x)*vec->vlen);\
		yang_free(vec->payload);\
		vec->payload=(x*)calloc(sizeof(x)*(vec->capacity+10),1);\
		vec->capacity+=10;\
		memcpy(vec->payload,tmp,sizeof(x)*vec->vlen);\
		yang_free(tmp);\
	}\
	memcpy(&vec->payload[vec->vlen++],value,sizeof(x));\
}\


#define yang_impl_struct_init(x) \
void yang_init_##x##Vector(x##Vector* vec){\
	vec->capacity=10;\
	vec->payload=(x*)calloc(vec->capacity*sizeof(x),1);\
	vec->vlen=0;\
}\

#define yang_impl_struct_destroy(x) \
void yang_destroy_##x##Vector(x##Vector* vec){\
	memset(vec->payload,0,sizeof(x)*vec->capacity);\
	vec->vlen=0;\
}\

#endif /* SRC_YANGSDP_YANGSDPTYPE_H_ */
