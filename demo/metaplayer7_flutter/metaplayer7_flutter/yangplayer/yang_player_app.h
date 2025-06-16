
//
// Copyright (c) 2019-2023 yanggaofeng
//

#ifndef SRC_YANGMETAPLAYERAPP_H_
#define SRC_YANGMETAPLAYERAPP_H_
#include <stdint.h>
class YangPlayerApp{
    public:
    YangPlayerApp();
    ~YangPlayerApp();
    void* m_player;
    void loadLib();
    int32_t playRtc(char* url);
    int32_t close();
    uint8_t* getPlayBuffer(int32_t width,int32_t height); 
};
#endif
