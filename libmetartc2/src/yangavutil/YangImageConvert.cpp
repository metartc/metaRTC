/*
 * YangImageConvert.cpp
 *
 *  Created on: 2020年10月25日
 *      Author: yang
 */

#include <yangavutil/YangImageConvert.h>


#include <stdio.h>
#ifndef _WIN32
#include <yangutil/yang_unistd.h>

#include <string.h>
#include <stdlib.h>
#define MY(a,b,c) (( a*  0.2989  + b*  0.5866  + c*  0.1145))
#define MU(a,b,c) (( a*(-0.1688) + b*(-0.3312) + c*  0.5000 + 128))
#define MV(a,b,c) (( a*  0.5000  + b*(-0.4184) + c*(-0.0816) + 128))

#define DY(a,b,c) (MY(a,b,c) > 255 ? 255 : (MY(a,b,c) < 0 ? 0 : MY(a,b,c)))
#define DU(a,b,c) (MU(a,b,c) > 255 ? 255 : (MU(a,b,c) < 0 ? 0 : MU(a,b,c)))
#define DV(a,b,c) (MV(a,b,c) > 255 ? 255 : (MV(a,b,c) < 0 ? 0 : MV(a,b,c)))
#define uint8_t uint8_t
YangImageConvert::YangImageConvert() {
	// TODO Auto-generated constructor stub

}

YangImageConvert::~YangImageConvert() {
	// TODO Auto-generated destructor stub
}


void YangImageConvert::RGB24_TO_YV12(uint8_t* yv12,uint8_t* rgb24,int32_t w,int32_t h)
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

            //Y =   0.299R + 0.587G + 0.114B
            //U =  -0.147R - 0.289G + 0.436B
            //V =   0.615R - 0.515G - 0.100B

            // 这个算法颜色不正啊。。
            //vay =  pRGB->r *  0.299 + pRGB->g *  0.587 + pRGB->b *  0.114;    // 根据公式计算出Y
            //vav =  pRGB->r *  0.615 + pRGB->g * -0.515 + pRGB->b * -0.100;    // 根据公式计算出V
            //vau =  pRGB->r * -0.147 + pRGB->g * -0.289 + pRGB->b *  0.436;    // 根据公式计算出U


            //Y = round( 0.256788 * R + 0.504129 * G + 0.097906 * B) +  16
            //U = round(-0.148223 * R - 0.290993 * G + 0.439216 * B) + 128
            //V = round( 0.439216 * R - 0.367788 * G - 0.071427 * B) + 128


            // 好象这个算法颜色正，而且是MSDN中列出的算法
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

void YangImageConvert::RGB24_To_I420(  uint8_t *RGBbuf,  uint8_t *YUV,  int32_t width,  int32_t height )
{

        int32_t i,x,y,j;
        uint8_t *Y = NULL;
        uint8_t *U = NULL;
        uint8_t *V = NULL;

        uint8_t *RGB = NULL;

        int32_t imgSize = width*height;

        RGB = (uint8_t*)malloc(imgSize*3);

        memcpy(RGB, RGBbuf, width*height*3);
//      for(i=HEIGHT-1,j=0; i>=0; i--,j++)//µ÷ÕûË³Ðò
//      {
//              memcpy(RGB+j*WIDTH*3,RGB+WIDTH*HEIGHT*3+i*WIDTH*3,WIDTH*3);
//      }

        /************************************************************************/
        // ÊäÈëµÄRGBbufÈôÎªBGRÐòÁÐ£¬Ôò×¢ÊÍ¸Ã¶Î
        //Ë³Ðòµ÷Õû
//      uint8_t temp;
//      for(i=0; (unsigned int)i < WIDTH*HEIGHT*3; i+=3)
//      {
//              temp = RGB[i];
//              RGB[i] = RGB[i+2];
//              RGB[i+2] = temp;
//      }
        /************************************************************************/


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

        if(RGB) free(RGB);
}
uint8_t YangImageConvert::clip255(long v)
{
        if(v < 0) v=0;
        else if( v > 255) v=255;
        return (uint8_t )v;
}
void YangImageConvert::YUY2_To_RGB24(uint8_t *YUY2buff,uint8_t *RGBbuff,unsigned long dwSize)
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

