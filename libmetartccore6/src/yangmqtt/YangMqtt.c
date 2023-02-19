//
// Copyright (c) 2019-2023 yanggaofeng
//
#include <yangmqtt/YangMqtt.h>

#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangCString.h>

#include "YangMqttImpl.h"

typedef struct {
	yang_socket_t sockfd;
	yangbool isLoop;
	yangbool isStart;

	YangAVInfo* avinfo;
	YangMqttCallback callback;
	yang_mqtt_client client;
	uint8_t sendbuf[2048];
	uint8_t recvbuf[1024];
} YangMqttSession;

void yang_mqttc_publish_callback(void *state, yang_mqtt_response_publish *published) {
	if(state==NULL||published==NULL) return;
	YangMqttSession *session=(YangMqttSession*)state;
	char* topic=(char*)yang_malloc(published->topic_name_size+1);
	char* msg=(char*)yang_malloc(published->application_message_size+1);
	msg[published->application_message_size]=0x00;
	topic[published->topic_name_size]=0x00;
	yang_memcpy(topic,published->topic_name,published->topic_name_size);
	yang_memcpy(msg,published->application_message,published->application_message_size);
	if(session->callback.mqtt_receive)
		session->callback.mqtt_receive(session->callback.session,topic,msg);
	yang_free(topic);
	yang_free(msg);
}

void* yang_mqttc_rtcrecv_start_thread(void *obj) {

	YangMqttSession *session=(YangMqttSession*)obj;
	session->isStart=yangtrue;
	session->isLoop=yangtrue;
	while (session->isLoop) {

		if(yang_mqtt_sync(&session->client)!=MQTT_OK){
			yang_error("mqtt syc fail!");
		}
		yang_usleep(100 * 1000);
	}
	session->isStart=yangfalse;
	return NULL;
}

int32_t yang_mqttc_connect(void* psession,char *remoteIp,int32_t remotePort,char* username,char* password) {
	if(psession == NULL || remoteIp == NULL) return 1;
	YangMqttSession *session=(YangMqttSession*)psession;

	session->sockfd = yang_socket_create(session->avinfo->sys.familyType, Yang_Socket_Protocol_Tcp);;
	if (session->sockfd == -1) {
		 yang_error("Failed to open socket");
		 return 1;
	}

	YangIpAddress addr;
	yang_addr_set(&addr,remoteIp,remotePort,session->avinfo->sys.familyType,Yang_Socket_Protocol_Tcp);
	
	if (yang_socket_connect(session->sockfd, &addr) == -1) {
		yang_socket_close(session->sockfd);
		session->sockfd = -1;
		return yang_error_wrap(ERROR_SOCKET, "mqtt connect socket error(%d)", GetSockError());	
	}

	yang_socket_setNonblock(session->sockfd);

	yang_mqtt_init(&session->client, session->sockfd, session->sendbuf, sizeof(session->sendbuf),
			session->recvbuf, sizeof(session->recvbuf), yang_mqttc_publish_callback);

	uint8_t connect_flags = MQTT_CONNECT_CLEAN_SESSION;
	session->client.publish_response_callback_state=session;
	char clientId[13]={0};
	yang_cstr_random(12,clientId);
	int32_t err=yang_mqtt_connect(&session->client, clientId, NULL, NULL, 0, username, password,
			connect_flags, 400);
	if(err!=MQTT_OK){
		yang_error("\nmqtt_connect fail(%d)",err);
	}


	if (session->client.error != MQTT_OK) {
		yang_error( "error: %s", yang_mqtt_error_str(session->client.error));
		return 1;
	}

	yang_thread_t client_daemon;
	if (yang_thread_create(&client_daemon, NULL, yang_mqttc_rtcrecv_start_thread,
			session)) {
		yang_error( "Failed to start client daemon.");
		return 1;
	}



	return Yang_Ok;
}

int32_t yang_mqttc_subscribe(void* psession,char* topic){
	if(psession==NULL) return 1;
	YangMqttSession* session=(YangMqttSession*)psession;
	if(session->sockfd>-1)
		return yang_mqtt_subscribe(&session->client, topic, 0)==MQTT_OK?Yang_Ok:1;
	return 1;
}

int32_t yang_mqttc_unsubscribe(void* psession,char* topic){
	if(psession==NULL) return 1;
	YangMqttSession* session=(YangMqttSession*)psession;
	if(session->sockfd>-1)
		return yang_mqtt_unsubscribe(&session->client, (const char*)topic)==MQTT_OK?Yang_Ok:1;
	return 1;
}

