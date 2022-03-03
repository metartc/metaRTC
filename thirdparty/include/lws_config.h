/* lws_config.h  Generated from lws_config.h.in  */

#ifndef NDEBUG
	#ifndef _DEBUG
		#define _DEBUG
	#endif
#endif

#define LWS_INSTALL_DATADIR "/usr/local/share"
#define LWS_INSTALL_LIBDIR "/usr/local/lib"
#define LWS_LIBRARY_VERSION_MAJOR 4
#define LWS_LIBRARY_VERSION_MINOR 3
#define LWS_LIBRARY_VERSION_PATCH_ELABORATED 0-unknown
#define LWS_LIBRARY_VERSION_PATCH 0

/* LWS_LIBRARY_VERSION_NUMBER looks like 1005001 for e.g. version 1.5.1 */
#define LWS_LIBRARY_VERSION_NUMBER (LWS_LIBRARY_VERSION_MAJOR * 1000000) + \
					(LWS_LIBRARY_VERSION_MINOR * 1000) + \
					LWS_LIBRARY_VERSION_PATCH
#define LWS_MAX_SMP 1

/* #undef LWS_ESP_PLATFORM */
/* #undef LWS_LIBRARY_VERSION_NUMBER */

/* #undef LWS_EXT_PTHREAD_LIBRARIES */

