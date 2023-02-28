//
// Copyright (c) 2019-2023 yanggaofeng
// base on Liam Bindle MQTT-C
//

#ifndef _YANGMQTT_H_
#define _YANGMQTT_H_
#include <yangutil/sys/YangTime.h>
#include <yangutil/sys/YangSocket.h>
#include <yangutil/sys/YangThread.h>

#include <limits.h>

#define yang_pal_time() time(NULL)

#if Yang_Enable_Mqtt

#ifdef _WIN32
typedef SSIZE_T ssize_t;
#endif

 typedef enum {
    MQTT_CONTROL_CONNECT=1u,
    MQTT_CONTROL_CONNACK=2u,
    MQTT_CONTROL_PUBLISH=3u,
    MQTT_CONTROL_PUBACK=4u,
    MQTT_CONTROL_PUBREC=5u,
    MQTT_CONTROL_PUBREL=6u,
    MQTT_CONTROL_PUBCOMP=7u,
    MQTT_CONTROL_SUBSCRIBE=8u,
    MQTT_CONTROL_SUBACK=9u,
    MQTT_CONTROL_UNSUBSCRIBE=10u,
    MQTT_CONTROL_UNSUBACK=11u,
    MQTT_CONTROL_PINGREQ=12u,
    MQTT_CONTROL_PINGRESP=13u,
    MQTT_CONTROL_DISCONNECT=14u
}MQTTControlPacketType;


typedef struct {
    MQTTControlPacketType control_type;
    uint32_t  control_flags: 4;
    uint32_t remaining_length;
}yang_mqtt_fixed_header;

/**
 * @brief The protocol identifier for MQTT v3.1.1.
 * @ingroup packers
 * 
 * @see <a href="http://docs.oasis-open.org/mqtt/mqtt/v3.1.1/os/mqtt-v3.1.1-os.html#_Toc398718030">
 * MQTT v3.1.1: CONNECT Variable Header.
 * </a>  
 */
#define YANG_MQTT_PROTOCOL_LEVEL 0x04


#define __ALL_MQTT_ERRORS(MQTT_ERROR)                    \
    MQTT_ERROR(MQTT_ERROR_NULLPTR)                       \
    MQTT_ERROR(MQTT_ERROR_CONTROL_FORBIDDEN_TYPE)        \
    MQTT_ERROR(MQTT_ERROR_CONTROL_INVALID_FLAGS)         \
    MQTT_ERROR(MQTT_ERROR_CONTROL_WRONG_TYPE)            \
    MQTT_ERROR(MQTT_ERROR_CONNECT_CLIENT_ID_REFUSED)     \
    MQTT_ERROR(MQTT_ERROR_CONNECT_NULL_WILL_MESSAGE)     \
    MQTT_ERROR(MQTT_ERROR_CONNECT_FORBIDDEN_WILL_QOS)    \
    MQTT_ERROR(MQTT_ERROR_CONNACK_FORBIDDEN_FLAGS)       \
    MQTT_ERROR(MQTT_ERROR_CONNACK_FORBIDDEN_CODE)        \
    MQTT_ERROR(MQTT_ERROR_PUBLISH_FORBIDDEN_QOS)         \
    MQTT_ERROR(MQTT_ERROR_SUBSCRIBE_TOO_MANY_TOPICS)     \
    MQTT_ERROR(MQTT_ERROR_MALFORMED_RESPONSE)            \
    MQTT_ERROR(MQTT_ERROR_UNSUBSCRIBE_TOO_MANY_TOPICS)   \
    MQTT_ERROR(MQTT_ERROR_RESPONSE_INVALID_CONTROL_TYPE) \
    MQTT_ERROR(MQTT_ERROR_CONNECT_NOT_CALLED)            \
    MQTT_ERROR(MQTT_ERROR_SEND_BUFFER_IS_FULL)           \
    MQTT_ERROR(MQTT_ERROR_SOCKET_ERROR)                  \
    MQTT_ERROR(MQTT_ERROR_MALFORMED_REQUEST)             \
    MQTT_ERROR(MQTT_ERROR_RECV_BUFFER_TOO_SMALL)         \
    MQTT_ERROR(MQTT_ERROR_ACK_OF_UNKNOWN)                \
    MQTT_ERROR(MQTT_ERROR_NOT_IMPLEMENTED)               \
    MQTT_ERROR(MQTT_ERROR_CONNECTION_REFUSED)            \
    MQTT_ERROR(MQTT_ERROR_SUBSCRIBE_FAILED)              \
    MQTT_ERROR(MQTT_ERROR_CONNECTION_CLOSED)             \
    MQTT_ERROR(MQTT_ERROR_INITIAL_RECONNECT)             \
    MQTT_ERROR(MQTT_ERROR_INVALID_REMAINING_LENGTH)      \
    MQTT_ERROR(MQTT_ERROR_CLEAN_SESSION_IS_REQUIRED)     \
    MQTT_ERROR(MQTT_ERROR_RECONNECTING)

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

