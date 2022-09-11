#include <yangutil/sys/YangWebsocket.h>
#include <libwebsockets.h>
#include <yangutil/yangtype.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangFile.h>
#include <yangutil/sys/YangVector.h>
#include <yangutil/sys/YangLibHandle.h>
#include <string.h>
#include <pthread.h>

#if Yang_Enable_Websockets
typedef struct{
	void (*yang_lws_set_log_level)(int level, lws_log_emit_t log_emit_function);
    int	(*yang_lws_write)(struct lws *wsi, unsigned char *buf, size_t len, enum lws_write_protocol protocol);
	int	(*yang_lws_callback_on_writable)(struct lws *wsi);
	int	(*yang_lws_http_client_read)(struct lws *wsi, char **buf, int *len);
	int (*yang_lws_add_http_header_by_token)(struct lws *wsi, enum lws_token_indexes token,
				     const unsigned char *value, int length,
				     unsigned char **p, unsigned char *end);
	void (*yang_lws_client_http_body_pending)(struct lws *wsi, int something_left_to_send);
	struct lws_context* (*yang_lws_create_context)(const struct lws_context_creation_info *info);

	struct lws* (*yang_lws_client_connect_via_info)(const struct lws_client_connect_info *ccinfo);
	void (*yang_lws_context_destroy)(struct lws_context *context);
	int	(*yang_lws_service)(struct lws_context *context, int timeout_ms);
	void (*yang_lws_cancel_service)(struct lws_context *context);
}YangLws;



typedef struct{
	int32_t isloop;
	int32_t isStart;
	pthread_t threadId;

	bool ssl;
	YangWebsocketCallback* callback;
	YangUrlData urlData;
    struct lws_context *wsicontext;
	struct lws *wsi;
	struct lws_context_creation_info info;
	struct lws_client_connect_info conn_info;

	YangLws lws;
	YangLibHandle lib;
}YangWebsocketContext;



int yang_websocket_callback(struct lws *wsi, enum lws_callback_reasons reason,
		void *user, void *in, size_t len) {
	if (user == NULL)
		return 0;
	YangWebsocketContext *obj = (YangWebsocketContext*) user;

	if(reason==LWS_CALLBACK_CLIENT_RECEIVE){
    // 接收到服务器数据后的回调，数据为in，其长度为len



		YangSample pdata;
		pdata.bytes = (char*) in;
		pdata.nb = len;

		if (obj->callback)	obj->callback->receive(obj->callback->context, &pdata);
		return Yang_Ok;
	}
	if(reason==LWS_CALLBACK_CLIENT_ESTABLISHED){
		yang_info("Connected to server ok!\n");
	}
	/**switch (reason) {
	case LWS_CALLBACK_CLIENT_ESTABLISHED:   // 连接到服务器后的回调
		yang_info("Connected to server ok!\n");
		break;
	case LWS_CALLBACK_CLIENT_RECEIVE:       // 接收到服务器数据后的回调，数据为in，其长度为len
	{
		//yang_trace("Rx: %s\n", (char* ) in);

		YangSample pdata;
		pdata.bytes = (char*) in;
		pdata.nb = len;

		if (obj->callback)	obj->callback->receive(obj->callback->context, &pdata);

		break;
	}
	case LWS_CALLBACK_CLIENT_WRITEABLE:     // 当此客户端可以发送数据时的回调
	{
		break;
	}

	}**/
	return Yang_Ok;
}


