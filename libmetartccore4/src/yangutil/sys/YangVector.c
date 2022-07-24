//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangutil/sys/YangVector.h>
#include <yangutil/yangtype.h>
#include <yangutil/sys/YangMath.h>
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
void yang_create_stringVector(YangStringVector* vec){
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


void yang_insert_uint16Sort(YangUint16Sort* psort,uint16_t val){
	if (psort->vsize >= psort->capacity - 1) {
		uint16_t *tmp = (uint16_t*) calloc((psort->capacity + 50) * sizeof(uint16_t), 1);
		memcpy(tmp, psort->payload, psort->vsize * sizeof(uint16_t));
		yang_free(psort->payload);
		psort->payload = tmp;
		psort->capacity += 50;
	}
	yang_insert_uint16_sort(psort->payload, val, &psort->vsize);
}
void yang_create_uint16Sort(YangUint16Sort* psort){
	yang_create_uint16Sort2(psort,50);
}
void yang_create_uint16Sort2(YangUint16Sort* psort,int32_t pcapacity){
	if(psort==NULL) return;
	psort->capacity = pcapacity;
	if (psort->payload == NULL)		psort->payload = (uint16_t*) calloc(sizeof(uint16_t) * psort->capacity,1);
}
void yang_destroy_uint16Sort(YangUint16Sort* psort){
	if(psort==NULL) return;
	yang_free(psort->payload);
	psort->capacity=0;
	psort->vsize=0;
}
void yang_clear_uint16Sort(YangUint16Sort* psort){
	psort->vsize=0;
}

void yang_clear_uint16Sort2(YangUint16Sort* psort,int32_t index){
	int32_t vsize=psort->vsize-index-1;
	memmove((char*)psort->payload,(char*)psort->payload+index*sizeof(uint16_t),vsize*sizeof(uint16_t));
	psort->vsize=vsize;
}