int32_t yang_mqttc_close(void* psession){
	if(psession==NULL) return 1;
	YangMqttSession* session=(YangMqttSession*)psession;

	if(session->isStart){
		session->isLoop=yangfalse;
		while(session->isStart){
			yang_usleep(10*1000);
		}

	}

	if(session->sockfd>-1){
		yang_mqtt_disconnect(&session->client);
		yang_socket_close(session->sockfd);
	}
	session->sockfd=-1;

	return Yang_Ok;
}

int32_t yang_mqttc_publish(void* psession,char* topic,char* msg){
	if(psession==NULL || msg==NULL) return 1;
	YangMqttSession* session=(YangMqttSession*)psession;
	yang_mqtt_publish(&session->client, topic, msg, strlen(msg) + 1, MQTT_PUBLISH_QOS_0);
	return Yang_Ok;
}

int32_t yang_create_mqtt(YangMqtt* mqtt,YangAVInfo* avinfo,YangMqttCallback* mqttcb){
	if(mqtt==NULL || avinfo==NULL || mqttcb==NULL) return 1;
	YangMqttSession* session=(YangMqttSession*)yang_calloc(sizeof(YangMqttSession),1);

	session->avinfo=avinfo;
	session->callback.session=mqttcb->session;
	session->callback.mqtt_receive=mqttcb->mqtt_receive;
	session->isLoop=yangfalse;
	session->isStart=yangfalse;
	session->sockfd=-1;

	mqtt->session=session;
	mqtt->connect=yang_mqttc_connect;
	mqtt->close=yang_mqttc_close;
	mqtt->publish=yang_mqttc_publish;
	mqtt->subscribe=yang_mqttc_subscribe;
	mqtt->unsubscribe=yang_mqttc_unsubscribe;

	return Yang_Ok;
}

void yang_destroy_mqtt(YangMqtt* mqtt){
	if(mqtt==NULL) return;
	yang_mqttc_close(mqtt->session);
	yang_free(mqtt->session);
}



#ifdef _WIN32

int32_t yang_mqtt_sendmsg(yang_socket_t fd, const void* buf, int32_t len) {
	int32_t sent = 0;
	while (sent < len) {
		int32_t tmp = yang_socket_send(fd, (char*)buf + sent, len - sent);
		if (tmp < 1) {
			return MQTT_ERROR_SOCKET_ERROR;
		}
		sent += tmp;
	}
	return sent;
}
int32_t yang_mqtt_recvmsg(yang_socket_t fd, void* buf, int32_t bufsz, int32_t flags) {
	 char*  start = buf;
	int32_t rv;
	do {
		rv = yang_socket_recv(fd, buf, bufsz, flags);
		if (rv > 0) {
			// successfully read bytes from the socket 
			buf = (char*)buf + rv;
			bufsz -= rv;
		}else if (rv < 0) {
			int err = WSAGetLastError();
			if (err != WSAEWOULDBLOCK) {
				// an error occurred that wasn't "nothing to read".
				return MQTT_ERROR_SOCKET_ERROR;
			}
		}
	} while (rv > 0 && bufsz > 0);

	return (char*)buf - start;
}
#else
int32_t yang_mqtt_sendmsg(yang_socket_t fd, const void* buf, int32_t len) {
	YangmqttErrors error = 0;
	int32_t sent = 0;
	while (sent < len) {
		int32_t rv = yang_socket_send(fd, (char*)buf + sent, len - sent);
		if (rv < 0) {
			if (errno == EAGAIN) {
				// should call send later again
				break;
			}
			yang_error("mqtt recv fail(%d)", GetSockError());
			error = MQTT_ERROR_SOCKET_ERROR;
			break;
		}
		if (rv == 0) {
			// is this possible? maybe OS bug.
			error = MQTT_ERROR_SOCKET_ERROR;
			break;
		}
		sent += rv;
	}
	if (sent == 0) {
		return error;
	}
	return sent;
}
int32_t yang_mqtt_recvmsg(yang_socket_t fd, void* buf, int32_t bufsz, int32_t flags) {
    const void *const start = buf;
    YangmqttErrors error = 0;
    int32_t rv;

    do {
        rv = yang_socket_recv(fd, buf, bufsz, flags);

        if (rv == 0) {

            //recv returns 0 when the socket is (half) closed by the peer.
            // Raise an error to trigger a reconnect.

        	yang_error("mqtt recv fail(%d)",GetSockError());
            error = MQTT_ERROR_SOCKET_ERROR;
            break;
        }
        if (rv < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // should call recv later again
                break;
            }
            // an error occurred that wasn't "nothing to read".
            yang_error("mqtt recv fail(%d)",GetSockError());
            error = MQTT_ERROR_SOCKET_ERROR;
            break;
        }
        buf = (char*)buf + rv;
        bufsz -= (unsigned long)rv;
    } while (bufsz > 0);
    if (buf == start) {
        return error;
    }
    return (char*)buf - (const char*)start;
}
#endif

