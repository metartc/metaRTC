/*
 * YangYuvUtil.cpp
 *
 *  Created on:  2019年8月25日
 *      Author: yang
 */

#include "yangavutil/video/YangYuvUtil.h"
#include "stdio.h"
#include "memory.h"


void yang_plusNV12(uint8_t *src, uint8_t *dest, int32_t model, int32_t srcWidth,
		int32_t srcHeight, int32_t destWidth, int32_t destHeight) {
	int32_t srcLen = srcWidth * srcHeight, destLen = destWidth * destHeight;
	uint8_t *temp, *srcTmp, *dstTmp;
	if (model == 4) {
		int32_t start4 = (srcHeight - destHeight + 1) * srcWidth - destWidth;
		int32_t start41 = (srcHeight / 2 - destHeight / 2) * srcWidth
				+ (srcWidth - destWidth);
		temp = src + start4;
		srcTmp = src + srcLen + start41;
	} else if (model == 3) {
		int32_t start3 = srcWidth * (srcHeight - destHeight);
		int32_t start31 = (srcHeight / 2 - destHeight / 2) * srcWidth;
		temp = src + start3;
		srcTmp = src + srcLen + start31;
	} else if (model == 2) {
		int32_t start2 = srcWidth - destWidth;
		temp = src + start2;
		srcTmp = src + srcLen + srcWidth - destWidth;
	} else if (model == 1) {
		temp = src;
		srcTmp = src + srcLen;
	}
	dstTmp = dest + destLen;
	for (int32_t i = 0; i < destHeight; i += 2) {
		memcpy(temp + srcWidth * i, dest + i * destWidth, destWidth);
		memcpy(temp + srcWidth * (i + 1), dest + (i + 1) * destWidth,
				destWidth);
		memcpy(srcTmp + i * srcWidth / 2, dstTmp + i * destWidth / 2,
				destWidth);
	}
	temp=NULL;
	srcTmp=NULL;
	dstTmp=NULL;
}

void yang_plusYuy2(uint8_t *src, uint8_t *dest, int32_t model, int32_t srcWidth,
		int32_t srcHeight, int32_t destWidth, int32_t destHeight) {
	//int32_t srcLen = srcWidth * srcHeight, destLen = destWidth * destHeight;
	uint8_t *temp;//, *srcTmp, *dstTmp;
	if (model == 4) {
		int32_t start4 = (srcHeight - destHeight + 1) * srcWidth * 2 - destWidth * 2;
		temp = src + start4;
		//srcTmp=src+srcLen+start41;
	} else if (model == 3) {
		int32_t start3 = srcWidth * (srcHeight - destHeight);
		//int32_t start31 = (srcHeight / 2 - destHeight / 2) * srcWidth;
		temp = src + start3;
	//	srcTmp = src + srcLen + start31;
	} else if (model == 2) {
		int32_t start2 = srcWidth - destWidth;
		temp = src + start2;
	//	srcTmp = src + srcLen + srcWidth - destWidth;
	} else if (model == 1) {
		temp = src;
	//	srcTmp = src + srcLen;
	}
	//dstTmp = dest + destLen;
	for (int32_t i = 0; i < destHeight; i += 1) {
		memcpy(temp + srcWidth * i * 2, dest + i * destWidth * 2,
				destWidth * 2);
		//memcpy(temp+srcWidth*(i+1),dest+(i+1)*destWidth,destWidth);
		//memcpy(srcTmp+i*srcWidth/2,dstTmp+i*destWidth/2,destWidth);
	}
	temp=NULL;
	//srcTmp=NULL;

	//dstTmp=NULL;

}

