//
// Copyright (c) 2019-2023 yanggaofeng
//

#ifndef YANGVIDEODEVICEMAC_H
#define YANGVIDEODEVICEMAC_H
#include <yangutil/yangtype.h>
#include <yangutil/yangavinfotype.h>


typedef struct{
    void* user;
    void (*on_video)(uint8_t* data,uint32_t nb,uint64_t ts,void* user);
}YangMacVideoCallback;

typedef struct{
    void* session;
    int32_t (*init)(void* session);
    int32_t (*start)(void* session);
    int32_t (*stop)(void* session);
}YangVideoDeviceMac;


int32_t yang_create_macVideo(YangVideoDeviceMac* video,uint32_t width,uint32_t height,uint32_t framerate,YangMacVideoCallback* callback);
void yang_destroy_macVideo(YangVideoDeviceMac* video);


#endif // YANGVIDEODEVICEMAC_H
