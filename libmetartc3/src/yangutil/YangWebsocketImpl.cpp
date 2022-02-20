
#include <yangutil/yangtype.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangFile.h>
#include "YangWebsocketImpl.h"

#if HaveWebsockets
YangWebsocket* YangWebsocket::getWebsocket(){
    return new YangWebsocketImpl();
}
int yang_websocket_callback(struct lws *wsi, enum lws_callback_reasons reason,
		void *user, void *in, size_t len) {
	YangWebsocketImpl *obj = (struct YangWebsocketImpl*) user;

	if(obj==NULL) return 0;

	switch (reason) {
	case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
	{
		yang_error("Connected to server fail!\n");
		if(in){
			yang_error("fail reason==%s",(uint8_t*)in);
		}
		break;
	}
	case LWS_CALLBACK_CLIENT_ESTABLISHED:   // 连接到服务器后的回调
		yang_info("Connected to server ok!\n");
		break;
	case LWS_CALLBACK_ESTABLISHED_CLIENT_HTTP:
		yang_info("Connected to http server ok!\n");
		break;
	case LWS_CALLBACK_CLIENT_RECEIVE:       // 接收到服务器数据后的回调，数据为in，其长度为len
	{
		yang_trace("Rx: %s\n", (char* ) in);
        /**
		YangWebsocketData pdata;
		pdata.payload=(uint8_t*)in;
		pdata.nb=len;
		yang_trace("\nresponse(%d)==%s",len,(uint8_t*)in);
		if(obj->getReceiveCallback()) obj->getReceiveCallback()->receive(&pdata);
    **/
		break;
	}

	case LWS_CALLBACK_CLIENT_WRITEABLE:     // 当此客户端可以发送数据时的回调
	{
		yang_trace("\n*****LWS_CALLBACK_CLIENT_WRITEABLE ");
		if(obj->m_messages.size()>=0) break;

		uint8_t buf[2048];

		for (size_t i=0;i<obj->m_messages.size();i++) {

			memset(buf,0,2048);
			YangWebsocketData* data=obj->m_messages.front();
		//	char *msg = (char*) buf+LWS_PRE;

			memcpy(buf+LWS_PRE,data->payload,data->nb);
			yang_trace("\nsend data(%d)=%s",data->nb,buf+LWS_PRE);
			obj->yang_lws_write(wsi, buf+LWS_PRE, data->nb,LWS_WRITE_TEXT);
			//memcpy(buf,data->payload,data->nb);
			//obj->yang_lws_write(wsi, buf, data->nb, LWS_WRITE_HTTP);

			delete data;
			obj->m_messages.erase(obj->m_messages.begin());
		}
		break;
	}



	}
	return Yang_Ok;
}
YangWebsocketImpl::YangWebsocketImpl() {
	m_context = NULL;
	m_wsi=NULL;
	m_loop=0;
	m_ssl=false;
	m_lib.loadObject("libwebsockets");
	loadLib();
}

YangWebsocketImpl::~YangWebsocketImpl() {
	disconnectServer();
}

void YangWebsocketImpl::loadLib(){
	yang_lws_set_log_level=(void (*)(int level, lws_log_emit_t log_emit_function))m_lib.loadFunction("lws_set_log_level");
	yang_lws_create_context=(struct lws_context * (*)(const struct lws_context_creation_info *info))m_lib.loadFunction("lws_create_context");
	yang_lws_write=(int	(*)(struct lws *wsi, unsigned char *buf, size_t len, enum lws_write_protocol protocol))m_lib.loadFunction("lws_write");
	yang_lws_client_connect_via_info=(struct lws* (*)(const struct lws_client_connect_info *ccinfo))m_lib.loadFunction("lws_client_connect_via_info");
	yang_lws_context_destroy=(void (*)(struct lws_context *context))m_lib.loadFunction("lws_context_destroy");
	yang_lws_service=(int	(*)(struct lws_context *context, int timeout_ms))m_lib.loadFunction("lws_service");
	yang_lws_callback_on_writable=(int	(*)(struct lws *wsi))m_lib.loadFunction("lws_callback_on_writable");
	yang_lws_http_client_read=(int	(*)(struct lws *wsi, char **buf, int *len))m_lib.loadFunction("lws_http_client_read");
	yang_lws_add_http_header_by_token=(int (*)(struct lws *wsi, enum lws_token_indexes token,
				     const unsigned char *value, int length,
				     unsigned char **p, unsigned char *end))m_lib.loadFunction("lws_add_http_header_by_token");
	yang_lws_client_http_body_pending=(void (*)(struct lws *wsi, int something_left_to_send))m_lib.loadFunction("lws_client_http_body_pending");
}