void yang_zoom4(uint8_t* src,uint8_t* dest,int32_t srcWidth,int32_t hei){

	int32_t srcLen=srcWidth*hei;
	int32_t destWidth=srcWidth/4;
	int32_t destHeight=hei/4;
	int32_t dstLen=destWidth*destHeight;
	uint8_t* srcU=src+srcLen;
	uint8_t* srcV=src+srcLen*5/4;
	uint8_t* dstU=dest+dstLen;
	uint8_t* dstV=dest+dstLen*5/4;
	/**
	for(int32_t i=0;i<dstHei;i++){
		*(dst+i*dstWidth+i)=*(src+i*4*wid+i*4);
	}
	for(int32_t i=0;i<dstHei/2;i++){
		*(dstU+dstWidth*i/2+i)=*(srcU+wid*4*i/2+4*i);
		*(dstV+dstWidth*i/2+i)=*(srcV+wid*4*i/2+4*i);
	}**/
	for(int32_t i=0;i<destHeight;i+=2){
		for(int32_t j=0;j<destWidth;j+=2){
			*(dest+i*destWidth+j)=*(src+4*i*srcWidth+4*j);
			*(dest+i*destWidth+j+1)=*(src+4*i*srcWidth+4*j+1);
			*(dest+(i+1)*destWidth+j)=*(src+(4*i+1)*srcWidth+4*j);
			*(dest+(i+1)*destWidth+j+1)=*(src+(4*i+1)*srcWidth+4*j+1);
                        *(dstU+(i/2)*destWidth/2+j/2)=*(srcU+2*i*srcWidth+2*j);
                        *(dstV+(i/2)*destWidth/2+j/2)=*(srcV+2*i*srcWidth+2*j);
		}
	}
 srcU=NULL;
	srcV=NULL;
	dstU=NULL;
	dstV=NULL;
}
void zoomIn4(uint8_t *src,uint8_t *dest,int32_t srcWidth,int32_t srcHeight){
	int32_t srcLen=srcWidth*srcHeight;
	int32_t destWidth=srcWidth/4;
	int32_t destHeight=srcHeight/4;
	int32_t destLen=destWidth*destHeight;
	uint8_t* srcTmp=src+srcLen;
	uint8_t* dstTmp=dest+destLen;
	for(int32_t i=0;i<destHeight;i+=2){
		for(int32_t j=0;j<destWidth;j+=2){
			*(dest+i*destWidth+j)=*(src+4*i*srcWidth+4*j);
			*(dest+i*destWidth+j+1)=*(src+4*i*srcWidth+4*j+1);
			*(dest+(i+1)*destWidth+j)=*(src+(4*i+1)*srcWidth+4*j);
			*(dest+(i+1)*destWidth+j+1)=*(src+(4*i+1)*srcWidth+4*j+1);
			*(dstTmp+i*destWidth/2+j)=*(srcTmp+2*i*srcWidth+4*j);
			*(dstTmp+i*destWidth/2+j+1)=*(srcTmp+2*i*srcWidth+4*j+1);
		}
	}

}
void yang_plusI420(uint8_t *src, uint8_t *dest, int32_t model, int32_t srcWidth,
	int32_t srcHeight, int32_t destWidth, int32_t destHeight) {
	int32_t srcLen = srcWidth * srcHeight;
	int32_t destLen = destWidth * destHeight;
	uint8_t *temp, *srcTmp, *srcTmp1, *dstTmp, *dstTmp1;
	uint8_t *srcU=src+srcLen;
	uint8_t *srcV=src+srcLen+srcLen/4;
	//uint8_t *dstU=dest+destLen;
	//uint8_t *dstV=dest+destLen+destLen/4;
	if (model == 4) {
		int32_t start4 = (srcHeight - destHeight) * srcWidth + (srcWidth-destWidth);
		int32_t start41=(srcHeight/2-destHeight/2)*srcWidth/2+srcWidth/2-destWidth/2;//start4/4;
		//int32_t start41 = (srcHeight / 4 - destHeight / 4) * srcWidth+ (srcWidth - destWidth);
		//int32_t start42 = (srcHeight / 4 - destHeight / 4) * srcWidth+ (srcWidth - destWidth);
		temp = src + start4;
		srcTmp = srcU + start41;
		srcTmp1 = srcV + start41;
	} else if (model == 3) {
		int32_t start3 = srcWidth * (srcHeight - destHeight);
		int32_t start31 = (srcHeight / 4 - destHeight / 4) * srcWidth;
		//int32_t start32 = (srcHeight / 2 - destHeight / 2) * srcWidth;
		temp = src + start3;
		srcTmp = srcU + start31;
		srcTmp1 = srcV + start31;
	} else if (model == 2) {
		int32_t start2 = srcWidth - destWidth;
		temp = src + start2;
		srcTmp = srcU + srcWidth - destWidth;
		srcTmp1 = srcV + srcWidth - destWidth;
	} else if (model == 1) {
		temp = src;
		srcTmp = srcU;
		srcTmp1 = srcV;
	}
	dstTmp = dest + destLen;
	dstTmp1=dest+destLen+destLen/4;
	/**
	for (int32_t i = 0; i < destHeight; i += 1) {
		memcpy(temp + srcWidth * i, dest + i * destWidth, destWidth);
	//	memcpy(temp + srcWidth * (i + 1), dest + (i + 1) * destWidth,
		//		destWidth);
		//memcpy(temp + srcWidth * (i + 2), dest + (i + 2) * destWidth,
		//		destWidth);
		//memcpy(temp + srcWidth * (i + 3), dest + (i + 3) * destWidth,
			//	destWidth);

	}
	for(int32_t i=0;i<destHeight/2;i++){
		memcpy(srcTmp + i * srcWidth/2 , dstTmp + i * destWidth/2,
						destWidth/2);
		memcpy(srcTmp1 + i * srcWidth/2, dstTmp1 + i * destWidth/2,
						destWidth/2);
	}**/
	int32_t i=0;
	for(i=0;i<destHeight;i++){
		memcpy(temp+srcWidth*i,dest+i*destWidth,destWidth);
	}
	for(i=0;i<destHeight/2;i++){
		memcpy(srcTmp+i*srcWidth/2,dstTmp+i*destWidth/2,destWidth/2);
		memcpy(srcTmp1+i*srcWidth/2,dstTmp1+i*destWidth/2,destWidth/2);
	}
	/**for(int32_t i=0;i<destHeight;i+=2){
			memcpy(temp+srcWidth*i,dest+i*destWidth,destWidth);
			memcpy(temp+srcWidth*(i+1),dest+(i+1)*destWidth,destWidth);
			memcpy(srcTmp+(i/2)*srcWidth/2,dstTmp+(i/2)*destWidth/2,destWidth/2);
			memcpy(srcTmp1+(i/2)*srcWidth/2,dstTmp1+(i/2)*destWidth/2,destWidth/2);
		}**/
	temp=NULL;
	srcTmp=NULL;
	srcTmp1=NULL;
	dstTmp=NULL;
	dstTmp1=NULL;
}

void yang_rgbtobgr(uint8_t *rgb,uint8_t *bgr,int32_t srcWidth,int32_t srcHeight){
int32_t sizes=srcWidth*srcHeight*3;
	for(int32_t i=0;i<sizes;i+=3){
		bgr[i]=rgb[i+2];
		bgr[i+1]=rgb[i+1];
		bgr[i+2]=rgb[i];
	}

}
