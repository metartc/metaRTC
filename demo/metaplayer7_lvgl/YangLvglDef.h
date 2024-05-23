//
// Copyright (c) 2019-2023 yanggaofeng
//

#ifndef SRC_YANGLVGLDEF_H_
#define SRC_YANGLVGLDEF_H_
#include <yangutil/yangtype.h>

char* yang_lvgl_get_urlstr();
void yang_lvgl_setPlayButtonStr(char* str);
void yang_lvgl_setUrlText(char* str);
void yang_lvgl_setPlayState(yangbool isStart);
void yang_lvgl_refreshImg(uint8_t* data,int32_t nb,int32_t width,int32_t height);
void yang_lvgl_refreshImg_background(uint8_t* data,int32_t nb,int32_t width,int32_t height);

#endif /* SRC_YANGLVGLDEF_H_ */