/* #undef LWS_AVOID_SIGPIPE_IGN */
#define LWS_BUILD_HASH "unknown"
/* #undef LWS_BUILTIN_GETIFADDRS */
#define LWS_CLIENT_HTTP_PROXYING
/* #undef LWS_DETECTED_PLAT_IOS */
/* #undef LWS_FALLBACK_GETHOSTBYNAME */
#define LWS_HAS_INTPTR_T
#define LWS_HAS_GETOPT_LONG
/* #undef LWS_HAVE__ATOI64 */
#define LWS_HAVE_ATOLL
#define LWS_HAVE_BN_bn2binpad
#define LWS_HAVE_CLOCK_GETTIME
#define LWS_HAVE_EC_POINT_get_affine_coordinates
#define LWS_HAVE_EC_KEY_new_by_curve_name
#define LWS_HAVE_ECDSA_SIG_set0
#define LWS_HAVE_EVP_MD_CTX_free
#define LWS_HAVE_EVP_aes_128_wrap
#define LWS_HAVE_EVP_aes_128_cfb8
#define LWS_HAVE_EVP_aes_128_cfb128
#define LWS_HAVE_EVP_aes_192_cfb8
#define LWS_HAVE_EVP_aes_192_cfb128
#define LWS_HAVE_EVP_aes_256_cfb8
#define LWS_HAVE_EVP_aes_256_cfb128
#define LWS_HAVE_EVP_aes_128_ofb
#define LWS_HAVE_EVP_aes_128_xts
#define LWS_HAVE_EVP_aes_128_ctr
#define LWS_HAVE_EVP_aes_128_ecb
#define LWS_HAVE_EVP_PKEY_new_raw_private_key
#define LWS_HAVE_EXECVPE
#define LWS_HAVE_LOCALTIME_R
#define LWS_HAVE_GMTIME_R
#define LWS_HAVE_CTIME_R
#define LWS_HAVE_GETGRGID_R
#define LWS_HAVE_GETGRNAM_R
#define LWS_HAVE_GETPWUID_R
#define LWS_HAVE_GETPWNAM_R
/* #undef LWS_HAVE_LIBCAP */
#define LWS_HAVE_HMAC_CTX_new
#define LWS_HAVE_MALLOC_H
#define LWS_HAVE_MALLOC_TRIM
#define LWS_HAVE_MALLOC_USABLE_SIZE
/* #undef LWS_HAVE_mbedtls_md_setup */
/* #undef LWS_HAVE_mbedtls_net_init */
/* #undef LWS_HAVE_mbedtls_rsa_complete */
/* #undef LWS_HAVE_mbedtls_internal_aes_encrypt */
/* #undef LWS_HAVE_mbedtls_ssl_conf_alpn_protocols */
/* #undef LWS_HAVE_mbedtls_ssl_get_alpn_protocol */
/* #undef LWS_HAVE_mbedtls_ssl_conf_sni */
/* #undef LWS_HAVE_mbedtls_ssl_set_hs_ca_chain */
/* #undef LWS_HAVE_mbedtls_ssl_set_hs_own_cert */
/* #undef LWS_HAVE_mbedtls_ssl_set_hs_authmode */
/* #undef LWS_HAVE_mbedtls_ssl_set_verify */
/* #undef LWS_HAVE_mbedtls_x509_crt_parse_file */
/* #undef LWS_HAVE_MBEDTLS_NET_SOCKETS */
/* #undef LWS_HAVE_MBEDTLS_AUTH_KEY_ID */
/* #undef LWS_HAVE_NEW_UV_VERSION_H */
#define LWS_HAVE_OPENSSL_ECDH_H
#define LWS_HAVE_OPENSSL_STACK
#define LWS_HAVE_PIPE2
#define LWS_HAVE_EVENTFD
#define LWS_HAVE_PTHREAD_H
#define LWS_HAVE_RSA_SET0_KEY
/* #undef LWS_HAVE_RSA_verify_pss_mgf1 */
#define LWS_HAVE_SSL_CTX_get0_certificate
/* #undef LWS_HAVE_SSL_CTX_load_verify_file */
/* #undef LWS_HAVE_SSL_CTX_load_verify_dir */
#define LWS_HAVE_SSL_CTX_set1_param
/* #undef LWS_HAVE_SSL_CTX_set_ciphersuites */
#define LWS_HAVE_SSL_EXTRA_CHAIN_CERTS
#define LWS_HAVE_SSL_get0_alpn_selected
/* #undef LWS_HAVE_SSL_CTX_EVP_PKEY_new_raw_private_key */
#define LWS_HAVE_SSL_set_alpn_protos
#define LWS_HAVE_SSL_SET_INFO_CALLBACK
#define LWS_HAVE_SSL_SESSION_set_time
/* #undef LWS_HAVE_SSL_SESSION_up_ref */
/* #undef LWS_HAVE__STAT32I64 */
#define LWS_HAVE_STDINT_H
/* #undef LWS_HAVE_SYS_CAPABILITY_H */
#define LWS_HAVE_TIMEGM
#define LWS_HAVE_TLS_CLIENT_METHOD
#define LWS_HAVE_TLSV1_2_CLIENT_METHOD
#define LWS_HAVE_SUSECONDS_T
/* #undef LWS_HAVE_UV_VERSION_H */
#define LWS_HAVE_VFORK
#define LWS_HAVE_X509_get_key_usage
#define LWS_HAVE_X509_VERIFY_PARAM_set1_host
#define LWS_LIBRARY_VERSION "4.3.0-unknown"
#define LWS_LOGGING_BITFIELD_CLEAR 0
#define LWS_LOGGING_BITFIELD_SET 0
#define LWS_LOG_TAG_LIFECYCLE
/* #undef LWS_MINGW_SUPPORT */
/* #undef LWS_NO_CLIENT */
#define LWS_NO_DAEMONIZE
#define LWS_OPENSSL_CLIENT_CERTS "../share"
#define LWS_OPENSSL_SUPPORT
/* #undef LWS_PLAT_OPTEE */
#define LWS_PLAT_UNIX
/* #undef LWS_PLAT_FREERTOS */
/* #undef LWS_ROLE_CGI */
/* #undef LWS_ROLE_DBUS */
#define LWS_ROLE_H1
#define LWS_ROLE_H2
#define LWS_ROLE_RAW
#define LWS_ROLE_RAW_FILE
/* #undef LWS_ROLE_RAW_PROXY */
#define LWS_ROLE_WS
/* #undef LWS_ROLE_MQTT */
/* #undef LWS_SHA1_USE_OPENSSL_NAME */
#define LWS_SSL_CLIENT_USE_OS_CA_CERTS
/* #undef LWS_SSL_SERVER_WITH_ECDH_CERT */
#define LWS_SUPPRESS_DEPRECATED_API_WARNINGS
/* #undef LWS_TLS_LOG_PLAINTEXT_RX */
/* #undef LWS_TLS_LOG_PLAINTEXT_TX */
/* #undef LWS_WITH_ABSTRACT */
/* #undef LWS_WITH_ACCESS_LOG */
/* #undef LWS_WITH_ACME */
/* #undef LWS_WITH_ALSA */
/* #undef LWS_WITH_SYS_ASYNC_DNS */
/* #undef LWS_WITH_BORINGSSL */
/* #undef LWS_WITH_CGI */
#define LWS_WITH_CONMON
/* #undef LWS_WITH_COSE */
#define LWS_WITH_CUSTOM_HEADERS
/* #undef LWS_WITH_DEPRECATED_LWS_DLL */
/* #undef LWS_WITH_DETAILED_LATENCY */
#define LWS_WITH_DIR
/* #undef LWS_WITH_DRIVERS */
/* #undef LWS_WITH_ESP32 */
/* #undef LWS_HAVE_EVBACKEND_LINUXAIO */
/* #undef LWS_HAVE_EVBACKEND_IOURING */
/* #undef LWS_WITH_EXTERNAL_POLL */
#define LWS_WITH_FILE_OPS
/* #undef LWS_WITH_FSMOUNT */
/* #undef LWS_WITH_FTS */
/* #undef LWS_WITH_GENCRYPTO */
/* #undef LWS_WITH_GENERIC_SESSIONS */
/* #undef LWS_WITH_GLIB */
/* #undef LWS_WITH_GTK */
#define LWS_WITH_HTTP2
#define LWS_WITH_HTTP_BASIC_AUTH
/* #undef LWS_WITH_HTTP_BROTLI */
/* #undef LWS_HTTP_HEADERS_ALL */
/* #undef LWS_WITH_HTTP_PROXY */
/* #undef LWS_WITH_HTTP_STREAM_COMPRESSION */
#define LWS_WITH_HTTP_UNCOMMON_HEADERS
/* #undef LWS_WITH_IPV6 */
/* #undef LWS_WITH_JOSE */
/* #undef LWS_WITH_CBOR */
#define LWS_WITH_CBOR_FLOAT
#define LWS_WITH_LEJP
/* #undef LWS_WITH_LIBEV */
/* #undef LWS_WITH_LIBEVENT */
/* #undef LWS_WITH_LIBUV */
/* #undef LWS_WITH_SDEVENT */
#define LWS_WITH_LWSAC
#define LWS_LOGS_TIMESTAMP
/* #undef LWS_WITH_MBEDTLS */
/* #undef LWS_WITH_MINIZ */
#define LWS_WITH_NETLINK
#define LWS_WITH_NETWORK
/* #undef LWS_WITH_NO_LOGS */
#define LWS_WITH_CACHE_NSCOOKIEJAR
#define LWS_WITH_CLIENT
#define LWS_WITHOUT_EXTENSIONS
#define LWS_WITH_SERVER
/* #undef LWS_WITH_SPAWN */
/* #undef LWS_WITH_PEER_LIMITS */
/* #undef LWS_WITH_PLUGINS */
/* #undef LWS_WITH_PLUGINS_BUILTIN */
/* #undef LWS_WITH_POLARSSL */
#define LWS_WITH_POLL
/* #undef LWS_WITH_RANGES */
/* #undef LWS_WITH_RFC6724 */
/* #undef LWS_WITH_SECURE_STREAMS */
/* #undef LWS_WITH_SECURE_STREAMS_CPP */
/* #undef LWS_WITH_SECURE_STREAMS_SYS_AUTH_API_AMAZON_COM */
/* #undef LWS_WITH_SECURE_STREAMS_PROXY_API */
/* #undef LWS_WITH_SECURE_STREAMS_STATIC_POLICY_ONLY */
/* #undef LWS_WITH_SECURE_STREAMS_AUTH_SIGV4 */
/* #undef LWS_WITH_SECURE_STREAMS_BUFFER_DUMP */
/* #undef LWS_WITH_SS_DIRECT_PROTOCOL_STR */
/* #undef LWS_WITH_SELFTESTS */
/* #undef LWS_WITH_SEQUENCER */
/* #undef LWS_WITH_SERVER_STATUS */
#define LWS_WITH_SYS_SMD
/* #undef LWS_WITH_SMTP */
/* #undef LWS_WITH_SOCKS5 */
/* #undef LWS_WITH_STATEFUL_URLDECODE */
/* #undef LWS_WITH_STATS */
/* #undef LWS_WITH_STRUCT_SQLITE3 */
/* #undef LWS_WITH_STRUCT_JSON */
/* #undef LWS_WITH_SUL_DEBUGGING */
/* #undef LWS_WITH_SQLITE3 */
/* #undef LWS_WITH_SYS_DHCP_CLIENT */
/* #undef LWS_WITH_SYS_FAULT_INJECTION */
/* #undef LWS_WITH_SYS_METRICS */
/* #undef LWS_WITH_SYS_NTPCLIENT */
#define LWS_WITH_SYS_STATE
/* #undef LWS_WITH_THREADPOOL */
#define LWS_WITH_TLS
/* #undef LWS_WITH_TLS_JIT_TRUST */
#define LWS_WITH_TLS_SESSIONS
#define LWS_WITH_UDP
/* #undef LWS_WITH_ULOOP */
#define LWS_WITH_UNIX_SOCK
/* #undef LWS_WITH_ZIP_FOPS */
/* #undef USE_OLD_CYASSL */
/* #undef USE_WOLFSSL */
/* #undef LWS_WITH_EVENT_LIBS */
#define LWS_WITH_EVLIB_PLUGINS
/* #undef LWS_WITH_LIBUV_INTERNAL */
/* #undef LWS_WITH_PLUGINS_API */
#define LWS_HAVE_RTA_PREF

