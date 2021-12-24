#ifndef INCLUDE_YANGUTIL_YANGTYPE_H_
#define INCLUDE_YANGUTIL_YANGTYPE_H_
#include <stdio.h>
#include <stdint.h>
#include <yangutil/yang_unistd.h>
#include "YangErrorCode.h"
#include "../Yang_Config.h"


#define yang_delete(a) {if( (a)) {delete (a); (a) = NULL;}}
#define yang_deleteA(a) {if( (a)) {delete[] (a); (a) = NULL;}}
#define yang_free(a) {if( (a)) {free((a)); (a) = NULL;}}
#define yang_min(a, b) (((a) < (b))? (a) : (b))
#define yang_max(a, b) (((a) < (b))? (b) : (a))

#define Yang_Rtmp 0
#define Yang_Srt 1
#define Yang_Webrtc 2

#define YANG_VIDEO_ENCODE_BUFFER_LEN 1024 * 1024

#define YangAutoFree(className, instance) \
impl_YangAutoFree<className> _auto_free_##instance(&instance, false)
#define YangAutoFreeA(className, instance) \
impl_YangAutoFree<className> _auto_free_array_##instance(&instance, true)
template<class T>
class impl_YangAutoFree
{
private:
    T** ptr;
    bool is_array;
public:
    impl_YangAutoFree(T** p, bool array) {
        ptr = p;
        is_array = array;
    }

    virtual ~impl_YangAutoFree() {
        if (ptr == NULL || *ptr == NULL) {
            return;
        }

        if (is_array) {
            delete[] *ptr;
        } else {
            delete *ptr;
        }

        *ptr = NULL;
    }
};
#endif /* INCLUDE_YANGUTIL_YANGTYPE_H_ */
