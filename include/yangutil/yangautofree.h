//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef INCLUDE_YANGUTIL_YANGAUTOFREE_H_
#define INCLUDE_YANGUTIL_YANGAUTOFREE_H_
#include <yangutil/yangtype.h>


#ifdef __cplusplus
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

#endif


#endif /* INCLUDE_YANGUTIL_YANGAUTOFREE_H_ */