void yang_websocket_loadLib(void* pcontext){
	if(pcontext==NULL) return;
		YangWebsocketContext* context=(YangWebsocketContext*)pcontext;
		yang_create_libhandle(&context->lib);
		YangLibHandle* lib=&context->lib;
        lib->loadObject(lib->context,"libwebsockets");
        context->lws.yang_lws_set_log_level=(void (*)(int level, lws_log_emit_t log_emit_function))lib->loadFunction(lib->context,"lws_set_log_level");
        context->lws.yang_lws_create_context=(struct lws_context * (*)(const struct lws_context_creation_info *info))lib->loadFunction(lib->context,"lws_create_context");
        context->lws.yang_lws_write=(int	(*)(struct lws *wsi, unsigned char *buf, size_t len, enum lws_write_protocol protocol))lib->loadFunction(lib->context,"lws_write");
        context->lws.yang_lws_client_connect_via_info=(struct lws* (*)(const struct lws_client_connect_info *ccinfo))lib->loadFunction(lib->context,"lws_client_connect_via_info");
        context->lws.yang_lws_context_destroy=(void (*)(struct lws_context *context))lib->loadFunction(lib->context,"lws_context_destroy");
        context->lws.yang_lws_service=(int	(*)(struct lws_context *context, int timeout_ms))lib->loadFunction(lib->context,"lws_service");
        context->lws.yang_lws_cancel_service=(void (*)(struct lws_context *context))lib->loadFunction(lib->context,"lws_cancel_service");
        context->lws.yang_lws_callback_on_writable=(int	(*)(struct lws *wsi))lib->loadFunction(lib->context,"lws_callback_on_writable");
        context->lws.yang_lws_http_client_read=(int	(*)(struct lws *wsi, char **buf, int *len))lib->loadFunction(lib->context,"lws_http_client_read");
		context->lws.yang_lws_add_http_header_by_token=(int (*)(struct lws *wsi, enum lws_token_indexes token,
				     const unsigned char *value, int length,
                     unsigned char **p, unsigned char *end))lib->loadFunction(lib->context,"lws_add_http_header_by_token");
        context->lws.yang_lws_client_http_body_pending=(void (*)(struct lws *wsi, int something_left_to_send))lib->loadFunction(lib->context,"lws_client_http_body_pending");
}

void yang_websocket_unloadLib(void* pcontext){
	if(pcontext==NULL) return;
	YangWebsocketContext* context=(YangWebsocketContext*)pcontext;
	yang_destroy_libhandle(&context->lib);
}
int yang_websocket_init(void* pcontext) {
	if(pcontext==NULL) return 1;
	YangWebsocketContext* context=(YangWebsocketContext*)pcontext;
	//bool ssl = protocol == "wss" ? true : false; //确认是否进行SSL加密
    context->lws.yang_lws_set_log_level(7, NULL);


	memset(&context->info, 0, sizeof(context->info));
	/** const struct lws_protocols protocols[] = {
	                {"ws",yang_websocket_callback,0,2048
	                },

	                {NULL, NULL, 0, 0}};**/
	static const struct lws_protocols protocols[] = {
		{
			"ws-client",
			yang_websocket_callback,
			0, 4096, 0, NULL, 0
		},
		LWS_PROTOCOL_LIST_TERM
	};

	context->info.protocols = protocols;       //设置处理协议
	context->info.port = CONTEXT_PORT_NO_LISTEN; //作为ws客户端，无需绑定端口
	context->info.iface=NULL;
	//ws和wss的初始化配置不同
	context->info.options = context->ssl ? LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT : 0; //如果是wss，需要做全局SSL初始化
	//info.options |= LWS_SERVER_OPTION_H2_JUST_FIX_WINDOW_UPDATE_OVERFLOW;
	context->info.gid = (gid_t)-1;
	context->info.uid = (uid_t)-1;
	if(context->ssl){
		char cerpath[255]={0};
		char privatekey[255]={0};

		yang_getCaFile(cerpath,privatekey);

		context->info.client_ssl_cert_filepath=cerpath;
		context->info.client_ssl_private_key_filepath=privatekey;
		context->info.client_ssl_private_key_password="metaRTC";
		//info.client_ssl_ca_filepath=cerpath;
	}
    context->wsicontext = context->lws.yang_lws_create_context(&context->info); //创建连接上下文
    if (context->wsicontext == NULL) {
		yang_error("create context error\n");
		return 1;
	}

	memset(&context->conn_info, 0, sizeof(struct lws_client_connect_info));
	 char addr_port[256] = { 0 };
	 sprintf(addr_port, "%s:%u", context->urlData.server, context->urlData.port & 65535 );
	//初始化连接信息
     context->conn_info.context = context->wsicontext;      //设置上下文
	 context->conn_info.address = context->urlData.server; //设置目标主机IP
	 context->conn_info.port = context->urlData.port;            //设置目标主机服务端口
	 context->conn_info.path = "./";//context->urlData.app.c_str();    //设置目标主机服务PATH
	 context->conn_info.host = context->urlData.server;//addr_port;      //设置目标主机IP
	 context->conn_info.origin = context->urlData.server;//addr_port;    //设置目标主机IP
	//conn_info.pwsi = &context->wsi;            //设置wsi句柄
	 context->conn_info.userdata =pcontext;//NULL;      //userdata 指针会传递给callback的user参数，一般用作自定义变量传入
	 context->conn_info.protocol = protocols[0].name;
	//conn_info.ietf_version_or_minus_one=-1;
	//conn_info.method="POST";
	//ws/wss需要不同的配置
	int usessl= LCCSCF_USE_SSL |
						  LCCSCF_ALLOW_SELFSIGNED |
						  LCCSCF_SKIP_SERVER_CERT_HOSTNAME_CHECK;
	//LCCSCF_ALLOW_INSECURE)
	context->conn_info.ssl_connection =	context->ssl ? usessl:	0;
	context->wsi = context->lws.yang_lws_client_connect_via_info(&context->conn_info);

	if(context->wsi==NULL){
		return yang_error_wrap(1,"lws_client_connect_via_info error!");
	}


	return Yang_Ok;
}



