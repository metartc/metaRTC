#include "my_application.h"

#include <flutter_linux/flutter_linux.h>
#ifdef GDK_WINDOWING_X11
#include <gdk/gdkx.h>
#endif

#include "flutter/generated_plugin_registrant.h"
#include "yang_texture.h"
#include "yangplayer/YangPlayerDef.h"

struct _MyApplication {
  GtkApplication parent_instance;
  char** dart_entrypoint_arguments;
    // Channel to receive texture requests from Flutter.
  FlMethodChannel* texture_channel;

  // Texture we've created.
  YangTexture* texture;
  FlView* view;
  char url[256];
};

G_DEFINE_TYPE(MyApplication, my_application, GTK_TYPE_APPLICATION)

MyApplication *g_yang_application=nullptr;
// Handle request to create the texture.
static FlMethodResponse* handle_create(MyApplication* self,
                                       FlMethodCall* method_call) {
  if (self->texture != nullptr) {
    return FL_METHOD_RESPONSE(fl_method_error_response_new(
        "Error", "texture already created", nullptr));
  }

  FlValue* args = fl_method_call_get_args(method_call);
  if (fl_value_get_type(args) != FL_VALUE_TYPE_LIST ||
      fl_value_get_length(args) != 2) {
    return FL_METHOD_RESPONSE(fl_method_error_response_new(
        "Invalid args", "Invalid create args", nullptr));
  }
  FlValue* width_value = fl_value_get_list_value(args, 0);
  FlValue* height_value = fl_value_get_list_value(args, 1);
  if (fl_value_get_type(width_value) != FL_VALUE_TYPE_INT ||
      fl_value_get_type(height_value) != FL_VALUE_TYPE_INT) {
    return FL_METHOD_RESPONSE(fl_method_error_response_new(
        "Invalid args", "Invalid create args", nullptr));
  }

  FlEngine* engine = fl_view_get_engine(self->view);
  FlTextureRegistrar* texture_registrar =
      fl_engine_get_texture_registrar(engine);

  self->texture =
      my_texture_new(fl_value_get_int(width_value),
                     fl_value_get_int(height_value), 0x05, 0x53, 0xb1);
  if (!fl_texture_registrar_register_texture(texture_registrar,
                                             FL_TEXTURE(self->texture))) {
    return FL_METHOD_RESPONSE(fl_method_error_response_new(
        "Error", "Failed to register texture", nullptr));
  }

  yang_player_init(fl_value_get_int(width_value),fl_value_get_int(height_value));
  // Return the texture ID to Flutter so it can use this texture.
  g_autoptr(FlValue) id =
      fl_value_new_int(fl_texture_get_id(FL_TEXTURE(self->texture)));
      g_yang_application=self;
  return FL_METHOD_RESPONSE(fl_method_success_response_new(id));
}



static FlMethodResponse* yang_get_localUrl(MyApplication* self,
                                       FlMethodCall* method_call) {
//  FlEngine* engine = fl_view_get_engine(self->view);
  my_texture_set_localurl(self->url,sizeof(self->url));

  //g_autoptr(FlValue) url = fl_value_new_string_sized((const gchar*)self->url,strlen(self->url)+1);
  FlValue* url = fl_value_new_string("webrtc://192.168.3.5/live/livestream");


  return FL_METHOD_RESPONSE(fl_method_success_response_new(url));              
}


static FlMethodResponse* yang_cb_playRtc(MyApplication* self,
                                       FlMethodCall* method_call) {
//  FlEngine* engine = fl_view_get_engine(self->view);
  FlValue* args = fl_method_call_get_args(method_call);
  FlValue* url_value = fl_value_get_list_value(args, 0);
  const char* url=fl_value_get_string(url_value);
  printf("\n>>>>>>>>>>>>>>>>get param url==%s\n",url);
  yang_playRtc((char*)url);

  return FL_METHOD_RESPONSE(fl_method_success_response_new(nullptr));              
}
static FlMethodResponse* yang_cb_closeRtc(MyApplication* self,
                                       FlMethodCall* method_call) {
//  FlEngine* engine = fl_view_get_engine(self->view);

  yang_closeRtc();
  my_texture_set_color(self->texture, 0x05, 0x53, 0xb1);
  return FL_METHOD_RESPONSE(fl_method_success_response_new(nullptr));              
}

static FlMethodResponse* yang_cb_closeWindows(MyApplication* self,
                                       FlMethodCall* method_call) {
//  FlEngine* engine = fl_view_get_engine(self->view);

 
  return FL_METHOD_RESPONSE(fl_method_success_response_new(nullptr));              
}


