//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangsdp/YangCodecPayload.h>
#include <string.h>
#include <yangutil/yangtype.h>
void yang_insert_fbsVector(YangFbsVector* vec,char* str){
	if(vec->vlen>=vec->capacity){
		char** tmp=(char**)malloc(vec->vlen*sizeof(char*));
		memcpy(tmp,vec->groups,vec->vlen*sizeof(char*));
		yang_free(vec->groups);
		vec->groups=(char**)malloc((vec->capacity+10)*sizeof(char*));
		memcpy(vec->groups,tmp,vec->vlen*sizeof(char*));
		yang_free(tmp);
		vec->capacity+=10;
	}
	vec->groups[vec->vlen]=(char*)calloc(strlen(str)+1,1);
	strcpy(vec->groups[vec->vlen],str);
	vec->vlen++;
}
void yang_init_sdpVector(YangFbsVector* vec){
	vec->capacity=10;
	vec->groups=(char**)calloc(vec->capacity*sizeof(char*),1);
	vec->vlen=0;
}