void* yang_run_websocket_thread(void *obj) {
	YangWebsocketContext* context=(YangWebsocketContext*)obj;
	context->isStart=1;
	context->isloop=1;
	while (context->isloop) {
		//lws_service
        context->lws.yang_lws_service(context->wsicontext, 500);
      //  context->lws.yang_lws_callback_on_writable(context->wsi);
	}
	context->isStart=0;
	return NULL;
}
void yang_websocket_start(void* pcontext) {
	if(pcontext==NULL) return;
	YangWebsocketContext* context=(YangWebsocketContext*)pcontext;
	if (pthread_create(&context->threadId, 0, yang_run_websocket_thread, pcontext)) {
		yang_error("YangThread::start could not start thread");

	}

}
void yang_websocket_stop(void* pcontext) {
	if(pcontext==NULL) return;
	YangWebsocketContext* context=(YangWebsocketContext*)pcontext;
	context->isloop=0;
	int32_t waitCount=0;
	while (context->isStart)	{
		yang_usleep(1000);
		waitCount++;
		if(waitCount>500&&context->lws.yang_lws_cancel_service) context->lws.yang_lws_cancel_service(context->wsicontext);
	}
}
int32_t yang_websocket_connectServer(void* pcontext,char* url) {
	if(pcontext==NULL) return 1;
	YangWebsocketContext* context=(YangWebsocketContext*)pcontext;
	if(yang_ws_url_parse(url,&context->urlData)) return 1;
	yang_trace("\nserver==%s,port==%d,path==%s",context->urlData.server,context->urlData.port,context->urlData.app);
	context->ssl=(context->urlData.netType==Yang_Websocket_Wss||context->urlData.netType==Yang_Websocket_Https)?1:0;
	int err=Yang_Ok;
	if((err=yang_websocket_init(pcontext))==Yang_Ok) {
		yang_websocket_start(pcontext);
	}else{
		return yang_error_wrap(err,"websocket init error!");
	}
	return err;
}
int yang_websocket_sendData(void* pcontext,uint8_t *p, int nb) {
	if (pcontext == NULL)
		return 1;
	YangWebsocketContext *context = (YangWebsocketContext*) pcontext;
	char buf[2000];
	memset(buf, 0, 2000);
	//LWS_SEND_BUFFER_PRE_PADDING
	memcpy(buf + LWS_PRE, p, nb);
	//yang_trace("\nsend data(%d)=%s", nb, buf+LWS_PRE);

	context->lws.yang_lws_write(context->wsi, ((uint8_t*)buf) + LWS_PRE, nb,LWS_WRITE_TEXT);
	context->lws.yang_lws_callback_on_writable(context->wsi);

	return Yang_Ok;
}
int32_t yang_websocket_disconnectServer(void* pcontext) {
	if(pcontext==NULL) return 1;
	yang_websocket_stop(pcontext);
    YangWebsocketContext* context=(YangWebsocketContext*)pcontext;
    if(context->wsicontext) 	context->lws.yang_lws_context_destroy(context->wsicontext);
    context->wsicontext=NULL;
	return Yang_Ok;
}

void yang_create_websocket(YangWebsocket* web,YangWebsocketCallback* callback){
	if(web==NULL) return;
    web->context=calloc(sizeof(YangWebsocketContext),1);
    YangWebsocketContext* context=(YangWebsocketContext*)web->context;

	context->callback=callback;
    context->wsicontext = NULL;
	context->wsi=NULL;
	context->isloop=0;
	context->ssl=false;

    yang_websocket_loadLib(web->context);
	web->connectServer=yang_websocket_connectServer;
	web->disconnectServer=yang_websocket_disconnectServer;
	web->sendData=yang_websocket_sendData;
}
void yang_destroy_websocket(YangWebsocket* web){
	if(web==NULL) return;

    yang_websocket_disconnectServer(web->context);

    yang_websocket_unloadLib(web->context);
    yang_free(web->context);
}
#endif