typedef enum{
    MQTT_ERROR_UNKNOWN=INT_MIN,
    __ALL_MQTT_ERRORS(GENERATE_ENUM)
    MQTT_OK = 1
}YangmqttErrors;

const char* yang_mqtt_error_str(YangmqttErrors error);

ssize_t __yang_mqtt_pack_uint16(uint8_t *buf, uint16_t integer);
uint16_t __yang_mqtt_unpack_uint16(const uint8_t *buf);
ssize_t __yang_mqtt_pack_str(uint8_t *buf, const char* str);

#define __yang_mqtt_packed_cstrlen(x) (2 + (unsigned int)strlen(x))


typedef enum{
    MQTT_CONNACK_ACCEPTED = 0u,
    MQTT_CONNACK_REFUSED_PROTOCOL_VERSION = 1u,
    MQTT_CONNACK_REFUSED_IDENTIFIER_REJECTED = 2u,
    MQTT_CONNACK_REFUSED_SERVER_UNAVAILABLE = 3u,
    MQTT_CONNACK_REFUSED_BAD_USER_NAME_OR_PASSWORD = 4u,
    MQTT_CONNACK_REFUSED_NOT_AUTHORIZED = 5u
}MQTTConnackReturnCode;

typedef struct {
   uint8_t session_present_flag;
   MQTTConnackReturnCode return_code;
}yang_mqtt_response_connack;


typedef struct  {
    uint8_t dup_flag;
    uint8_t qos_level;
    uint8_t retain_flag;
    uint16_t topic_name_size;
    const void* topic_name;
    uint16_t packet_id;
    const void* application_message;
    size_t application_message_size;
}yang_mqtt_response_publish;

typedef struct {
    uint16_t packet_id;
}yang_mqtt_response_puback;


typedef struct {
    uint16_t packet_id;
}yang_mqtt_response_pubrec;


typedef struct {
    uint16_t packet_id;
}yang_mqtt_response_pubrel;

typedef struct{
    uint16_t packet_id;
}yang_mqtt_response_pubcomp;

typedef enum{
    MQTT_SUBACK_SUCCESS_MAX_QOS_0 = 0u,
    MQTT_SUBACK_SUCCESS_MAX_QOS_1 = 1u,
    MQTT_SUBACK_SUCCESS_MAX_QOS_2 = 2u,
    MQTT_SUBACK_FAILURE           = 128u
}MQTTSubackReturnCodes;

typedef struct{
    uint16_t packet_id;
    const uint8_t *return_codes;
    size_t num_return_codes;
}yang_mqtt_response_suback;

typedef struct {
    uint16_t packet_id;
}yang_mqtt_response_unsuback;

typedef struct{
  int dummy;
}yang_mqtt_response_pingresp;

typedef struct{
    yang_mqtt_fixed_header fixed_header;
    union {
        yang_mqtt_response_connack  connack;
        yang_mqtt_response_publish  publish;
        yang_mqtt_response_puback   puback;
        yang_mqtt_response_pubrec   pubrec;
        yang_mqtt_response_pubrel   pubrel;
        yang_mqtt_response_pubcomp  pubcomp;
        yang_mqtt_response_suback   suback;
        yang_mqtt_response_unsuback unsuback;
        yang_mqtt_response_pingresp pingresp;
    } decoded;
}yang_mqtt_response;

