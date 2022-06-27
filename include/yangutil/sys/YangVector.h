//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef INCLUDE_YANGUTIL_SYS_YANGVECTOR_H_
#define INCLUDE_YANGUTIL_SYS_YANGVECTOR_H_
#include <yangutil/buffer/YangBuffer.h>
#include <stdint.h>
#include <string.h>

typedef struct{
	int32_t capacity;
	int32_t vsize;
	char** payload;
}YangStringVector;

typedef struct{
	 uint32_t vsize;
	 uint32_t capacity;
	 uint16_t* payload;
}YangUint16Sort;



void yang_insert_uint16Sort(YangUint16Sort* psort,uint16_t val);
void yang_create_uint16Sort(YangUint16Sort* psort);
void yang_create_uint16Sort2(YangUint16Sort* psort,int32_t pcapacity);
void yang_destroy_uint16Sort(YangUint16Sort* psort);
void yang_clear_uint16Sort(YangUint16Sort* psort);
void yang_clear_uint16Sort2(YangUint16Sort* psort,int32_t index);

void yang_insert_stringVector(YangStringVector* vec,char* str);
void yang_insert_stringVector2(YangStringVector* vec,char* str,int plen);
void yang_create_stringVector(YangStringVector* vec);
void yang_destroy_stringVector(YangStringVector* vec);
void yang_clear_stringVector(YangStringVector* vec);

#define yang_vector_declare(x) \
typedef struct{ \
	int32_t capacity; \
	int32_t vsize; \
	x* payload; \
}x##Vector; \
void yang_create_##x##Vector(x##Vector* vec);\
void yang_destroy_##x##Vector(x##Vector* vec);\
void yang_clear_##x##Vector(x##Vector* vec);\
void yang_insert_##x##Vector(x##Vector* vec,x* value);\
void yang_remove_##x##Vector(x##Vector* vec,int32_t index);\


#define yang_vector_impl(x) \
void yang_insert_##x##Vector(x##Vector* vec,x* value){ \
	if(vec==NULL) return;\
	if(vec->vsize>=vec->capacity){\
		x* tmp=(x*)calloc(sizeof(x)*(vec->capacity+5),1);\
		memcpy(tmp,vec->payload,sizeof(x)*vec->vsize);\
		yang_free(vec->payload);\
		vec->payload=tmp;\
		vec->capacity+=5;\
	}\
	if(value)\
		memcpy(&vec->payload[vec->vsize++],value,sizeof(x));\
	else\
		memset(&vec->payload[vec->vsize++],0,sizeof(x));\
}\
void yang_create_##x##Vector(x##Vector* vec){\
	vec->capacity=5;\
	vec->payload=(x*)calloc(vec->capacity*sizeof(x),1);\
	vec->vsize=0;\
}\
void yang_destroy_##x##Vector(x##Vector* vec){\
	vec->vsize=0;\
	vec->capacity=0;\
	yang_free(vec->payload);\
}\
void yang_clear_##x##Vector(x##Vector* vec){\
	memset(vec->payload,0,vec->capacity*sizeof(x));\
	vec->vsize=0;\
}\
void yang_remove_##x##Vector(x##Vector* vec,int32_t index){\
	if(vec==NULL||vec->vsize==0||index>=vec->vsize) return;\
	if(vec->vsize==1) {yang_clear_##x##Vector(vec);return;}\
	memmove(vec->payload+index*sizeof(x),vec->payload+(index+1)*sizeof(x),sizeof(x)*(vec->vsize-index-1));\
	vec->vsize--;\
}\



#endif /* INCLUDE_YANGUTIL_SYS_YANGVECTOR_H_ */
