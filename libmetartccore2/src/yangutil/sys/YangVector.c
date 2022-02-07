//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangutil/sys/YangVector.h>
#include <yangutil/yangtype.h>
#include <string.h>
#include <stdlib.h>



void yang_insert_stringVector(YangStringVector* vec,char* str){
	if(vec==NULL||str==NULL) return;
	yang_insert_stringVector2(vec,str,strlen(str)+1);
}
void yang_insert_stringVector2(YangStringVector* vec,char* str,int plen){
	if(vec==NULL||str==NULL) return;
	if(vec->vsize>=vec->capacity){
		char** tmp=(char**)malloc((vec->capacity+5)*sizeof(char*));
		memcpy(tmp,vec->payload,vec->vsize*sizeof(char*));
		yang_free(vec->payload);
		vec->payload=tmp;
		vec->capacity+=5;
	}
	vec->payload[vec->vsize]=(char*)calloc(plen,1);
	memcpy(vec->payload[vec->vsize],str,plen);
	vec->vsize++;
}
void yang_init_stringVector(YangStringVector* vec){
	if(vec==NULL) return;
	vec->capacity=5;
	vec->payload=(char**)calloc(vec->capacity*sizeof(char*),1);
	vec->vsize=0;
}

void yang_destroy_stringVector(YangStringVector* vec){
	if(vec==NULL) return;
	for(int i=0;i<vec->vsize;i++){
		yang_free(vec->payload[i]);
	}
	vec->vsize=0;
	vec->capacity=0;
	yang_free(vec->payload);
}
void yang_clear_stringVector(YangStringVector* vec){
	if(vec==NULL) return;
	memset(vec->payload,0,sizeof(char*)*vec->capacity);
	vec->vsize=0;
}
