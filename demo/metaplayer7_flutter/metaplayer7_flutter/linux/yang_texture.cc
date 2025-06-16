// Copyright 2014 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "yang_texture.h"
#include "yangplayer/yang_player_app.h"
#include "yangplayer/YangPlayerDef.h"
#include "yangplayer/yangrenderthread.h"
#include <stdio.h>
// An object that generates a texture for Flutter.
struct _YangTexture {
  FlPixelBufferTexture parent_instance;

  // Dimensions of texture.
  uint32_t width;
  uint32_t height;

  // Buffer used to store texture.
  uint8_t* buffer;
};

YangPlayerApp *g_yang_player=nullptr;
YangRenderThread *g_yang_render=nullptr;


G_DEFINE_TYPE(YangTexture, my_texture, fl_pixel_buffer_texture_get_type())

// Implements GObject::dispose.
static void my_texture_dispose(GObject* object) {
  YangTexture* self = MY_TEXTURE(object);

  free(self->buffer);

  G_OBJECT_CLASS(my_texture_parent_class)->dispose(object);
}

// Implements FlPixelBufferTexture::copy_pixels.
static gboolean my_texture_copy_pixels(FlPixelBufferTexture* texture,
                                       const uint8_t** out_buffer,
                                       uint32_t* width, uint32_t* height,
                                       GError** error) {
  YangTexture* self = MY_TEXTURE(texture);
  *out_buffer = self->buffer;
  *width = self->width;
  *height = self->height;
  return TRUE;
}

static void my_texture_class_init(YangTextureClass* klass) {
  G_OBJECT_CLASS(klass)->dispose = my_texture_dispose;
  FL_PIXEL_BUFFER_TEXTURE_CLASS(klass)->copy_pixels = my_texture_copy_pixels;
}

static void my_texture_init(YangTexture* self) {}

YangTexture* my_texture_new(uint32_t width, uint32_t height, uint8_t r, uint8_t g,
                          uint8_t b) {
  YangTexture* self = MY_TEXTURE(g_object_new(my_texture_get_type(), nullptr));
  self->width = width;
  self->height = height;
  self->buffer = static_cast<uint8_t*>(malloc(self->width * self->height * 4));
  my_texture_set_color(self, r, g, b);
  return self;
}


void yang_player_init(int width,int height){
  if(g_yang_player==nullptr)
    g_yang_player=new YangPlayerApp();
  if(g_yang_render==nullptr){
    g_yang_render=new YangRenderThread();
    g_yang_render->m_player=g_yang_player;
    g_yang_render->m_playWidth=width;
    g_yang_render->m_playHeight=height;
    g_yang_render->start();
  }

}

void yang_player_destroy(){
  if(g_yang_render){
    delete g_yang_render;
    g_yang_render=nullptr;
  }
   if(g_yang_player){
      delete g_yang_player;
      g_yang_player=nullptr;
   }
}

int32_t yang_playRtc(char* url){
  if(g_yang_render) g_yang_render->m_isRender=true;
  if(g_yang_player) return g_yang_player->playRtc(url);
  return 1;
}
int32_t yang_closeRtc(){
   if(g_yang_render) g_yang_render->m_isRender=false;
  if(g_yang_player) return g_yang_player->close();
  return 1;
}

void my_texture_set_localurl(char* url,int urlLen){
  memset(url,0,urlLen);
  sprintf(url,"webrtc://%s:1985/live/livestream","192.168.3.5");
}
void my_texture_put_data(YangTexture* self, uint8_t* data,int width,int height){
  memcpy(self->buffer,data,self->width*self->height*4);
}
// Draws the texture with the requested color.
void my_texture_set_color(YangTexture* self, uint8_t r, uint8_t g, uint8_t b) {
  g_return_if_fail(MY_IS_TEXTURE(self));

  for (size_t y = 0; y < self->height; y++) {
    for (size_t x = 0; x < self->width; x++) {
      uint8_t* pixel = self->buffer + (y * self->width * 4) + (x * 4);
      pixel[0] = r;
      pixel[1] = g;
      pixel[2] = b;
      pixel[3] = 255;
    }
  }
}