// Handle request to set the texture color.
static FlMethodResponse* handle_set_color(MyApplication* self,
                                          FlMethodCall* method_call) {
  FlValue* args = fl_method_call_get_args(method_call);
  if (fl_value_get_type(args) != FL_VALUE_TYPE_LIST ||
      fl_value_get_length(args) != 3) {
    return FL_METHOD_RESPONSE(fl_method_error_response_new(
        "Invalid args", "Invalid setColor args", nullptr));
  }
  FlValue* r_value = fl_value_get_list_value(args, 0);
  FlValue* g_value = fl_value_get_list_value(args, 1);
  FlValue* b_value = fl_value_get_list_value(args, 2);
  if (fl_value_get_type(r_value) != FL_VALUE_TYPE_INT ||
      fl_value_get_type(g_value) != FL_VALUE_TYPE_INT ||
      fl_value_get_type(b_value) != FL_VALUE_TYPE_INT) {
    return FL_METHOD_RESPONSE(fl_method_error_response_new(
        "Invalid args", "Invalid setColor args", nullptr));
  }

  FlEngine* engine = fl_view_get_engine(self->view);
  FlTextureRegistrar* texture_registrar =
      fl_engine_get_texture_registrar(engine);

  // Redraw in requested color.
  my_texture_set_color(self->texture, fl_value_get_int(r_value),
                       fl_value_get_int(g_value), fl_value_get_int(b_value));

  // Notify Flutter the texture has changed.
  fl_texture_registrar_mark_texture_frame_available(texture_registrar,
                                                    FL_TEXTURE(self->texture));

  return FL_METHOD_RESPONSE(fl_method_success_response_new(nullptr));
}


void yang_player_refreshImg(uint8_t* data,int32_t width,int32_t height){
if(g_yang_application==nullptr) return;
FlEngine* engine = fl_view_get_engine(g_yang_application->view);
  FlTextureRegistrar* texture_registrar =
      fl_engine_get_texture_registrar(engine);

  // Redraw in requested color.
  //my_texture_set_color(self->texture, fl_value_get_int(r_value),
                      // fl_value_get_int(g_value), fl_value_get_int(b_value));
  my_texture_put_data(g_yang_application->texture, data,width,height);
  // Notify Flutter the texture has changed.
  fl_texture_registrar_mark_texture_frame_available(texture_registrar,
                                                    FL_TEXTURE(g_yang_application->texture));
}


void yang_player_refreshImg_background(){
  if(g_yang_application==nullptr) return;
FlEngine* engine = fl_view_get_engine(g_yang_application->view);
  FlTextureRegistrar* texture_registrar =
      fl_engine_get_texture_registrar(engine);

  // Redraw in requested color. 0x04, 0x2b, 0x59
 // my_texture_set_color(g_yang_application->texture, 4, 0x2b, 0x59);
 // my_texture_put_data(g_yang_application->texture, data,width,height);
  // Notify Flutter the texture has changed.
  fl_texture_registrar_mark_texture_frame_available(texture_registrar,
                                                    FL_TEXTURE(g_yang_application->texture));
}


// Handle texture requests from Flutter.
static void texture_channel_method_cb(FlMethodChannel* channel,
                                      FlMethodCall* method_call,
                                      gpointer user_data) {
  MyApplication* self = MY_APPLICATION(user_data);

  const char* name = fl_method_call_get_name(method_call);
  if (g_str_equal(name, "create")) {
    g_autoptr(FlMethodResponse) response = handle_create(self, method_call);
    fl_method_call_respond(method_call, response, NULL);
  } else if (g_str_equal(name, "setColor")) {
    g_autoptr(FlMethodResponse) response = handle_set_color(self, method_call);
    fl_method_call_respond(method_call, response, NULL);
  } else if (g_str_equal(name, "getLocalUrl")) {
   
     g_autoptr(FlMethodResponse) response = yang_get_localUrl(self, method_call);
     fl_method_call_respond(method_call, response, NULL);
  }else if (g_str_equal(name, "playRtc")) {
    g_autoptr(FlMethodResponse) response = yang_cb_playRtc(self, method_call);
    fl_method_call_respond(method_call, response, NULL);
  }else if (g_str_equal(name, "closeRtc")) {
    g_autoptr(FlMethodResponse) response = yang_cb_closeRtc(self, method_call);
    fl_method_call_respond(method_call, response, NULL);
  } else if (g_str_equal(name, "closeWindows")) {
    g_autoptr(FlMethodResponse) response = yang_cb_closeWindows(self, method_call);
    fl_method_call_respond(method_call, response, NULL);
  } else {
    fl_method_call_respond_not_implemented(method_call, NULL);
  }
}


