//
// Copyright (c) 2019-2023 yanggaofeng
//

#ifndef SRC_YANGPlayerDEF_H_
#define SRC_YANGPlayerDEF_H_
#include <stdint.h>
extern "C"{

void* yang_create_player();
void yang_destroy_player(void* user);
int32_t yang_player_playRtc(void* user,char* url);
int32_t yang_player_close(void* user);
uint8_t* yang_player_get_buffer(void* user,int32_t width,int32_t height);
}



#endif
