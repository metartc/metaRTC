//
// Copyright (c) 2019-2023 yanggaofeng
//

#ifndef SRC_YANGPlayerDEF_H_
#define SRC_YANGPlayerDEF_H_
#include <stdint.h>

void yang_player_init(int width,int height);
void yang_player_destroy();

int32_t yang_playRtc(char* url);
int32_t yang_closeRtc();

void yang_player_refreshImg(uint8_t* data,int32_t width,int32_t height);
void yang_player_refreshImg_background();
#endif /* SRC_YANGLVGLDEF_H_ */