ssize_t yang_mqtt_unpack_fixed_header(yang_mqtt_response *response, const uint8_t *buf, size_t bufsz);
ssize_t yang_mqtt_unpack_connack_response (yang_mqtt_response *yang_mqtt_response, const uint8_t *buf);
ssize_t yang_mqtt_unpack_publish_response (yang_mqtt_response *yang_mqtt_response, const uint8_t *buf);
ssize_t yang_mqtt_unpack_pubxxx_response(yang_mqtt_response *yang_mqtt_response, const uint8_t *buf);
ssize_t yang_mqtt_unpack_suback_response(yang_mqtt_response *yang_mqtt_response, const uint8_t *buf);
ssize_t yang_mqtt_unpack_unsuback_response(yang_mqtt_response *yang_mqtt_response, const uint8_t *buf);
ssize_t yang_mqtt_unpack_response(yang_mqtt_response* response, const uint8_t *buf, size_t bufsz);
ssize_t yang_mqtt_pack_fixed_header(uint8_t *buf, size_t bufsz, const yang_mqtt_fixed_header *fixed_header);

typedef enum{
    MQTT_CONNECT_RESERVED = 1u,
    MQTT_CONNECT_CLEAN_SESSION = 2u,
    MQTT_CONNECT_WILL_FLAG = 4u,
    MQTT_CONNECT_WILL_QOS_0 = (0u & 0x03) << 3,
    MQTT_CONNECT_WILL_QOS_1 = (1u & 0x03) << 3,
    MQTT_CONNECT_WILL_QOS_2 = (2u & 0x03) << 3,
    MQTT_CONNECT_WILL_RETAIN = 32u,
    MQTT_CONNECT_PASSWORD = 64u,
    MQTT_CONNECT_USER_NAME = 128u
}MQTTConnectFlags;

ssize_t yang_mqtt_pack_connection_request(uint8_t* buf, size_t bufsz,
                                     const char* client_id,
                                     const char* will_topic,
                                     const void* will_message,
                                     size_t will_message_size,
                                     const char* user_name,
                                     const char* password,
                                     uint8_t connect_flags,
                                     uint16_t keep_alive);


enum MQTTPublishFlags {
    MQTT_PUBLISH_DUP = 8u,
    MQTT_PUBLISH_QOS_0 = ((0u << 1) & 0x06),
    MQTT_PUBLISH_QOS_1 = ((1u << 1) & 0x06),
    MQTT_PUBLISH_QOS_2 = ((2u << 1) & 0x06),
    MQTT_PUBLISH_QOS_MASK = ((3u << 1) & 0x06),
    MQTT_PUBLISH_RETAIN = 0x01
};


ssize_t yang_mqtt_pack_publish_request(uint8_t *buf, size_t bufsz,
                                  const char* topic_name,
                                  uint16_t packet_id,
                                  const void* application_message,
                                  size_t application_message_size,
                                  uint8_t publish_flags);


ssize_t yang_mqtt_pack_pubxxx_request(uint8_t *buf, size_t bufsz,
                                 MQTTControlPacketType control_type,
                                 uint16_t packet_id);


#define MQTT_SUBSCRIBE_REQUEST_MAX_NUM_TOPICS 8

ssize_t yang_mqtt_pack_subscribe_request(uint8_t *buf, size_t bufsz,
                                    unsigned int packet_id, 
                                    ...); //null terminated

#define MQTT_UNSUBSCRIBE_REQUEST_MAX_NUM_TOPICS 8

ssize_t yang_mqtt_pack_unsubscribe_request(uint8_t *buf, size_t bufsz,
                                      unsigned int packet_id, 
                                      ...); // null terminated

ssize_t yang_mqtt_pack_ping_request(uint8_t *buf, size_t bufsz);
ssize_t yang_mqtt_pack_disconnect(uint8_t *buf, size_t bufsz);

typedef enum{
    MQTT_QUEUED_UNSENT,
    MQTT_QUEUED_AWAITING_ACK,
    MQTT_QUEUED_COMPLETE
}MQTTQueuedMessageState;


typedef struct{
    uint8_t *start;
    size_t size;
    MQTTQueuedMessageState state;
    yang_time_t time_sent;
    MQTTControlPacketType control_type;
    uint16_t packet_id;
}yang_mqtt_queued_message;

typedef struct{
    void *mem_start;
    void *mem_end;
    uint8_t *curr;
    size_t curr_sz;
    yang_mqtt_queued_message *queue_tail;
}yang_mqtt_message_queue;

void yang_mqtt_mq_init(yang_mqtt_message_queue *mq, void *buf, size_t bufsz);
void yang_mqtt_mq_clean(yang_mqtt_message_queue *mq);
yang_mqtt_queued_message* yang_mqtt_mq_register(yang_mqtt_message_queue *mq, size_t nbytes);
yang_mqtt_queued_message* yang_mqtt_mq_find(yang_mqtt_message_queue *mq, MQTTControlPacketType control_type, uint16_t *packet_id);

