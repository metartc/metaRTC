
#define _DEFAULT_SOURCE /* needed for usleep() */
#include <stdlib.h>
#include <unistd.h>
#define SDL_MAIN_HANDLED /*To fix SDL's "undefined reference to WinMain" issue*/
#include <SDL2/SDL.h>
#include <yangutil/sys/YangThread.h>
#include "YangMetaplayer.h"
extern "C"{
	#include "lvgl/lvgl.h"
	#include "lv_drivers/sdl/sdl.h"
	#include <libyuv.h>
}

typedef struct{
	yangbool isStartPlay;
	int32_t width;
	int32_t height;
	int32_t imgobj_width;
	int32_t imgobj_height;
	lv_obj_t * cont;
	lv_obj_t * cont_row1;
	lv_obj_t * cont_row2;
	lv_obj_t * label;
	lv_obj_t* url;
	lv_obj_t* play_label;
	lv_obj_t* play;
	lv_obj_t* img;
	lv_img_dsc_t * img_dsc;
	//void* user;
	uint8_t* rgbaBuffer;
	uint8_t* imgBuffer;
	yang_thread_mutex_t mutex;
}YangLvglUI;
YangLvglUI* g_yangui=NULL;
void yang_lvgl_render(uint8_t* data,int32_t width,int32_t height,yangbool isInit);
static void yang_hal_init(void)
{
  /* Use the 'monitor' driver which creates window on PC's monitor to simulate a display*/
  sdl_init();

  /*Create a display buffer*/
  static lv_disp_draw_buf_t disp_buf1;
  static lv_color_t buf1_1[SDL_HOR_RES * SDL_VER_RES];
  lv_disp_draw_buf_init(&disp_buf1, buf1_1, NULL, SDL_HOR_RES * SDL_VER_RES);

  /*Create a display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv); /*Basic initialization*/
  disp_drv.draw_buf = &disp_buf1;
  disp_drv.flush_cb = sdl_display_flush;
  disp_drv.hor_res = SDL_HOR_RES;
  disp_drv.ver_res = SDL_VER_RES;

  lv_disp_t * disp = lv_disp_drv_register(&disp_drv);

  lv_theme_t * th = lv_theme_default_init(disp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), LV_THEME_DEFAULT_DARK, LV_FONT_DEFAULT);
  lv_disp_set_theme(disp, th);

  lv_group_t * g = lv_group_create();
  lv_group_set_default(g);

  /* Add the mouse as input device
   * Use the 'mouse' driver which reads the PC's mouse*/
  static lv_indev_drv_t indev_drv_1;
  lv_indev_drv_init(&indev_drv_1); /*Basic initialization*/
  indev_drv_1.type = LV_INDEV_TYPE_POINTER;

  /*This function will be called periodically (by the library) to get the mouse position and state*/
  indev_drv_1.read_cb = sdl_mouse_read;
  lv_indev_t *mouse_indev = lv_indev_drv_register(&indev_drv_1);

  static lv_indev_drv_t indev_drv_2;
  lv_indev_drv_init(&indev_drv_2); /*Basic initialization*/
  indev_drv_2.type = LV_INDEV_TYPE_KEYPAD;
  indev_drv_2.read_cb = sdl_keyboard_read;
  lv_indev_t *kb_indev = lv_indev_drv_register(&indev_drv_2);
  lv_indev_set_group(kb_indev, g);

  static lv_indev_drv_t indev_drv_3;
  lv_indev_drv_init(&indev_drv_3); /*Basic initialization*/
  indev_drv_3.type = LV_INDEV_TYPE_ENCODER;
  indev_drv_3.read_cb = sdl_mousewheel_read;
  lv_indev_t * enc_indev = lv_indev_drv_register(&indev_drv_3);
  lv_indev_set_group(enc_indev, g);

  /*Set a cursor for the mouse*/
  LV_IMG_DECLARE(mouse_cursor_icon); /*Declare the image file.*/
  lv_obj_t * cursor_obj = lv_img_create(lv_scr_act()); /*Create an image object for the cursor */
  lv_img_set_src(cursor_obj, &mouse_cursor_icon);           /*Set the image source*/
  lv_indev_set_cursor(mouse_indev, cursor_obj);             /*Connect the image  object to the driver*/
}


static void textarea_event_handler(lv_event_t * e)
{
    lv_obj_t * ta = lv_event_get_target(e);
    LV_LOG_USER("Enter was pressed. The current text is: %s", lv_textarea_get_text(ta));
}

static void play_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_CLICKED) {
    	g_yangui->isStartPlay=yangtrue;

    }
    else if(code == LV_EVENT_VALUE_CHANGED) {

    }
}
void yang_lvgl_render_init(int32_t width,int32_t height,yangbool isplaying);

