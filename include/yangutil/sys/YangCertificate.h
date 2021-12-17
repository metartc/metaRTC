#ifndef INCLUDE_YANGUTIL_SYS_YANGCERTIFICATE_H_
#define INCLUDE_YANGUTIL_SYS_YANGCERTIFICATE_H_
#include <stdint.h>
#include <string>
class YangCertificate{
public:
	YangCertificate(){};
	virtual ~YangCertificate(){};
	virtual int32_t init()=0;
	virtual    std::string get_fingerprint()=0;
	    // whether is ecdsa
	virtual   bool is_ecdsa()=0;
};



#endif /* INCLUDE_YANGUTIL_SYS_YANGCERTIFICATE_H_ */
