#include "yang_player_app.h"
#include "yangutil/YangLoadLib.h"
#include <stdio.h>
void* (*yang_create_player)();
void (*yang_destroy_player)(void* user);
int32_t (*yang_player_playRtc)(void* user,char* url);
int32_t (*yang_player_close)(void* user);
uint8_t* (*yang_player_get_buffer)(void* user,int32_t width,int32_t height);
YangLoadLib *g_yang_lib=nullptr;

void YangPlayerApp::loadLib() {
    yang_create_player = (void *(*)())g_yang_lib->loadFunction(
        "yang_create_player");
    yang_destroy_player = (void (*)(void *user))g_yang_lib->loadFunction(
        "yang_destroy_player");
    yang_player_playRtc = (int32_t(*)(void *user, char *url))g_yang_lib->loadFunction(
        "yang_player_playRtc");
    yang_player_close = (int32_t(*)(void *user))g_yang_lib->loadFunction(
        "yang_player_close");
    yang_player_get_buffer = (uint8_t * (*)(void *user, int32_t width, int32_t height)) g_yang_lib->loadFunction(
        "yang_player_get_buffer");
}

    YangPlayerApp::YangPlayerApp(){
        m_player=nullptr;
        if(g_yang_lib==nullptr)
            g_yang_lib=new YangLoadLib();
        g_yang_lib->loadObject("libmetaplayer7");
 
        loadLib();
        if(yang_create_player)
            m_player=yang_create_player();
    }
   YangPlayerApp::~YangPlayerApp(){
    
    if(m_player&&yang_destroy_player) {
        yang_destroy_player(m_player);
        m_player=nullptr;
    }
    if(g_yang_lib) delete(g_yang_lib);
   }

   int32_t YangPlayerApp::playRtc(char *url) { 
    return yang_player_playRtc(m_player,url);  
    
    }

    int32_t YangPlayerApp::close()
    {
        return yang_player_close(m_player); 
        
    }

   uint8_t *YangPlayerApp::getPlayBuffer(int32_t width, int32_t height) {
    return yang_player_get_buffer(m_player,width,height);
   }