void YangWebsocketImpl::unloadLib(){
	yang_lws_set_log_level=NULL;
	yang_lws_create_context=NULL;
	yang_lws_write=NULL;
	yang_lws_client_connect_via_info=NULL;
	yang_lws_context_destroy=NULL;
	yang_lws_service=NULL;
	yang_lws_callback_on_writable=NULL;
	yang_lws_http_client_read=NULL;
	yang_lws_add_http_header_by_token=NULL;
	yang_lws_client_http_body_pending=NULL;
}
int YangWebsocketImpl::init() {
	//bool ssl = protocol == "wss" ? true : false; //确认是否进行SSL加密
    yang_lws_set_log_level(7, NULL);


	memset(&info, 0, sizeof(info));
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

	info.protocols = protocols;       //设置处理协议
	info.port = CONTEXT_PORT_NO_LISTEN; //作为ws客户端，无需绑定端口
	//ws和wss的初始化配置不同
	info.options = m_ssl ? LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT : 0; //如果是wss，需要做全局SSL初始化
	//info.options |= LWS_SERVER_OPTION_H2_JUST_FIX_WINDOW_UPDATE_OVERFLOW;
	info.gid = (gid_t)-1;
	info.uid = (uid_t)-1;

	char cerpath[255]={0};
	char privatekey[255]={0};

	yang_getCaFile(cerpath,privatekey);

	info.client_ssl_cert_filepath=cerpath;
	info.client_ssl_private_key_filepath=privatekey;
	info.client_ssl_private_key_password="yangrtc";
	//info.client_ssl_ca_filepath=cerpath;
	m_context = yang_lws_create_context(&info); //创建连接上下文
	if (m_context == NULL) {
		yang_error("create context error\n");
		return 1;
	}

	memset(&conn_info, 0, sizeof(struct lws_client_connect_info));
	 char addr_port[256] = { 0 };
	 sprintf(addr_port, "%s:%u", m_urlData.server.c_str(), m_urlData.port & 65535 );
	//初始化连接信息
	conn_info.context = m_context;      //设置上下文
	conn_info.address = m_urlData.server.c_str(); //设置目标主机IP
	conn_info.port = m_urlData.port;            //设置目标主机服务端口
	conn_info.path = "./";//m_urlData.app.c_str();    //设置目标主机服务PATH
	conn_info.host = m_urlData.server.c_str();//addr_port;      //设置目标主机IP
	conn_info.origin = m_urlData.server.c_str();//addr_port;    //设置目标主机IP
	//conn_info.pwsi = &m_wsi;            //设置wsi句柄
	conn_info.userdata =(void*)this;//NULL;      //userdata 指针会传递给callback的user参数，一般用作自定义变量传入
	conn_info.protocol = protocols[0].name;
	//conn_info.ietf_version_or_minus_one=-1;


	//conn_info.method="POST";
	//ws/wss需要不同的配置
	int usessl= LCCSCF_USE_SSL |
						  LCCSCF_ALLOW_SELFSIGNED |
						  LCCSCF_SKIP_SERVER_CERT_HOSTNAME_CHECK;
	//LCCSCF_ALLOW_INSECURE)
	conn_info.ssl_connection =	m_ssl ? usessl:	0;
	m_wsi = yang_lws_client_connect_via_info(&conn_info);

	if(m_wsi==NULL){
		return yang_error_wrap(1,"lws_client_connect_via_info error!");
	}
	//yang_trace("\ninit ws sucess!");
	// lws_client_connect_via_info(&ci); //使连接信息生效

	return Yang_Ok;
}
int YangWebsocketImpl::connectServer(std::string url) {

	if(yang_ws_url_parse((char*)url.c_str(),&m_urlData)) return 1;
	yang_trace("\nserver==%s,port==%d,path==%s",m_urlData.server.c_str(),m_urlData.port,m_urlData.app.c_str());
	m_ssl=(m_urlData.netType==Yang_Websocket_Wss||m_urlData.netType==Yang_Websocket_Https);
	int err=Yang_Ok;
	if((err=init())==Yang_Ok) {
		start();
	}else{
		return yang_error_wrap(err,"websocket init error!");
	}
	return err;
}
int YangWebsocketImpl::disconnectServer() {
	stop();
	yang_stop_thread(this);
//	yang_delete(m_wsi);
	if(m_context) 	yang_lws_context_destroy(m_context);
	m_context=NULL;
	return Yang_Ok;
}

int YangWebsocketImpl::sendData(uint8_t *p, int nb) {
	m_messages.push_back(new YangWebsocketData());
    memcpy(m_messages.back()->payload,p,nb);
	m_messages.back()->nb=nb;
	yang_lws_callback_on_writable(m_wsi);
//	yang_trace("\nsend data len==%d",nb);
	return Yang_Ok;
}

void YangWebsocketImpl::startLoop() {
	m_loop=1;

	while (m_loop) {

		yang_lws_service(m_context, 500);
		//yang_lws_callback_on_writable(m_wsi);
	}

}
void YangWebsocketImpl::stopLoop() {
	m_loop=0;
}
#endif
