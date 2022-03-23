CONFIG -= qt

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++14

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += HAVE_CONFIG_H
HOME_BASE=../
INCLUDEPATH += $$HOME_BASE/libmetasrtp3/include
INCLUDEPATH += $$HOME_BASE/libmetasrtp3/crypto/include
INCLUDEPATH += $$HOME_BASE/thirdparty/user_include
INCLUDEPATH += $$HOME_BASE/thirdparty/user_include/gmssl
unix{
  CONFIG(debug, debug|release) {
        DESTDIR += $$HOME_BASE/bin/lib_debug
    }else{

        DESTDIR += $$HOME_BASE/bin/lib_release
    }
}
win32{
    DEFINES += __x86_64__
    DEFINES += HAVE_WINSOCK2_H

  CONFIG(debug, debug|release) {
        DESTDIR += $$HOME_BASE\bin\lib_win_debug
    }else{
        DESTDIR += $$HOME_BASE\bin\lib_win_release
    }

    msvc{
        QMAKE_CFLAGS += /utf-8
        QMAKE_CXXFLAGS += /utf-8
       # QMAKE_CXXFLAGS += /source-charset:utf-8 /execution-charset:utf-8
       # DEFINES +=HAVE_STRUCT_TIMESPEC
        #DEFINES +=WIN32_LEAN_AND_MEAN

    }

}
# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    crypto/cipher/aes_gcm_ossl.c \
    crypto/cipher/aes_icm_ossl.c \
    crypto/cipher/cipher.c \
    crypto/cipher/cipher_test_cases.c \
    crypto/cipher/null_cipher.c \
    crypto/hash/auth.c \
    crypto/hash/auth_test_cases.c \
    crypto/hash/hmac_ossl.c \
    crypto/hash/null_auth.c \
    crypto/kernel/alloc.c \
    crypto/kernel/crypto_kernel.c \
    crypto/kernel/err.c \
    crypto/kernel/key.c \
    crypto/math/datatypes.c \
    crypto/replay/rdb.c \
    crypto/replay/rdbx.c \
    srtp/srtp.c


HEADERS += \
    crypto/cipher/cipher_test_cases.h \
    crypto/hash/auth_test_cases.h \
    crypto/include/aes.h \
    crypto/include/aes_gcm.h \
    crypto/include/aes_icm.h \
    crypto/include/aes_icm_ext.h \
    crypto/include/alloc.h \
    crypto/include/auth.h \
    crypto/include/cipher.h \
    crypto/include/cipher_priv.h \
    crypto/include/cipher_types.h \
    crypto/include/config.h \
    crypto/include/crypto_kernel.h \
    crypto/include/crypto_types.h \
    crypto/include/datatypes.h \
    crypto/include/err.h \
    crypto/include/hmac.h \
    crypto/include/integers.h \
    crypto/include/key.h \
    crypto/include/null_auth.h \
    crypto/include/null_cipher.h \
    crypto/include/rdb.h \
    crypto/include/rdbx.h \
    crypto/include/sha1.h \
    include/srtp.h \
    include/srtp_priv.h


# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    include/srtp2/meson.build