static void yang_widget_init(){
	int32_t width=SDL_HOR_RES;
	int32_t height=SDL_VER_RES;
	int32_t row1_height=80;

	if(g_yangui->cont==NULL){
		g_yangui->cont = lv_obj_create(lv_scr_act());
		lv_obj_set_size(g_yangui->cont, width, height);
		lv_obj_center(g_yangui->cont);
		lv_obj_set_flex_flow(g_yangui->cont, LV_FLEX_FLOW_COLUMN);
	}
	if(g_yangui->cont_row1==NULL){
		g_yangui->cont_row1 = lv_obj_create(g_yangui->cont);
		lv_obj_set_width(g_yangui->cont_row1,width);
		lv_obj_set_height(g_yangui->cont_row1,row1_height);
	}

	if(g_yangui->cont_row2==NULL){
		g_yangui->cont_row2 = lv_obj_create(g_yangui->cont);
		lv_obj_set_size(g_yangui->cont_row2,width,height-row1_height);

	}
	if(g_yangui->label==NULL){
		g_yangui->label = lv_label_create(g_yangui->cont_row1);
		lv_label_set_long_mode(g_yangui->label, LV_LABEL_LONG_WRAP);
		lv_label_set_recolor(g_yangui->label, true);
		lv_label_set_text(g_yangui->label, "URL:");
		lv_obj_set_width(g_yangui->label, 50);
		lv_obj_set_style_text_align(g_yangui->label, LV_TEXT_ALIGN_CENTER, 0);
		lv_obj_align(g_yangui->label, LV_ALIGN_BOTTOM_LEFT, 0, 0);
	}
	if(g_yangui->url==NULL){
		g_yangui->url = lv_textarea_create(g_yangui->cont_row1);
		lv_textarea_set_one_line(g_yangui->url, true);
		lv_obj_align(g_yangui->url, LV_ALIGN_TOP_LEFT, 50, 0);
		lv_obj_set_width(g_yangui->url,500);
		lv_obj_add_event_cb(g_yangui->url, textarea_event_handler, LV_EVENT_READY, g_yangui->url);
		lv_obj_add_state(g_yangui->url, LV_STATE_FOCUSED);
	}
	if(g_yangui->play==NULL){
		g_yangui->play = lv_btn_create(g_yangui->cont_row1);
	    lv_obj_add_event_cb(g_yangui->play, play_event_handler, LV_EVENT_ALL, NULL);
	    lv_obj_align(g_yangui->play, LV_ALIGN_TOP_LEFT, 550, 0);

	    g_yangui->play_label = lv_label_create(g_yangui->play);
	    lv_label_set_text(g_yangui->play_label, "play");
	    lv_obj_center(g_yangui->play_label);
	}
	if(g_yangui->img==NULL){
		g_yangui->img = lv_img_create(g_yangui->cont_row2);

	    lv_obj_align(g_yangui->img, LV_ALIGN_TOP_MID, 0, 0);
	    lv_obj_set_size(g_yangui->img, lv_obj_get_width(g_yangui->cont_row2), lv_obj_get_height(g_yangui->cont_row2));

	    g_yangui->img_dsc = (lv_img_dsc_t*)lv_mem_alloc(sizeof(lv_img_dsc_t));
	    lv_memset_00(g_yangui->img_dsc, sizeof(lv_img_dsc_t));
	}
}

void yang_lvgl_render_init(int32_t width,int32_t height,yangbool isPlaying){
	uint8_t* p=NULL;
			uint8_t* tmp=NULL;
	if(g_yangui->width==0){
		g_yangui->width=width;
		g_yangui->height=height;
		g_yangui->imgobj_width=lv_obj_get_width(g_yangui->img);
		g_yangui->imgobj_height=lv_obj_get_height(g_yangui->img);

		if(g_yangui->rgbaBuffer==NULL)
			g_yangui->rgbaBuffer=(uint8_t*)yang_malloc(width*height*4);
		if(g_yangui->imgBuffer==NULL)
			g_yangui->imgBuffer=(uint8_t*)yang_malloc(g_yangui->imgobj_width*g_yangui->imgobj_height*4);


		yang_lvgl_render(g_yangui->rgbaBuffer,width,height,yangtrue);
	}else{
		if(g_yangui->width!=width){
			g_yangui->width=width;
			g_yangui->height=height;
			yang_free(g_yangui->rgbaBuffer);
			if(isPlaying){
				g_yangui->rgbaBuffer=(uint8_t*)yang_malloc(width*height*4);

				yang_lvgl_render(g_yangui->rgbaBuffer,g_yangui->imgobj_width,g_yangui->imgobj_height,yangtrue);
			}else{
				yang_lvgl_render(NULL,width,height,yangtrue);
			}
		}
	}
}

