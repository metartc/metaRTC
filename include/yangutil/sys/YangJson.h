#ifndef INCLUDE_YANGUTIL_SYS_YANGJSON_H_
#define INCLUDE_YANGUTIL_SYS_YANGJSON_H_
#include <yangutil/yangtype.h>
#if Yang_HaveJson
#include <vector>
#include <string>
using namespace std;
struct YangJsonData{
	string key;
	string value;
};

void yang_gen_jsonstr(vector<YangJsonData> &jsons,string &outstr);

#endif
#endif /* INCLUDE_YANGUTIL_SYS_YANGJSON_H_ */