#define yang_mqtt_mq_get(mq_ptr, index) (((yang_mqtt_queued_message*) ((mq_ptr)->mem_end)) - 1 - index)
#define yang_mqtt_mq_length(mq_ptr) (((yang_mqtt_queued_message*) ((mq_ptr)->mem_end)) - (mq_ptr)->queue_tail)
#define yang_mqtt_mq_currsz(mq_ptr) (mq_ptr->curr >= (uint8_t*) ((mq_ptr)->queue_tail - 1)) ? 0 : ((uint8_t*) ((mq_ptr)->queue_tail - 1)) - (mq_ptr)->curr


typedef struct{
    yang_socket_t socketfd;
    uint16_t pid_lfsr;
    uint16_t keep_alive;
    int32_t number_of_keep_alives;
    size_t send_offset;
    yang_time_t time_of_last_send;
    YangmqttErrors error;

    int32_t response_timeout;
    int32_t number_of_timeouts;

    double typical_response_time;

    void (*publish_response_callback)(void* state, yang_mqtt_response_publish *publish);
    void* publish_response_callback_state;

    void (*reconnect_callback)(void*, void**);
    void* reconnect_state;

    struct {
        uint8_t *mem_start;
        size_t mem_size;
        uint8_t *curr;
        size_t curr_sz;
    } recv_buffer;

    yang_thread_mutex_t mutex;
    yang_mqtt_message_queue mq;
}yang_mqtt_client;


uint16_t __yang_mqtt_next_pid(yang_mqtt_client *client);
ssize_t __yang_mqtt_send(yang_mqtt_client *client);
ssize_t __yang_mqtt_recv(yang_mqtt_client *client);
YangmqttErrors yang_mqtt_sync(yang_mqtt_client *client);
YangmqttErrors yang_mqtt_init(yang_mqtt_client *client,
                          yang_socket_t sockfd,
                          uint8_t *sendbuf, size_t sendbufsz,
                          uint8_t *recvbuf, size_t recvbufsz,
                          void (*publish_response_callback)(void* state, yang_mqtt_response_publish *publish));


void yang_mqtt_init_reconnect(yang_mqtt_client *client,
                         void (*reconnect_callback)(void *client, void** state),
                         void *reconnect_state,
                         void (*publish_response_callback)(void* state, yang_mqtt_response_publish *publish));


void yang_mqtt_reinit(yang_mqtt_client* client,
                 yang_socket_t socketfd,
                 uint8_t *sendbuf, size_t sendbufsz,
                 uint8_t *recvbuf, size_t recvbufsz);

YangmqttErrors yang_mqtt_connect(yang_mqtt_client *client,
                             const char* client_id,
                             const char* will_topic,
                             const void* will_message,
                             size_t will_message_size,
                             const char* user_name,
                             const char* password,
                             uint8_t connect_flags,
                             uint16_t keep_alive);

YangmqttErrors yang_mqtt_publish(yang_mqtt_client *client,
                             const char* topic_name,
                             const void* application_message,
                             size_t application_message_size,
                             uint8_t publish_flags);


ssize_t __yang_mqtt_puback(yang_mqtt_client *client, uint16_t packet_id);
ssize_t __yang_mqtt_pubrec(yang_mqtt_client *client, uint16_t packet_id);
ssize_t __yang_mqtt_pubrel(yang_mqtt_client *client, uint16_t packet_id);
ssize_t __yang_mqtt_pubcomp(yang_mqtt_client *client, uint16_t packet_id);
YangmqttErrors yang_mqtt_subscribe(yang_mqtt_client *client,
                               const char* topic_name,
                               int32_t max_qos_level);

YangmqttErrors yang_mqtt_unsubscribe(yang_mqtt_client *client,
                                 const char* topic_name);

YangmqttErrors yang_mqtt_ping(yang_mqtt_client *client);
YangmqttErrors __yang_mqtt_ping(yang_mqtt_client *client);
YangmqttErrors yang_mqtt_disconnect(yang_mqtt_client *client);
YangmqttErrors yang_mqtt_reconnect(yang_mqtt_client *client);

int32_t yang_mqtt_sendmsg(yang_socket_t fd, const void* buf, int32_t len);
int32_t yang_mqtt_recvmsg(yang_socket_t fd, void* buf, int32_t bufsz, int32_t flags);
#endif
#endif
