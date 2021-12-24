#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "yangavutil/video/YangPicConvert.h"


 struct RGB24{
        uint8_t    b;
        uint8_t    g;
        uint8_t    r;
} ;
#define MY(a,b,c) (( a*  0.2989  + b*  0.5866  + c*  0.1145))
#define MU(a,b,c) (( a*(-0.1688) + b*(-0.3312) + c*  0.5000 + 128))
#define MV(a,b,c) (( a*  0.5000  + b*(-0.4184) + c*(-0.0816) + 128))

#define DY(a,b,c) (MY(a,b,c) > 255 ? 255 : (MY(a,b,c) < 0 ? 0 : MY(a,b,c)))
#define DU(a,b,c) (MU(a,b,c) > 255 ? 255 : (MU(a,b,c) < 0 ? 0 : MU(a,b,c)))
#define DV(a,b,c) (MV(a,b,c) > 255 ? 255 : (MV(a,b,c) < 0 ? 0 : MV(a,b,c)))


YangConvert::YangConvert(){
	//mpu=NULL;

}
YangConvert::~YangConvert(){
	//mpu=NULL;

}


void YangConvert::zoomIn4(uint8_t *src,uint8_t *dest,int32_t srcWidth,int32_t srcHeight,int32_t destWidth,int32_t destHeight){
	uint8_t *srcTmp=src+srcWidth*srcHeight;
	uint8_t *dstTmp=dest+destWidth*destHeight;
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


void YangConvert::zoom4Yuy2(uint8_t *src,uint8_t *dest,int32_t srcWidth,int32_t srcHeight,int32_t destWidth,int32_t destHeight){
	for(int32_t i=0;i<destHeight;i+=1){
		for(int32_t j=0;j<destWidth*2;j+=4){
			*(dest+i*destWidth*2+j)=*(src+4*i*srcWidth*2+4*j);
			*(dest+i*destWidth*2+j+1)=*(src+4*i*srcWidth*2+4*j+1);
			*(dest+i*destWidth*2+j+2)=*(src+4*i*srcWidth*2+4*j+2);
			*(dest+i*destWidth*2+j+3)=*(src+4*i*srcWidth*2+4*j+3);

		}
	}
}

void YangConvert::resize_NV12(uint8_t *src,uint8_t *dest,int32_t srcWidth,int32_t srcHeight,int32_t destWidth,int32_t destHeight){
	uint8_t *dest_uv=dest+destWidth*destHeight;
	uint8_t *src_uv=src+srcWidth*srcHeight;
	int32_t ts=0;
	int32_t td=0;
	int32_t ts1=0,ts2=0;
	int32_t td1=0,td2=0;
	float f=srcWidth/destWidth;
	/**for(int32_t i=0;i<p_destHeight;i++){
		for(int32_t j=0;j<p_destWidth;j++){
			ts=(i*p_srcWidth+j)*f;
			td=i*p_destWidth+j;
			*(dest+td)=*(src+ts);
			if(i%4==0&&j%4==0) {
				ts=((i*p_srcWidth+j)>>2)*f;
				td=(i*p_destWidth+j)>>2;
				*(dest_uv+td)=*(src_uv+ts);
				*(dest_uv+td+1)=*(src_uv+ts+1);
			}
		}
	}**/

	for(int32_t i=0;i<destHeight;i+=2){
		for(int32_t j=0;j<destWidth;j+=2){
			ts=(i*srcWidth+j)*f;
			td=i*destWidth+j;
			ts1=(f*i+1)*srcWidth+f*j;
			td1=(i+1)*destWidth+j;
			ts2=(i*srcWidth/2+j)*f;
			td2=i*destWidth/2+j;

			*(dest+td)=*(src+ts);
			*(dest+td+1)=*(src+ts+1);
			*(dest+td1)=*(src+ts1);
			*(dest+td1+1)=*(src+ts1+1);
			*(dest_uv+td2)=*(src_uv+ts2);
			*(dest_uv+td2+1)=*(src_uv+ts2+1);
		}
	}

}

void YangConvert::nv12_nearest_scale(uint8_t*  src, uint8_t*  dst,int32_t srcWidth,int32_t srcHeight,int32_t dstWidth,int32_t dstHeight)      //restrict keyword is for compiler to optimize program
{
    register int32_t sw = srcWidth;  //register keyword is for local var to accelorate
    register int32_t sh = srcHeight;
    register int32_t dw = dstWidth;
    register int32_t dh = dstHeight;
    register int32_t y, x;

    uint64_t xrIntFloat_16 = (sw << 16) / dw + 1; //better than float division
    uint64_t yrIntFloat_16 = (sh << 16) / dh + 1;
    uint8_t* dst_uv = dst + dh * dw; //memory start pointer of dest uv
    uint8_t* src_uv = src + sh * sw; //memory start pointer of source uv
    uint8_t* dst_y_slice = dst; //memory start pointer of dest y
    uint8_t* dst_uv_yScanline;
    uint8_t* src_uv_yScanline;
    //uint8_t* dst_y_slice; //memory start pointer of dest y
    uint8_t* src_y_slice;
    uint8_t* sp;
    uint8_t* dp;
    uint64_t  srcy, srcx, src_index, dst_index;
    for (y = 0; y < (dh & ~7); ++y)  //'dh & ~7' is to generate faster assembly code
    {
        srcy = (y * yrIntFloat_16) >> 16;
        src_y_slice = src + srcy * sw;

        if((y & 1) == 0)
        {
            dst_uv_yScanline = dst_uv + (y / 2) * dw;
            src_uv_yScanline = src_uv + (srcy / 2) * sw;
        }

        for(x = 0; x < (dw & ~7); ++x)
        {
            srcx = (x * xrIntFloat_16) >> 16;
            dst_y_slice[x] = src_y_slice[srcx];

            if((y & 1) == 0) //y is even
            {
                if((x & 1) == 0) //x is even
                {
                    src_index = (srcx / 2) * 2;

                    sp = dst_uv_yScanline + x;
                    dp = src_uv_yScanline + src_index;
                    *sp = *dp;
                    ++sp;
                    ++dp;
                    *sp = *dp;
                }
             }
         }
        dst_y_slice += dw;
    }
}

void YangConvert::resize_Yuy2_NV12(uint8_t *src,uint8_t *dest,int32_t p_srcWidth,int32_t p_srcHeight,int32_t p_destWidth,int32_t p_destHeight){
	uint8_t *dest_uv=dest+p_destWidth*p_destHeight;
	//uint8_t *src_uv=src+p_srcWidth*p_srcHeight;
	int32_t temp=0;
	float f=p_srcWidth/p_destWidth;
	for(int32_t i=0;i<p_destWidth;i+=2){
		for(int32_t j=0;j<p_destHeight;j+=2){
			temp=((i*p_destWidth+j)<<1)*f;
			*(dest+i*p_destWidth+j)=*(src+temp);
			if(i%4==0&&j%4==0) {
				*(dest_uv+i*p_destWidth+j)=*(src+temp+1);
				*(dest_uv+i*p_destWidth+j+1)=*(src+temp+3);
			}
		}
	}

}


void YangConvert::plusNV12(uint8_t* src,uint8_t * dest,int32_t model,int32_t srcWidth,int32_t srcHeight,int32_t destWidth,int32_t destHeight){
	int32_t srcLen=srcWidth*srcHeight,destLen=destWidth*destHeight;
	int32_t start2=srcWidth-destWidth;
	int32_t start3=srcWidth*(srcHeight-destHeight);
	int32_t start31=(srcHeight/2-destHeight/2)*srcWidth;
	//printf("\n..............start3====%d>>>>>>>>>>>>>>>>>>>>",start3);
	int32_t start4=(srcHeight-destHeight+1)*srcWidth-destWidth;
	int32_t start41=(srcHeight/2-destHeight/2)*srcWidth+(srcWidth-destWidth);
	uint8_t *temp,*srcTmp,*dstTmp;
	if(model==4){		
		temp=src+start4;
		srcTmp=src+srcLen+start41;	
	}else if(model==3){
		temp=src+start3;
		srcTmp=src+srcLen+start31;
	}
	else if(model==2){		
		temp=src+start2;
		srcTmp=src+srcLen+srcWidth-destWidth;
	}
	else if(model==1){
		temp=src;	
		srcTmp=src+srcLen;
	}
		dstTmp=dest+destLen;
		for(int32_t i=0;i<destHeight;i+=2){
			memcpy(temp+srcWidth*i,dest+i*destWidth,destWidth);
			memcpy(temp+srcWidth*(i+1),dest+(i+1)*destWidth,destWidth);
			memcpy(srcTmp+i*srcWidth/2,dstTmp+i*destWidth/2,destWidth);
		}
}

void YangConvert::plusYuy2(uint8_t* src,uint8_t * dest,int32_t model,int32_t srcWidth,int32_t srcHeight,int32_t destWidth,int32_t destHeight){
	int32_t srcLen=srcWidth*srcHeight,destLen=destWidth*destHeight;
	int32_t start2=srcWidth-destWidth;
	int32_t start3=srcWidth*(srcHeight-destHeight);
	int32_t start31=(srcHeight/2-destHeight/2)*srcWidth;
	//printf("\n..............start3====%d>>>>>>>>>>>>>>>>>>>>",start3);
	int32_t start4=(srcHeight-destHeight+1)*srcWidth*2-destWidth*2;
	//int32_t start41=(srcHeight/2-destHeight/2)*srcWidth+(srcWidth-destWidth);
	uint8_t *temp,*srcTmp,*dstTmp;
	if(model==4){
		temp=src+start4;
		//srcTmp=src+srcLen+start41;
	}else if(model==3){
		temp=src+start3;
		srcTmp=src+srcLen+start31;
	}
	else if(model==2){
		temp=src+start2;
		srcTmp=src+srcLen+srcWidth-destWidth;
	}
	else if(model==1){
		temp=src;
		srcTmp=src+srcLen;
	}
		dstTmp=dest+destLen;
		for(int32_t i=0;i<destHeight;i+=1){
			memcpy(temp+srcWidth*i*2,dest+i*destWidth*2,destWidth*2);
			//memcpy(temp+srcWidth*(i+1),dest+(i+1)*destWidth,destWidth);
			//memcpy(srcTmp+i*srcWidth/2,dstTmp+i*destWidth/2,destWidth);
		}
}

void YangConvert::YUY2toI420(int32_t inWidth, int32_t inHeight, uint8_t *pSrc, uint8_t *pDest)
{
 int32_t i, j;
 uint8_t *u = pDest + (inWidth * inHeight);
 uint8_t *v = u + (inWidth * inHeight) / 4;

 for (i = 0; i < inHeight/2; i++)
 {

        uint8_t *src_l1 = pSrc + inWidth*2*2*i;
        uint8_t *src_l2 = src_l1 + inWidth*2;
        uint8_t *y_l1 = pDest + inWidth*2*i;
        uint8_t *y_l2 = y_l1 + inWidth;
  for (j = 0; j < inWidth/2; j++)
  {
   // two pels in one go
    *y_l1++ = src_l1[0];
    *u++ = src_l1[1];
    *y_l1++ = src_l1[2];
    *v++ = src_l1[3];
    *y_l2++ = src_l2[0];
    *y_l2++ = src_l2[2];
    src_l1 += 4;
    src_l2 += 4;
  }
 }
}
void YangConvert::YUY2toNV12(int32_t inWidth, int32_t inHeight, uint8_t *pSrc, uint8_t *pDest)
{
 int32_t i, j;
 uint8_t *uv = pDest + (inWidth * inHeight);
// uint8_t *v = u + (inWidth * inHeight) / 4;

 for (i = 0; i < inHeight/2; i++)
 {

	 uint8_t *src_l1 = pSrc + inWidth*2*2*i;
	 uint8_t *src_l2 = src_l1 + inWidth*2;
	 uint8_t *y_l1 = pDest + inWidth*2*i;
	 uint8_t *y_l2 = y_l1 + inWidth;
  for (j = 0; j < inWidth/2; j++)
  {
    *y_l1++ = src_l1[0];
    *uv++ = src_l1[1];
    *y_l1++ = src_l1[2];
    *uv++ = src_l1[3];
    *y_l2++ = src_l2[0];
    *y_l2++ = src_l2[2];
    src_l1 += 4;
    src_l2 += 4;
  }
 }
}
/**	
void Convert::plus(uint8_t* src,uint8_t * dest,int32_t srcWidth,int32_t srcHeight,int32_t destWidth,int32_t destHeight,int32_t model){
	
	i=0;
	j=0;
 sh=srcHeight-destHeight;
	sw=srcWidth-destWidth;
	start=0,start1=0;
	
	if(model==4){
		start=(srcHeight-destHeight+1)*srcWidth*2-destWidth*2;
		temp=src+start;
		for(i=0;i<destHeight;i++){
			
			for(j=0;j<destWidth*2;j=j+2){
				temp=src+start+2*i*srcWidth;
				*(temp+j)=*(dest+i*2*destWidth+j);
				*(temp+j+1)=*(dest+i*2*destWidth+j+1);
			}
		}
	}else if(model==3){
		start=2*srcWidth*(srcHeight-destHeight);
		temp=src+start;
		for(i=0;i<destHeight;i++){
			for(j=0;j<destWidth*2;j=j+2){
				*(temp+2*i*srcWidth+j)=*(dest+i*2*destWidth+j);
				*(temp+2*i*srcWidth+j+1)=*(dest+i*2*destWidth+j+1);
			}
		}
	
	}
	else if(model==2){
		start=2*srcWidth-2*destWidth;
		temp=src+start;
		for(i=0;i<destHeight;i++){
			for(j=0;j<destWidth*2;j=j+2){
				*(temp+2*i*srcWidth+j)=*(dest+2*i*destWidth+j);
				*(temp+2*i*srcWidth+j+1)=*(dest+2*i*destWidth+j+1);
			}
		}
	
	}
	else if(model==1){
		//start=srcWidth-destWidth;
		temp=src;
		for(i=0;i<destHeight;i++){
			for(j=0;j<destWidth;j=j+2){
				*(temp+2*i*srcWidth+j)=*(dest+i*2*destWidth+j);
				*(temp+2*i*srcWidth+j+1)=*(dest+i*2*destWidth+j+1);
			}
		}
	
	}
	
}
**/

void YangConvert::RGB24_To_NV12(  uint8_t *RGBbuf,  uint8_t *YUV,  int32_t width,  int32_t height ){

	for(int32_t i=0;i<height;i++){

	}
}
/**
void YangConvert::RGB24_TO_YV12(uint8_t* yv12,uint8_t* rgb24,int32_t w,int32_t h)
{
    int32_t iBufLen = w * h;
    int32_t i,j,vay,vau,vav;
    uint8_t* cv;        // 当前坐标的v(current v);
    uint8_t* nv;        // 在cv下一行的对应位置的 v;
    uint8_t* cu;        // 当前坐标的u(current u);
    uint8_t* nu;        // 在cu下一行的对应位置的 u;
    uint8_t v01,v02,v11,v12,u01,u02,u11,u12;            // 需要整合的相邻的4个象素 如下

    uint8_t* vv = new uint8_t[iBufLen];            // 每个RGB单位对应的V!
    uint8_t* uu = new uint8_t[iBufLen];            // 每个RGB单位对应的U!

    // 按标准算法从RGB24计算出所有YUV

    RGB24 * pRGB = (RGB24*)rgb24;
    uint8_t* y = yv12;                                // 这里直接用 yuv 缓冲。。省了copy了。
    uint8_t* v = vv;
    uint8_t* u = uu;

    for(i = 0; i < h; i++)
    {
        for(j = 0; j < w; j++)
        {

            vay =  0.256788 * pRGB->r + 0.504129 * pRGB->g + 0.097906 * pRGB->b +  16;
            vau = -0.148223 * pRGB->r - 0.290993 * pRGB->g + 0.439216 * pRGB->b + 128;
            vav =  0.439216 * pRGB->r - 0.367788 * pRGB->g - 0.071427 * pRGB->b + 128;


            *y =  vay < 0 ? 0 : (vay > 255 ? 255: vay);                        // 如果Y小于0置换成0，如果Y大于255就置换成255
            *v =  vav < 0 ? 0 : (vav > 255 ? 255: vav);                        // 如果V小于0置换成0，如果V大于255就置换成255
            *u =  vau < 0 ? 0 : (vau > 255 ? 255: vau);                        // 如果U小于0置换成0，如果U大于255就置换成255

            y++;    // 移动到下一位！
            v++;
            u++;
            pRGB++;
         }
     }

      u = yv12 + iBufLen;                                // 记录成品YV12的U的位置

      v = u    + (iBufLen >> 2);                          // 记录成品YV12的V的位置

    for(i = 0; i < h; i+=2)                            // 由于 V 和 U 只记录隔行的，所以 += 2;
    {
        cv = vv + i * w;                            // 取得第i     行的v;
        nv = vv + (i + 1) * w;                        // 取得第i + 1 行的v

        cu = uu + i * w;                            // 取得第i     行的u;
        nu = uu + (i + 1) * w;                        // 取得第i + 1 行的u


        for(j = 0; j < w; j+=2)                        // 由于 一躺循环 我们访问 两个 uu 或 vv 所以 += 2;
        {
            v01 = *(cv + j);                        // 取得第i     行的第j     个v的具体的值
            v02 = *(cv + j + 1);                    // 取得第i     行的第j + 1 个v的具体的值
            v11 = *(nv + j);                        // 取得第i + 1 行的第j     个v的具体的值
            v12 = *(nv + j + 1);                    // 取得第i + 1 行的第j + 1 个v的具体的值

            *v = (v01 + v02 + v11 + v12) / 4;        // 取v01,v02,v11,v12的平均值给v


            u01 = *(cu + j);                        // 取得第i     行的第j     个u的具体的值
            u02 = *(cu + j + 1);                    // 取得第i     行的第j + 1 个u的具体的值
            u11 = *(nu + j);                        // 取得第i + 1 行的第j     个u的具体的值
            u12 = *(nu + j + 1);                    // 取得第i + 1 行的第j + 1 个u的具体的值

            *u = (u01 + u02 + u11 + u12) / 4;        // 取u01,u02,u11,u12的平均值给u

            v++;    // 移动到下一位！
            u++;

         }
     }

    delete [] vv;
    delete [] uu;

 }
**/
void YangConvert::RGB24_TO_YV12(uint8_t* yv12,uint8_t* rgb24,int32_t w,int32_t h)
{
    int32_t iBufLen = w * h;
    int32_t i,j,vay,vau,vav;
    uint8_t* cv;        // 当前坐标的v(current v);
    uint8_t* nv;        // 在cv下一行的对应位置的 v;
    uint8_t* cu;        // 当前坐标的u(current u);
    uint8_t* nu;        // 在cu下一行的对应位置的 u;
    uint8_t v01,v02,v11,v12,u01,u02,u11,u12;            // 需要整合的相邻的4个象素 如下

    uint8_t* vv = new uint8_t[iBufLen];            // 每个RGB单位对应的V!
    uint8_t* uu = new uint8_t[iBufLen];            // 每个RGB单位对应的U!

    // 按标准算法从RGB24计算出所有YUV

    RGB24 * pRGB = (RGB24*)rgb24;
    uint8_t* y = yv12;                                // 这里直接用 yuv 缓冲。。省了copy了。
    uint8_t* v = vv;
    uint8_t* u = uu;

    for(i = 0; i < h; i++)
    {
        for(j = 0; j < w; j++)
        {

            vay =  0.256788 * pRGB->r + 0.504129 * pRGB->g + 0.097906 * pRGB->b +  16;
            vau = -0.148223 * pRGB->r - 0.290993 * pRGB->g + 0.439216 * pRGB->b + 128;
            vav =  0.439216 * pRGB->r - 0.367788 * pRGB->g - 0.071427 * pRGB->b + 128;


            *y =  vay < 0 ? 0 : (vay > 255 ? 255: vay);                        // 如果Y小于0置换成0，如果Y大于255就置换成255
            *v =  vav < 0 ? 0 : (vav > 255 ? 255: vav);                        // 如果V小于0置换成0，如果V大于255就置换成255
            *u =  vau < 0 ? 0 : (vau > 255 ? 255: vau);                        // 如果U小于0置换成0，如果U大于255就置换成255

            y++;    // 移动到下一位！
            v++;
            u++;
            pRGB++;
         }
     }

      u = yv12 + iBufLen;                                // 记录成品YV12的U的位置

      v = u    + (iBufLen >> 2);                          // 记录成品YV12的V的位置

    for(i = 0; i < h; i+=2)                            // 由于 V 和 U 只记录隔行的，所以 += 2;
    {
        cv = vv + i * w;                            // 取得第i     行的v;
        nv = vv + (i + 1) * w;                        // 取得第i + 1 行的v

        cu = uu + i * w;                            // 取得第i     行的u;
        nu = uu + (i + 1) * w;                        // 取得第i + 1 行的u


        for(j = 0; j < w; j+=2)                        // 由于 一躺循环 我们访问 两个 uu 或 vv 所以 += 2;
        {
            v01 = *(cv + j);                        // 取得第i     行的第j     个v的具体的值
            v02 = *(cv + j + 1);                    // 取得第i     行的第j + 1 个v的具体的值
            v11 = *(nv + j);                        // 取得第i + 1 行的第j     个v的具体的值
            v12 = *(nv + j + 1);                    // 取得第i + 1 行的第j + 1 个v的具体的值

            *v = (v01 + v02 + v11 + v12) / 4;        // 取v01,v02,v11,v12的平均值给v


            u01 = *(cu + j);                        // 取得第i     行的第j     个u的具体的值
            u02 = *(cu + j + 1);                    // 取得第i     行的第j + 1 个u的具体的值
            u11 = *(nu + j);                        // 取得第i + 1 行的第j     个u的具体的值
            u12 = *(nu + j + 1);                    // 取得第i + 1 行的第j + 1 个u的具体的值

            *u = (u01 + u02 + u11 + u12) / 4;        // 取u01,u02,u11,u12的平均值给u

            v++;    // 移动到下一位！
            u++;

         }
     }

    delete [] vv;
    delete [] uu;

 }

void YangConvert::RGB24_To_I420(  uint8_t *RGB,  uint8_t *YUV,  int32_t width,  int32_t height )
{
        int32_t i,x,y,j;
        uint8_t *Y = NULL;
        uint8_t *U = NULL;
        uint8_t *V = NULL;
        Y = YUV;
        U = YUV + width*height;
        V = U + ((width*height)>>2);

        for(y=0; y < height; y++)
                for(x=0; x < width; x++)
                {
                        j = y*width + x;
                        i = j*3;
                        Y[j] = (uint8_t)(DY(RGB[i], RGB[i+1], RGB[i+2]));

                        if(x%2 == 1 && y%2 == 1)
                        {
                                j = (width>>1) * (y>>1) + (x>>1);
                                //ÉÏÃæiÈÔÓÐÐ§
                                U[j] = (uint8_t)
                                        ((DU(RGB[i  ], RGB[i+1], RGB[i+2]) +
                                        DU(RGB[i-3], RGB[i-2], RGB[i-1]) +
                                        DU(RGB[i  -width*3], RGB[i+1-width*3], RGB[i+2-width*3]) +
                                        DU(RGB[i-3-width*3], RGB[i-2-width*3], RGB[i-1-width*3]))/4);

                                V[j] = (uint8_t)
                                        ((DV(RGB[i  ], RGB[i+1], RGB[i+2]) +
                                        DV(RGB[i-3], RGB[i-2], RGB[i-1]) +
                                        DV(RGB[i  -width*3], RGB[i+1-width*3], RGB[i+2-width*3]) +
                                        DV(RGB[i-3-width*3], RGB[i-2-width*3], RGB[i-1-width*3]))/4);
                        }

                }
       Y = NULL;
       U = NULL;
       V = NULL;
       // if(RGB) free(RGB);
}
uint8_t YangConvert::clip255(long v)
{
        if(v < 0) v=0;
        else if( v > 255) v=255;
        return (uint8_t )v;
}
void YangConvert::YUY2_To_RGB24(uint8_t *YUY2buff,uint8_t *RGBbuff,unsigned long dwSize)
{
        //
        //lC = lY - 16
        //lD = btU - 128
        //lE = btV - 128
        //btR = clip(( 298 * lC           + 409 * lE + 128) >> 8)
        //btG = clip(( 298 * lC - 100 * lD - 208 * lE + 128) >> 8)
        //btB = clip(( 298 * lC + 516 * lD           + 128) >> 8)
        uint8_t  *orgRGBbuff = RGBbuff;
        for( unsigned long count = 0; count < dwSize; count += 4 )
        {
                //Y0 U0 Y1 V0
                uint8_t  btY0 = *YUY2buff;
                uint8_t  btU  = *(++YUY2buff);
                uint8_t  btY1 = *(++YUY2buff);
                uint8_t  btV  = *(++YUY2buff);
                ++YUY2buff;

                long lY,lC,lD,lE;
                uint8_t  btR,btG,btB;

                lY = btY0;
                lC = lY - 16;
                lD = btU - 128;
                lE = btV - 128;
                btR = clip255(( 298 * lC            + 409 * lE + 128) >> 8);
                btG = clip255(( 298 * lC - 100 * lD - 208 * lE + 128) >> 8);
                btB = clip255(( 298 * lC + 516 * lD            + 128) >> 8);

                *(RGBbuff)   = btB;
                *(++RGBbuff) = btG;
                *(++RGBbuff) = btR;

                lY = btY1;
                lC = lY-16;
                lD = btU-128;
                lE = btV-128;
                btR = clip255(( 298 * lC           + 409 * lE + 128) >> 8);
                btG = clip255(( 298 * lC - 100 * lD - 208 * lE + 128) >> 8);
                btB = clip255(( 298 * lC + 516 * lD           + 128) >> 8);
                *(++RGBbuff) = btB;
                *(++RGBbuff) = btG;
                *(++RGBbuff) = btR;
                ++RGBbuff;
        }
}