void YangImageConvert::plusYuy2(uint8_t* src,uint8_t * dest,int32_t srcWidth,int32_t srcHeight,int32_t destWidth,int32_t destHeight,int32_t model){

	int32_t i=0;
	int32_t j=0;
	int32_t sh=srcHeight-destHeight;
	int32_t sw=srcWidth-destWidth;
	int32_t start=0;
	uint8_t* temp;
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
			for(j=0;j<destWidth*2;j=j+2){
				*(temp+2*i*srcWidth+j)=*(dest+i*2*destWidth+j);
				*(temp+2*i*srcWidth+j+1)=*(dest+i*2*destWidth+j+1);
			}
		}

	}
}
void YangImageConvert::plusAuthor(uint8_t* src,uint8_t * dest,int32_t srcWidth,int32_t srcHeight,int32_t destWidth,int32_t destHeight,int32_t model){
	int32_t i=0;
	int32_t j=0;
	int32_t nline=3;
	int32_t sh=srcHeight-destHeight;
	int32_t sw=srcWidth-destWidth;
	int32_t destHlen=destWidth<<1,srcHlen=srcWidth<<1;
	int32_t start=0;
	switch(model){
	case 1:start=nline*srcWidth*2+20;break;
	case 2:start=(nline+1)*srcWidth*2-2*destWidth-20;break;
	case 3:start=(srcHeight-nline-1)*srcWidth*2+20;break;
	case 4:start=(srcHeight-nline)-2*destWidth-20;break;
	}
	/**for(i=0;i<destHeight;i++){
			//memcpy(src+start+(i*srcHlen),dest+(destHlen*i),destHlen);
		}**/
	    int32_t Y11, U11, V11, Y12, Y21, U21, V21, Y22;
		int32_t alpha1=128,alpha2=96;
		uint8_t *tmp1=NULL,*tmp2=NULL;
	 for (int32_t i=0; i<destHeight; ++i)
        {
            for (int32_t j=0; j<destWidth/2; ++j)
            {
				tmp1=src+start+i*srcWidth*2+j*4;
                Y11 = *tmp1;
                U11 = *(tmp1+1);
                Y12 = *(tmp1+2);
                V11 = *(tmp1+3);
				tmp2=dest+i*destWidth*2+j*4;
                Y21 = *(tmp2);
                U21 = *(tmp2+1);
                Y22 = *(tmp2+2);
                V21 = *(tmp2+3);
               // alpha1 = *(pFGData+i*width*3+j*6+4);
                //alpha2 = *(pFGData+i*width*3+j*6+5);

                *(tmp1) = (Y21-16)*alpha1/255+(Y11-16)*(255-alpha1)/255+16;
                *(tmp1+1) = ((U21-128)*alpha1/255+(U11-128)*(255-alpha1)/255 + (U21-128)*alpha2/255+(U11-128)*(255-alpha2)/255)/2+128;
                *(tmp1+3) = ((V21-128)*alpha1/255+(V11-128)*(255-alpha1)/255 + (V21-128)*alpha2/255+(V11-128)*(255-alpha2)/255)/2+128;
                *(tmp1+2) = (Y22-16)*alpha2/255+(Y12-16)*(255-alpha2)/255+16;
            }
	}
}
int32_t YangImageConvert::YUVBlending(void* pBGYUV, void* pFGYUV, int32_t width, int32_t height, bool alphaBG, bool alphaFG)
{
    if (NULL == pBGYUV || NULL == pFGYUV)
    {
        return -1;
    }
    uint8_t* pBGData = (uint8_t*)pBGYUV;
    uint8_t* pFGData = (uint8_t*)pFGYUV;
    if (!alphaFG)
    {
        if (!alphaBG)
        {
            memcpy(pBGData, pFGData, width*height*2);
        }
        else
        {
            for (int32_t i=0; i<height; ++i)
            {
                for (int32_t j=0; j<width/2; ++j)
                {
                    *(pBGData+i*width*2+j*4) = *(pFGData+i*width*2+j*4);
                    *(pBGData+i*width*2+j*4+1) = *(pFGData+i*width*2+j*4+1);
                    *(pBGData+i*width*2+j*4+2) = *(pFGData+i*width*2+j*4+2);
                    *(pBGData+i*width*2+j*4+3) = *(pFGData+i*width*2+j*4+3);
                }
            }
        }
    }
    int32_t Y11, U11, V11, Y12, Y21, U21, V21, Y22;
    int32_t alpha1, alpha2;
    if (!alphaBG)
    {
        for (int32_t i=0; i<height; ++i)
        {
            for (int32_t j=0; j<width/2; ++j)
            {
                Y11 = *(pBGData+i*width*2+j*4);
                U11 = *(pBGData+i*width*2+j*4+1);
                Y12 = *(pBGData+i*width*2+j*4+2);
                V11 = *(pBGData+i*width*2+j*4+3);

                Y21 = *(pFGData+i*width*3+j*6);
                U21 = *(pFGData+i*width*3+j*6+1);
                Y22 = *(pFGData+i*width*3+j*6+2);
                V21 = *(pFGData+i*width*3+j*6+3);
                alpha1 = *(pFGData+i*width*3+j*6+4);
                alpha2 = *(pFGData+i*width*3+j*6+5);

                *(pBGData+i*width*2+j*4) = (Y21-16)*alpha1/255+(Y11-16)*(255-alpha1)/255+16;
                *(pBGData+i*width*2+j*4+1) = ((U21-128)*alpha1/255+(U11-128)*(255-alpha1)/255 + (U21-128)*alpha2/255+(U11-128)*(255-alpha2)/255)/2+128;
                *(pBGData+i*width*2+j*4+3) = ((V21-128)*alpha1/255+(V11-128)*(255-alpha1)/255 + (V21-128)*alpha2/255+(V11-128)*(255-alpha2)/255)/2+128;
                *(pBGData+i*width*2+j*4+2) = (Y22-16)*alpha2/255+(Y12-16)*(255-alpha2)/255+16;
            }
        }
    }
    else
    {
        for (int32_t i=0; i<height; ++i)
        {
            for (int32_t j=0; j<width/2; ++j)
            {
                Y11 = *(pBGData+i*width*3+j*6);
                U11 = *(pBGData+i*width*3+j*6+1);
                Y12 = *(pBGData+i*width*3+j*6+2);
                V11 = *(pBGData+i*width*3+j*6+3);

                Y21 = *(pFGData+i*width*3+j*6);
                U21 = *(pFGData+i*width*3+j*6+1);
                Y22 = *(pFGData+i*width*3+j*6+2);
                V21 = *(pFGData+i*width*3+j*6+3);
                alpha1 = *(pFGData+i*width*3+j*6+4);
                alpha2 = *(pFGData+i*width*3+j*6+5);

                *(pBGData+i*width*3+j*6) = (Y21-16)*alpha1/255+(Y11-16)*(255-alpha1)/255+16;
                *(pBGData+i*width*3+j*6+1) = ((U21-128)*alpha1/255+(U11-128)*(255-alpha1)/255 + (U21-128)*alpha2/255+(U11-128)*(255-alpha2)/255)/2+128;
                *(pBGData+i*width*3+j*6+3) = ((V21-128)*alpha1/255+(V11-128)*(255-alpha1)/255 + (V21-128)*alpha2/255+(V11-128)*(255-alpha2)/255)/2+128;
                *(pBGData+i*width*3+j*6+2) = (Y22-16)*alpha2/255+(Y12-16)*(255-alpha2)/255+16;
            }
        }
    }
    return 0;
}
#endif
