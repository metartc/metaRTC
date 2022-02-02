/*
 * YangVector.c
 *
 *  Created on: 2022年1月29日
 *      Author: yang
 */
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
	if(vec->vlen>=vec->capacity){
		char** tmp=(char**)malloc(vec->vlen*sizeof(char*));
		memcpy(tmp,vec->payload,vec->vlen*sizeof(char*));
		yang_free(vec->payload);
		vec->payload=(char**)malloc((vec->capacity+10)*sizeof(char*));
		memcpy(vec->payload,tmp,vec->vlen*sizeof(char*));
		yang_free(tmp);
		vec->capacity+=10;
	}
	vec->payload[vec->vlen]=(char*)calloc(plen,1);
	memcpy(vec->payload[vec->vlen],str,plen);
	vec->vlen++;
}
void yang_init_stringVector(YangStringVector* vec){
	if(vec==NULL) return;
	vec->capacity=10;
	vec->payload=(char**)calloc(vec->capacity*sizeof(char*),1);
	vec->vlen=0;
}

void yang_destroy_stringVector(YangStringVector* vec){
	if(vec==NULL) return;
	for(int i=0;i<vec->vlen;i++){
		yang_free(vec->payload[i]);
	}
	vec->vlen=0;
	vec->capacity=0;
	yang_free(vec->payload);
}
void yang_clear_stringVector(YangStringVector* vec){
	if(vec==NULL) return;
	memset(vec->payload,0,sizeof(char*)*vec->capacity);
	vec->vlen=0;
}
