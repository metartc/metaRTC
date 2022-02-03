/*
 * YangVector.h
 *
 *  Created on: 2022年1月29日
 *      Author: yang
 */

#ifndef INCLUDE_YANGUTIL_SYS_YANGVECTOR_H_
#define INCLUDE_YANGUTIL_SYS_YANGVECTOR_H_

#include <stdint.h>
#include <yangutil/buffer/YangBuffer.h>
typedef struct{
	int32_t capacity;
	int32_t vsize;
	char** payload;
}YangStringVector;




void yang_insert_stringVector(YangStringVector* vec,char* str);
void yang_insert_stringVector2(YangStringVector* vec,char* str,int plen);
void yang_init_stringVector(YangStringVector* vec);
void yang_destroy_stringVector(YangStringVector* vec);
void yang_clear_stringVector(YangStringVector* vec);

#define yang_vector_declare(x) \
typedef struct{ \
	int32_t capacity; \
	int32_t vsize; \
	x* payload; \
}x##Vector; \
void yang_init_##x##Vector(x##Vector* vec);\
void yang_destroy_##x##Vector(x##Vector* vec);\
void yang_clear_##x##Vector(x##Vector* vec);\
void yang_insert_##x##Vector(x##Vector* vec,x* value);\



#define yang_vector_impl(x) \
void yang_insert_##x##Vector(x##Vector* vec,x* value){ \
	if(vec==NULL) return;\
	if(vec->vsize>=vec->capacity){\
		x* tmp=(x*)calloc(sizeof(x),1);\
		memcpy(tmp,vec->payload,sizeof(x)*vec->vsize);\
		yang_free(vec->payload);\
		vec->payload=(x*)calloc(sizeof(x)*(vec->capacity+10),1);\
		vec->capacity+=10;\
		memcpy(vec->payload,tmp,sizeof(x)*vec->vsize);\
		yang_free(tmp);\
	}\
	if(value)\
		memcpy(&vec->payload[vec->vsize++],value,sizeof(x));\
	else\
		memset(&vec->payload[vec->vsize++],0,sizeof(x));\
}\
void yang_init_##x##Vector(x##Vector* vec){\
	vec->capacity=10;\
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




#endif /* INCLUDE_YANGUTIL_SYS_YANGVECTOR_H_ */
