#ifndef SRC_YANGUTIL_YANGWEBSOCKETIMPL_H_
#define SRC_YANGUTIL_YANGWEBSOCKETIMPL_H_
#include <yangutil/sys/YangWebsocket.h>
#include <yangutil/sys/YangLoadLib.h>
#include <libwebsockets.h>
#include <vector>
#if HaveWebsockets
class YangWebsocketImpl :public YangWebsocket{
public:
	YangWebsocketImpl();
	virtual ~YangWebsocketImpl();
	int init();
	int connectServer(std::string url);
	int disconnectServer();

	int sendData(uint8_t* p,int nb);


	std::vector<YangWebsocketData*> m_messages;
	void (*yang_lws_set_log_level)(int level, lws_log_emit_t log_emit_function);
    int	(*yang_lws_write)(struct lws *wsi, unsigned char *buf, size_t len, enum lws_write_protocol protocol);
	int	(*yang_lws_callback_on_writable)(struct lws *wsi);
	int	(*yang_lws_http_client_read)(struct lws *wsi, char **buf, int *len);
	int (*yang_lws_add_http_header_by_token)(struct lws *wsi, enum lws_token_indexes token,
				     const unsigned char *value, int length,
				     unsigned char **p, unsigned char *end);
	void (*yang_lws_client_http_body_pending)(struct lws *wsi, int something_left_to_send);
protected:

	void startLoop();
	void stopLoop();


private:
	int m_loop;
	bool m_ssl;
	struct lws_context *m_context;
	struct lws *m_wsi;
	struct lws_context_creation_info info;
	struct lws_client_connect_info conn_info;
private:
	YangLoadLib m_lib;
	void loadLib();
	void unloadLib();
	struct lws_context* (*yang_lws_create_context)(const struct lws_context_creation_info *info);

	struct lws* (*yang_lws_client_connect_via_info)(const struct lws_client_connect_info *ccinfo);
	void (*yang_lws_context_destroy)(struct lws_context *context);
	int	(*yang_lws_service)(struct lws_context *context, int timeout_ms);

};

#endif
#endif /* SRC_YANGUTIL_YANGWEBSOCKETIMPL_H_ */
