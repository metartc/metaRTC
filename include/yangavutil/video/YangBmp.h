//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YangBmp_H_
#define YangBmp_H_

#pragma pack(1)
struct Yang_BITMAPFILEHEADER {
        unsigned short bfType;
        uint32_t  bfSize;
        unsigned short bfReserved1;
        unsigned short bfReserved2;
        uint32_t  bfOffBits;

};

struct Yang_BITMAPINFOHEADER
{
        uint32_t  biSize;
        uint32_t  biWidth;
        uint32_t  biHeight;
        unsigned short biPlanes;
        unsigned short biBitCount;
        uint32_t  biCompression;
        uint32_t  biSizeImage;
        uint32_t  biXPelsPerMeter;
        uint32_t  biYPelsPerMeter;
        uint32_t  biClrUsed;
        uint32_t  biClrImportant;
};
#pragma pack()

class YangBmp {
public:
        YangBmp();
        virtual ~YangBmp();
public:
        void create_bmpheader(int32_t p_width, int32_t p_height);
        void save_bmp(char *p_filename, char *p_addr, int32_t p_len);
protected:

private:
        struct Yang_BITMAPFILEHEADER bfh;
        struct Yang_BITMAPINFOHEADER bih;
};

#endif