void yang_lvgl_render(uint8_t* data,int32_t width,int32_t height,yangbool isInit){

    yang_thread_mutex_lock(&g_yangui->mutex);

    g_yangui->img_dsc->data = (const uint8_t*)data;
 	if(isInit)		{
 		g_yangui->img_dsc->header.w = width;
 		g_yangui->img_dsc->header.h = height;
 		g_yangui->img_dsc->data_size = width*height*4;
 	    //LV_IMG_CF_TRUE_COLOR_ALPHA; LV_IMG_CF_TRUE_COLOR;
 		g_yangui->img_dsc->header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA;
 	 	lv_img_cache_invalidate_src(lv_img_get_src(g_yangui->img));
 		lv_img_set_src(g_yangui->img, g_yangui->img_dsc);
 		lv_obj_invalidate(g_yangui->img );
 	}else{
 	  lv_img_cache_invalidate_src(lv_img_get_src(g_yangui->img));
 	  lv_obj_invalidate(g_yangui->img );
 	}
	yang_thread_mutex_unlock(&g_yangui->mutex);
}


void yang_lvgl_refreshImg(uint8_t* data,int32_t nb,int32_t width,int32_t height){
	if(g_yangui->img==NULL||data==NULL) return;
	yang_lvgl_render_init(width,height,yangtrue);

	libyuv::I420ToARGB((const uint8_t*) data, width,
			(const uint8_t*) (data + (width * height)), (width >> 1),
			(const uint8_t*) (data + (width * height) + (int) (width * height / 4)),width >> 1,
			(uint8_t*)g_yangui->rgbaBuffer,width * 4,
			width, height);
	libyuv::ARGBScale((const uint8_t*)g_yangui->rgbaBuffer,width*4,
		width,height,
		g_yangui->imgBuffer,g_yangui->imgobj_width*4,
		g_yangui->imgobj_width,g_yangui->imgobj_height,
		libyuv::kFilterNone
		);

	yang_lvgl_render(g_yangui->imgBuffer,g_yangui->imgobj_width,g_yangui->imgobj_height,yangfalse);
}


void yang_lvgl_refreshImg_background(uint8_t* data,int32_t nb,int32_t width,int32_t height){
	lv_img_dsc_t * img_dsc=g_yangui->img_dsc;
	if(g_yangui->img==NULL||data==NULL) return;
	yang_lvgl_render_init(width,height,yangfalse);
	yang_lvgl_render(data,width,height,yangfalse);
}

char* yang_lvgl_get_urlstr(){
	if(g_yangui&&g_yangui->url)
			return (char*)lv_textarea_get_text(g_yangui->url);
	return NULL;
}
void yang_lvgl_setPlayButtonStr(char* str){
	if(g_yangui&&g_yangui->play_label)
		lv_label_set_text(g_yangui->play_label, str);
}
void yang_lvgl_setUrlText(char* str){
	if(g_yangui&&g_yangui->url)
			lv_textarea_set_text(g_yangui->url, str);
}

int main(int argc, char **argv)
{
  (void)argc; /*Unused*/
  (void)argv; /*Unused*/
  YangMetaplayer *player=NULL;

  if(g_yangui==NULL)
	  g_yangui= (YangLvglUI*)yang_calloc(sizeof(YangLvglUI),1);
  yang_thread_mutex_init(&g_yangui->mutex,NULL);
 // g_yangui->user=player;
  lv_init();
  yang_hal_init();
  yang_widget_init();
  player=new YangMetaplayer();
  YangRecordThread videoThread;

  player->m_videoThread=&videoThread;
  player->initVideoThread(&videoThread);

  videoThread.start();
  while(1) {
	  if(g_yangui->isStartPlay){
		  g_yangui->isStartPlay=false;
		  if(player) player->on_m_b_play_clicked();
	  }
      lv_tick_inc(5) ;
      yang_thread_mutex_lock(&g_yangui->mutex);
      lv_timer_handler();
      yang_thread_mutex_unlock(&g_yangui->mutex);
      yang_usleep(5 * 1000);
  }
  yang_delete(player);
  lv_deinit();
  if(g_yangui->img_dsc) lv_mem_free(g_yangui->img_dsc);
  yang_free(g_yangui->rgbaBuffer);
  yang_thread_mutex_destroy(&g_yangui->mutex);
  yang_free(g_yangui);

  return 0;
}

