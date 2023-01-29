//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangsdp/YangCodecPayload.h>


void yang_insert_fbsVector(YangFbsVector* vec,char* str){
	if(vec->vlen>=vec->capacity){
		char** tmp=(char**)yang_malloc(vec->vlen*sizeof(char*));
		yang_memcpy(tmp,vec->groups,vec->vlen*sizeof(char*));
		yang_free(vec->groups);
		vec->groups=(char**)yang_malloc((vec->capacity+10)*sizeof(char*));
		yang_memcpy(vec->groups,tmp,vec->vlen*sizeof(char*));
		yang_free(tmp);
		vec->capacity+=10;
	}
	vec->groups[vec->vlen]=(char*)yang_calloc(yang_strlen(str)+1,1);
	yang_strcpy(vec->groups[vec->vlen],str);
	vec->vlen++;
}
void yang_init_sdpVector(YangFbsVector* vec){
	vec->capacity=10;
	vec->groups=(char**)yang_calloc(vec->capacity*sizeof(char*),1);
	vec->vlen=0;
}
