//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef METAPLAYER3_YANGAUDIOANDROID_H
#define METAPLAYER3_YANGAUDIOANDROID_H
#include <yangutil/yangtype.h>

typedef void (*yang_audioAndroid_record_callback)(uint8_t* data,int32_t nb,void* user);

struct YangAudioAndroid {
    void* context;

    int32_t (*setPlayRate)(void* context,int rateChange);
    int32_t (*getInputdataCount)(void* context);
    uint32_t (*audioOut)(void* context, uint16_t *buffer,uint32_t size);
    double (*getTimestamp)(void* context);

};
int32_t yang_create_audioAndroid_play(YangAudioAndroid* aa,int32_t sample,int32_t channels);
int32_t yang_create_audioAndroid_record(YangAudioAndroid* aa,yang_audioAndroid_record_callback callback,void* user,int32_t sample,int32_t channels);
void yang_destroy_audioAndroid(YangAudioAndroid* aa);

#endif //METAPLAYER3_YANGAUDIOANDROID_H