// Implements GApplication::activate.
static void my_application_activate(GApplication* application) {
  MyApplication* self = MY_APPLICATION(application);
  GtkWindow* window =
      GTK_WINDOW(gtk_application_window_new(GTK_APPLICATION(application)));

  // Use a header bar when running in GNOME as this is the common style used
  // by applications and is the setup most users will be using (e.g. Ubuntu
  // desktop).
  // If running on X and not using GNOME then just use a traditional title bar
  // in case the window manager does more exotic layout, e.g. tiling.
  // If running on Wayland assume the header bar will work (may need changing
  // if future cases occur).
  gboolean use_header_bar = TRUE;
#ifdef GDK_WINDOWING_X11
  GdkScreen* screen = gtk_window_get_screen(window);
  if (GDK_IS_X11_SCREEN(screen)) {
    const gchar* wm_name = gdk_x11_screen_get_window_manager_name(screen);
    if (g_strcmp0(wm_name, "GNOME Shell") != 0) {
      use_header_bar = FALSE;
    }
  }
#endif
  if (use_header_bar) {
    GtkHeaderBar* header_bar = GTK_HEADER_BAR(gtk_header_bar_new());
    gtk_widget_show(GTK_WIDGET(header_bar));
    gtk_header_bar_set_title(header_bar, "metaplayer7_flutter");
    gtk_header_bar_set_show_close_button(header_bar, TRUE);
    gtk_window_set_titlebar(window, GTK_WIDGET(header_bar));
  } else {
    gtk_window_set_title(window, "metaplayer7_flutter");
  }

  gtk_window_set_default_size(window, 1280, 720);
  gtk_widget_show(GTK_WIDGET(window));

  g_autoptr(FlDartProject) project = fl_dart_project_new();
  fl_dart_project_set_dart_entrypoint_arguments(project, self->dart_entrypoint_arguments);

  self->view= fl_view_new(project);
  gtk_widget_show(GTK_WIDGET(self->view));
  gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(self->view));

  // Create channel to handle texture requests from Flutter.
  FlEngine* engine = fl_view_get_engine(self->view);
  g_autoptr(FlStandardMethodCodec) codec = fl_standard_method_codec_new();
  self->texture_channel = fl_method_channel_new(
      fl_engine_get_binary_messenger(engine), "yang_channel_texture",
      FL_METHOD_CODEC(codec));
  fl_method_channel_set_method_call_handler(
      self->texture_channel, texture_channel_method_cb, self, nullptr);

  fl_register_plugins(FL_PLUGIN_REGISTRY(self->view));

  gtk_widget_grab_focus(GTK_WIDGET(self->view));

}

// Implements GApplication::local_command_line.
static gboolean my_application_local_command_line(GApplication* application, gchar*** arguments, int* exit_status) {
  MyApplication* self = MY_APPLICATION(application);
  // Strip out the first argument as it is the binary name.
  self->dart_entrypoint_arguments = g_strdupv(*arguments + 1);

  g_autoptr(GError) error = nullptr;
  if (!g_application_register(application, nullptr, &error)) {
     g_warning("Failed to register: %s", error->message);
     *exit_status = 1;
     return TRUE;
  }

  g_application_activate(application);
  *exit_status = 0;

  return TRUE;
}

// Implements GObject::dispose.
static void my_application_dispose(GObject* object) {
  yang_player_destroy();

  MyApplication* self = MY_APPLICATION(object);
  g_clear_pointer(&self->dart_entrypoint_arguments, g_strfreev);
  G_OBJECT_CLASS(my_application_parent_class)->dispose(object);
}

static void my_application_class_init(MyApplicationClass* klass) {
  G_APPLICATION_CLASS(klass)->activate = my_application_activate;
  G_APPLICATION_CLASS(klass)->local_command_line = my_application_local_command_line;
  G_OBJECT_CLASS(klass)->dispose = my_application_dispose;
}

static void my_application_init(MyApplication* self) {}

MyApplication* my_application_new() {
  return MY_APPLICATION(g_object_new(my_application_get_type(),
                                     "application-id", APPLICATION_ID,
                                     "flags", G_APPLICATION_NON_UNIQUE,
                                     nullptr));
}
